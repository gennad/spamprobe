///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_split.cc 272 2007-01-06 19:37:27Z brian $
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

#include <unistd.h>
#include "LockFD.h"
#include "WordData.h"
#include "DatabaseConfig.h"
#include "FrequencyDBImpl_hash.h"
#include "FrequencyDBImpl_split.h"

FrequencyDBImpl *FrequencyDBImpl_split::factory(const DatabaseConfig *config)
{
  return new FrequencyDBImpl_split(config);
}

FrequencyDBImpl_split::FrequencyDBImpl_split(const DatabaseConfig *config)
  : m_config(config),
    m_cursor(0)
{
}

FrequencyDBImpl_split::~FrequencyDBImpl_split()
{
  close();
}

bool FrequencyDBImpl_split::open(const string &filename,
                                 bool read_only,
                                 int create_mode)
{
  close();

  string db_filename(filename);
  m_db.set(m_config->createDatabaseImplUsingDefaultType(db_filename));
  if (!m_db->open(db_filename, read_only, create_mode)) {
    m_db.clear();
    return false;
  }

  db_filename = filename;
  m_hash.set(new FrequencyDBImpl_hash(m_config->targetSizeMB()));
  if (!m_hash->open(db_filename, read_only, create_mode)) {
    m_db.clear();
    m_hash.clear();
    return false;
  }

  return true;
}

void FrequencyDBImpl_split::close()
{
  m_db.clear();
  m_hash.clear();
}

void FrequencyDBImpl_split::flush()
{
  if (m_db.get()) {
    m_db->flush();
  }
  if (m_hash.get()) {
    m_hash->flush();
  }
}

void FrequencyDBImpl_split::beginTransaction()
{
  if (m_db.get()) {
    m_db->beginTransaction();
  }
  if (m_hash.get()) {
    m_hash->beginTransaction();
  }
}

void FrequencyDBImpl_split::endTransaction(bool commit)
{
  if (m_db.get()) {
    m_db->endTransaction(commit);
  }
  if (m_hash.get()) {
    m_hash->endTransaction(commit);
  }
}

void FrequencyDBImpl_split::writeWord(const string &word,
                                      const WordData &counts)
{
  assert(m_db.get());
  assert(m_hash.get());

  if (word.length() > 0 && word[0] == '_' && word != FrequencyDB::COUNT_WORD) {
    m_db->writeWord(word, counts);
  } else {
    m_hash->writeWord(word, counts);
  }
}

bool FrequencyDBImpl_split::readWord(const string &word,
                                     WordData &counts)
{
  assert(m_db.get());
  assert(m_hash.get());

  bool ret = false;
  if (word.length() > 0 && word[0] == '_' && word != FrequencyDB::COUNT_WORD) {
    ret = m_db->readWord(word, counts);
  } else {
    ret = m_hash->readWord(word, counts);
  }

  return ret;
}

bool FrequencyDBImpl_split::firstWord(string &word,
                                      WordData &counts)
{
  assert(m_db.get());
  assert(m_hash.get());

  if (m_db->firstWord(word, counts)) {
    m_cursor = 0;
    return true;
  }

  if (m_hash->firstWord(word, counts)) {
    m_cursor = 1;
    return true;
  }

  m_cursor = 2;
  return false;
}

bool FrequencyDBImpl_split::nextWord(string &word,
                                     WordData &counts)
{
  assert(m_db.get());
  assert(m_hash.get());

  if (m_cursor == 0) {
    if (m_db->nextWord(word, counts)) {
      return true;
    }
    m_cursor = 1;
  }

  if (m_cursor == 1) {
    if (m_hash->nextWord(word, counts)) {
      return true;
    }
    m_cursor = 2;
  }

  return false;
}

string FrequencyDBImpl_split::getDatabaseType() const
{
  return "BerkeleyDB-Hashed";
}

void FrequencyDBImpl_split::sweepOutOldTerms(const CleanupManager &cleanman)
{
  assert(m_db.get());
  assert(m_hash.get());

  m_db->sweepOutOldTerms(cleanman);
  m_hash->sweepOutOldTerms(cleanman);
}

bool FrequencyDBImpl_split::canCacheTerm(const string &word)
{
  return (word.length() == 0) || (word[0] == '_' && word != FrequencyDB::COUNT_WORD);
}

#endif // USE_DBM
