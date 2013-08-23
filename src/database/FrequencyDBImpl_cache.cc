///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_cache.cc 272 2007-01-06 19:37:27Z brian $
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

#include <unistd.h>
#include <fcntl.h>
#include "WordData.h"
#include "FrequencyDBImpl_cache.h"

static const string DATABASE_TYPE("-cached");

FrequencyDBImpl_cache::FrequencyDBImpl_cache(FrequencyDBImpl *db,
                                             int max_size)
  : m_db(db), m_cache(max_size)
{
  assert(db);
}

FrequencyDBImpl_cache::~FrequencyDBImpl_cache()
{
  close();
}

bool FrequencyDBImpl_cache::open(const string &filename,
                                 bool read_only,
                                 int create_mode)
{
  close();
  return m_db->open(filename, read_only, create_mode);
}

void FrequencyDBImpl_cache::close()
{
  m_db->close();
  clear();
}

void FrequencyDBImpl_cache::flush()
{
  // PBL performance poor for large transactions on large databases
  // 100 seems to be a good limit for records per transaction
  const int MAX_RECORDS_PER_TRANSACTION = 100;
  int num_writes = 0;
  m_db->beginTransaction();
  for (IteratorType i = m_cache.begin(), limit = m_cache.end(); i != limit; ++i) {
    if (i.isLocked()) {
      m_db->writeWord(i.key(), i.value()->counts);
      m_cache.unlock(i.key());
      if (++num_writes >= MAX_RECORDS_PER_TRANSACTION) {
        m_db->endTransaction(true);
        m_db->beginTransaction();
        num_writes = 0;
      }
    }
  }
  m_db->endTransaction(true);
  m_db->flush();
}

void FrequencyDBImpl_cache::addWordData(const string &word,
                                        bool is_dirty,
                                        bool is_shared,
                                        const WordData &counts)
{
  m_cache.put(word, make_ref(new CacheEntry(is_shared, counts)), is_dirty);
}

void FrequencyDBImpl_cache::clear()
{
  m_cache.clear();
}

void FrequencyDBImpl_cache::writeWord(const string &word,
                                      const WordData &counts)
{
  if (m_cache.lockedCount() >= m_cache.maxSize()) {
    if (is_debug) {
      cerr << "MAX CACHE TERMS EXCEEDED - FLUSHING CACHE" << endl;
    }
    flush();
  }

  if (!m_db->canCacheTerm(word)) {
    if (is_debug) {
      cerr << "UNCACHED TERM " << word << endl;
    }
    m_db->writeWord(word, counts);
  } else {
    Ref<CacheEntry> entry;
    if (!m_cache.get(word, entry)) {
      addWordData(word, true, false, counts);
      if (is_debug) {
        cerr << "CACHED TERM INSERTED " << word << endl;
      }
    } else if ((entry->counts).equals(counts)) {
      if (is_debug) {
        cerr << "CACHED TERM UNCHANGED " << word << endl;
      }
    } else if (entry->is_shared && entry->counts.hasSameCounts(counts)) {
      // Do nothing because we don't want terms migrating from shared to 
      // private database if only the timestamp changed.
      if (is_debug) {
        cerr << "CACHED TERM FROM SHARED " << word << endl;
      }
    } else {
      entry->is_shared = false;
      entry->counts = counts;
      m_cache.lock(word);
      if (is_debug) {
        cerr << "CACHED TERM UPDATED " << word << endl;
      }
    }
  }
  if (is_debug) {
    cerr << "CACHE SIZE LOCKED: " << m_cache.lockedCount() << " TOTAL " << m_cache.size() << " MAX " << m_cache.maxSize() << endl;
  }
}

bool FrequencyDBImpl_cache::readWord(const string &word,
                                     WordData &counts)
{
  Ref<CacheEntry> entry;
  if (m_cache.get(word, entry)) {
    if (is_debug) {
      cerr << "READ CACHED TERM " << word << endl;
    }
    counts = entry->counts;
    return true;
  }

  bool is_shared = false;
  if (m_db->readWord(word, counts, is_shared)) {
    if (m_db->canCacheTerm(word)) {
      addWordData(word, false, is_shared, counts);
      if (is_debug) {
        cerr << "CACHED UNMODIFIED TERM " << word << endl;
      }
    }
    return true;
  }

  return false;
}

bool FrequencyDBImpl_cache::firstWord(string &word,
                                      WordData &counts)
{
  flush();
  clear();
  return m_db->firstWord(word, counts);
}

bool FrequencyDBImpl_cache::nextWord(string &word,
                                     WordData &counts)
{
  return m_db->nextWord(word, counts);
}

string FrequencyDBImpl_cache::getDatabaseType() const
{
  return m_db->getDatabaseType() + DATABASE_TYPE;
}

void FrequencyDBImpl_cache::sweepOutOldTerms(const CleanupManager &cleanman)
{
  flush();
  clear();
  m_db->sweepOutOldTerms(cleanman);
}
