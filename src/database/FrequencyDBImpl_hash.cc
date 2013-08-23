///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_hash.cc 272 2007-01-06 19:37:27Z brian $
//
// Copyright (C) 2007 Burton Computer Corporation
// ALL RIGHTS RESERVED
//
// This program is open source software; you can redistribute it
// and/or modify it under the terms of the Q Public License (QPL)
// version 1.0. Use of this software in whole or in part, including
// linking it (modified or unmodified) into other programs is
// subject to the terms of the QPL.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Q Public License for more details.
//
// You should have received a copy of the Q Public License
// along with this program; see the file LICENSE.txt.  If not, visit
// the Burton Computer Corporation or CoolDevTools web site
// QPL pages at:
//
//    http://www.burton-computer.com/qpl.html
//    http://www.cooldevtools.com/qpl.html
//

#ifdef USE_MMAP

#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include "hash.h"
#include "CleanupManager.h"
#include "DatabaseConfig.h"
#include "LockFD.h"
#include "WordData.h"
#include "FrequencyDBImpl_hash.h"

const char *FrequencyDBImpl_hash::SEARCH_SUFFIX("hash");
static const string TEMP_SUFFIX("rehash");
static const string RENAME_SUFFIX("bak");

enum {
  INDEX_OFFSET = 5,
  FILE_TYPE_INDEX = 0,
  COUNT_INDEX = 1,
  DEBUG_HASH = 0,
  HASH32_FILE_KEY = 0x0bcc0001,
};

HashDataFile::ulong_t hash_string(const string &str)
{
  return jenkins_hash((ub1*)str.c_str(), (ub4)str.length(), 0);
}

FrequencyDBImpl *FrequencyDBImpl_hash::factory(const DatabaseConfig *config)
{
  return new FrequencyDBImpl_hash(config->targetSizeMB());
}

FrequencyDBImpl_hash::FrequencyDBImpl_hash(int _size)
  : m_cursor(0),
    m_dataFile(INDEX_OFFSET, _size * 1024 * 1024)
{
#ifdef USE_HASH_AUTO_CLEAN
  Ptr<CleanupManager> cleaner(new CleanupManager);
  cleaner->addLimit(2, 14);
  cleaner->addLimit(100, 30);
  cleaner->addLimit(10000, 180);
  cleaner->addLimit(100000, 360);
  m_dataFile.setAutoClean(cleaner.release());
#endif
}

FrequencyDBImpl_hash::~FrequencyDBImpl_hash()
{
  close();
}

bool FrequencyDBImpl_hash::open(const string &arg_filename,
                                bool read_only,
                                int create_mode)
{
  close();

  if (is_debug) {
    cerr << "OPEN DATABASE " << arg_filename << endl;
  }

  File db_file(arg_filename);
  db_file.setSuffix(SEARCH_SUFFIX);

  if (!m_dataFile.open(db_file.getPath(), read_only, create_mode)) {
    return false;
  }

  if (m_dataFile.isNewFile()) {
    initializeHeaderRecords();
  } else if (!validateHeaderRecords()) {
    m_dataFile.close();
    cerr << "ERROR: Unexpected header record" << endl;
    return false;
  }

  if (is_debug) {
    cerr << "DATABASE OPENED " << db_file.getPath() << endl;
  }

  return true;
}

void FrequencyDBImpl_hash::initializeHeaderRecords()
{
  HashDataFile::ulong_t key = HASH32_FILE_KEY;
  WordData word_data;
  word_data.reset(HASH32_FILE_KEY, 0, 0);
  m_dataFile.writeRecord(FILE_TYPE_INDEX, key, word_data);
}

bool FrequencyDBImpl_hash::validateHeaderRecords()
{
  HashDataFile::ulong_t key = 0;
  WordData word_data;
  m_dataFile.readRecord(FILE_TYPE_INDEX, key, word_data);
  return key == HASH32_FILE_KEY;
}

void FrequencyDBImpl_hash::close()
{
  m_cursor = 0;
  m_dataFile.close();
}

void FrequencyDBImpl_hash::flush()
{
}

void FrequencyDBImpl_hash::writeWord(const string &word,
                                     const WordData &counts)
{
  assert(m_dataFile.isOpen());

  if (word == FrequencyDB::COUNT_WORD) {
    m_dataFile.writeRecord(COUNT_INDEX, 0, counts);
  } else {
    HashDataFile::ulong_t key = computeKeyForWord(word);
    if (!m_dataFile.write(key, counts)) {
      throw runtime_error(string("no room in hash file for term ") + word);
    }
  }

  if (is_debug) {
    cerr << "wrote word " << word
         << " good " << counts.goodCount()
         << " spam " << counts.spamCount()
         << " total " << counts.totalCount()
         << endl;
  }
}

bool FrequencyDBImpl_hash::readWord(const string &word,
                                    WordData &counts)
{
  assert(m_dataFile.isOpen());

  bool answer = false;
  if (word == FrequencyDB::COUNT_WORD) {
    HashDataFile::ulong_t ignored;
    m_dataFile.readRecord(COUNT_INDEX, ignored, counts);
    answer = true;
  } else {
    HashDataFile::ulong_t key = computeKeyForWord(word);
    answer = m_dataFile.read(key, counts);
  }

  if (is_debug) {
    cerr << "read word " << word
         << " found? " << (answer ? "yes" : "no")
         << " good " << counts.goodCount()
         << " spam " << counts.spamCount()
         << " total " << counts.totalCount()
         << endl;
  }

  return answer;
}

