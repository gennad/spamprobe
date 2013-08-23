///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_dbm.cc 272 2007-01-06 19:37:27Z brian $
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

#ifdef USE_DBM

#include <unistd.h>
#include <fcntl.h>
#include "LockFile.h"
#include "WordData.h"
#include "FrequencyDBImpl_dbm.h"

static const string LOCK_FILENAME("lock");

FrequencyDBImpl *FrequencyDBImpl_dbm::factory()
{
  return new FrequencyDBImpl_dbm();
}

FrequencyDBImpl_dbm::FrequencyDBImpl_dbm()
  : m_file(0)
{
}

FrequencyDBImpl_dbm::~FrequencyDBImpl_dbm()
{
  close();
}

bool FrequencyDBImpl_dbm::open(const string &filename,
                               bool read_only)
{
  close();

  int flags;
  if (read_only) {
    flags = O_RDONLY;
  } else {
    flags = O_RDWR | O_CREAT;
  }
  m_file = dbm_open(filename.c_str(), flags, 0600);
  return m_file != 0;
}

void FrequencyDBImpl_dbm::close()
{
  if (m_file) {
    dbm_close(m_file);
    m_file = 0;
  }
  m_lock.clear();
}

void FrequencyDBImpl_dbm::flush()
{
}

void FrequencyDBImpl_dbm::writeWord(const string &word,
                                    const WordData &counts)
{
  assert(m_file);

  datum key;
  key.dptr = (char *)word.c_str();
  key.dsize = word.length() + 1;

  datum value;
  value.dptr = (char *)&counts;
  value.dsize = sizeof(counts);
  dbm_store(m_file, key, value, DBM_REPLACE);
}

bool FrequencyDBImpl_dbm::readWord(const string &word,
                                   WordData &counts)
{
  assert(m_file);

  datum key;
  key.dptr = (char *)word.c_str();
  key.dsize = word.length() + 1;
  return loadKey(key, counts);
}

bool FrequencyDBImpl_dbm::firstWord(string &word,
                                    WordData &counts)
{
  datum key = dbm_firstkey(m_file);
  return loadKey(key, word, counts);
}

bool FrequencyDBImpl_dbm::nextWord(string &word,
                                   WordData &counts)
{
  datum key = dbm_nextkey(m_file);
  return loadKey(key, word, counts);
}

bool FrequencyDBImpl_dbm::loadKey(const datum &key,
                                  string &word,
                                  WordData &counts) const
{
  if (key.dptr == NULL) {
    word.erase();
    counts.clear();
    return false;
  }

  word.assign(key.dptr, max(0, key.dsize - 1));
  return loadKey(key, counts);
}

bool FrequencyDBImpl_dbm::loadKey(const datum &key,
                                  WordData &counts) const
{
  if (key.dptr == NULL) {
    counts.clear();
    return false;
  }

  datum value = dbm_fetch(m_file, key);
  if (!value.dptr || value.dsize != sizeof(WordData)) {
    counts.clear();
    return false;
  }

  counts = *((WordData *)value.dptr);
  return true;
}

string FrequencyDBImpl_dbm::getDatabaseType() const
{
  return "DBM-hash";
}

void FrequencyDBImpl_dbm::sweepOutJunk(const CleanupManager &cleanman)
{
}

#endif // USE_DBM
