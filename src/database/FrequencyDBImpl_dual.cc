///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_dual.cc 272 2007-01-06 19:37:27Z brian $
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
#include "FrequencyDBImpl_dual.h"

static const string DATABASE_TYPE("-dual");

FrequencyDBImpl_dual::FrequencyDBImpl_dual(FrequencyDBImpl *shared_db,
                                           FrequencyDBImpl *private_db,
                                           const string &shared_filename)
  : m_sharedDB(shared_db),
    m_privateDB(private_db),
    m_sharedFilename(shared_filename)
{
  assert(shared_db);
  assert(private_db);
}

FrequencyDBImpl_dual::~FrequencyDBImpl_dual()
{
  close();
}

bool FrequencyDBImpl_dual::open(const string &filename,
                                bool read_only,
                                int create_mode)
{
  close();
  return m_sharedDB->open(m_sharedFilename, true, SHARED_DB_MODE)
         && m_privateDB->open(filename, read_only, PRIVATE_DB_MODE);
}

void FrequencyDBImpl_dual::close()
{
  m_privateDB->close();
  m_sharedDB->close();
}

void FrequencyDBImpl_dual::flush()
{
  m_sharedDB->flush();
  m_privateDB->flush();
}

void FrequencyDBImpl_dual::writeWord(const string &word,
                                     const WordData &counts)
{
  m_privateDB->writeWord(word, counts);
}

bool FrequencyDBImpl_dual::readWord(const string &word,
                                    WordData &counts)
{
  return m_privateDB->readWord(word, counts) || m_sharedDB->readWord(word, counts);
}

bool FrequencyDBImpl_dual::readWord(const string &word,
				    WordData &counts,
				    bool &is_shared)
{
  if (m_privateDB->readWord(word, counts)) {
    is_shared = false;
    return true;
  }

  if (m_sharedDB->readWord(word, counts)) {
    is_shared = true;
    return true;
  }

  return false;
}

bool FrequencyDBImpl_dual::firstWord(string &word,
                                     WordData &counts)
{
  return m_privateDB->firstWord(word, counts);
}

bool FrequencyDBImpl_dual::nextWord(string &word,
                                    WordData &counts)
{
  return m_privateDB->nextWord(word, counts);
}

string FrequencyDBImpl_dual::getDatabaseType() const
{
  return m_sharedDB->getDatabaseType() + DATABASE_TYPE;
}

void FrequencyDBImpl_dual::sweepOutOldTerms(const CleanupManager &cleanman)
{
  m_privateDB->sweepOutOldTerms(cleanman);
}

bool FrequencyDBImpl_dual::canCacheTerm(const string &word)
{
  return m_sharedDB->canCacheTerm(word) || m_privateDB->canCacheTerm(word);
}