string FrequencyDBImpl_hash::getWordForIndex(int index,
                                             HashDataFile::ulong_t key)
{
  if (index == COUNT_INDEX) {
    return FrequencyDB::COUNT_WORD;
  } else {
    char buffer[128];
    sprintf(buffer, "I0x%08x", key);
    return buffer;
  }
}

HashDataFile::ulong_t FrequencyDBImpl_hash::computeKeyForWord(const string &word)
{
  HashDataFile::ulong_t key = 0;
  if (word == FrequencyDB::COUNT_WORD) {
    // key not used for count
  } else if (starts_with(word, "I0x")) {
    sscanf(word.c_str() + 3, "%x", &key);
  } else {
    key = hash_string(word);
  }

  if (DEBUG_HASH && is_debug) {
    cerr << " KEY 0x" << hex << key
         << " WORD " << word
         << endl;
  }

  return key;
}

bool FrequencyDBImpl_hash::firstWord(string &word,
                                     WordData &counts)
{
  assert(m_dataFile.isOpen());

  HashDataFile::ulong_t key = 0;
  word = FrequencyDB::COUNT_WORD;
  m_dataFile.readRecord(COUNT_INDEX, key, counts);
  m_cursor = INDEX_OFFSET;
  return true;
}

bool FrequencyDBImpl_hash::nextWord(string &word,
                                    WordData &counts)
{
  assert(m_dataFile.isOpen());

  HashDataFile::ulong_t key = 0;
  for (; m_cursor < m_dataFile.indexLimit(); ++m_cursor) {
    m_dataFile.readRecord(m_cursor, key, counts);
    if (counts.totalCount() > 0) {
      word = getWordForIndex(m_cursor, key);
      ++m_cursor;
      return true;
    }
  }

  return false;
}

string FrequencyDBImpl_hash::getDatabaseType() const
{
  return "Hashed-array";
}

void FrequencyDBImpl_hash::sweepOutOldTerms(const CleanupManager &cleanman)
{
  assert(m_dataFile.isOpen());
  assert(!m_dataFile.isReadOnly());

  File live_file(m_dataFile.filename());

  File rehash_file(live_file);
  rehash_file.setSuffix(TEMP_SUFFIX);

  File temp_file(live_file);
  temp_file.setSuffix(RENAME_SUFFIX);

  copyToNewDataFile(cleanman, rehash_file.getPath());
  swapDataFilesAndReopen(live_file.getPath(), rehash_file.getPath(), temp_file.getPath());

  assert(m_dataFile.isOpen());
}

void FrequencyDBImpl_hash::copyToNewDataFile(const CleanupManager &cleanman,
                                             const string &rehash_filename)
{
  assert(m_dataFile.isOpen());

  File rehash_file(rehash_filename);
  if (rehash_file.isFile()) {
    if (is_debug) {
      cerr << "sweepOutJunk: removing existing rehash file " << rehash_filename << endl;
    }
    rehash_file.remove();
  }

  HashDataFile temp_file(INDEX_OFFSET, m_dataFile.size());
  temp_file.open(rehash_filename, false, m_dataFile.createMode());

  m_dataFile.copyHeadersToFile(temp_file);

  HashDataFile::ulong_t key;
  WordData counts;

  // now copy all non-empty keys with acceptable junk_count and max_age
  for (HashDataFile::ulong_t i = INDEX_OFFSET; i < m_dataFile.indexLimit(); ++i) {
    m_dataFile.readRecord(i, key, counts);
    if (key != 0) {
      if (counts.totalCount() == 0 || cleanman.shouldDelete(counts)) {
        if (is_debug) {
          cerr << "sweepOutJunk: removing term " << getWordForIndex(i, key)
               << " with total count " << counts.totalCount()
               << " and age " << counts.age()
               << endl;
        }
      } else {
        temp_file.write(key, counts);
      }
    }
  }

  temp_file.close();
}

void FrequencyDBImpl_hash::swapDataFilesAndReopen(const string &live_filename,
                                                  const string &rehash_filename,
                                                  const string &temp_filename)
{
  assert(m_dataFile.isOpen());

  bool read_only = m_dataFile.isReadOnly();
  int create_mode = m_dataFile.createMode();

  m_dataFile.close();

  if (is_debug) {
    cerr << "renaming " << live_filename << " to " << temp_filename << endl;
  }

  File renamer;
  renamer.setPath(live_filename);
  renamer.rename(temp_filename);

  try {
    if (is_debug) {
      cerr << "renaming " << rehash_filename << " to " << live_filename << endl;
    }
    renamer.setPath(rehash_filename);
    renamer.rename(live_filename);
  } catch (...) {
    if (is_debug) {
      cerr << "recovery: renaming " << temp_filename << " to " << live_filename << endl;
    }
    renamer.setPath(temp_filename);
    renamer.rename(live_filename);
    throw;
  }

  if (is_debug) {
    cerr << "opening new hash file" << endl;
  }

  if (!m_dataFile.open(live_filename, read_only, create_mode)) {
    throw runtime_error("unable to open new file");
  }

  if (is_debug) {
    cerr << "deleting original hash file " << temp_filename << endl;
  }

  renamer.setPath(temp_filename);
  renamer.remove();

  assert(m_dataFile.isOpen());
}

#endif // USE_MMAP
