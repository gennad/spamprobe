///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_pbl.cc 272 2007-01-06 19:37:27Z brian $
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

#ifdef USE_PBL

#include <unistd.h>
#include <fcntl.h>
#include <strstream>
#include <stdexcept>
#include "CleanupManager.h"
#include "LockFile.h"
#include "WordData.h"
#include "FrequencyDBImpl_pbl.h"

static const int BUFFER_SIZE = 1024;
static const int CLEANUP_RECORDS_PER_TRANSACTION = 10;

const char *FrequencyDBImpl_pbl::SEARCH_SUFFIX("pkey");

inline int throw_on_error(const char *function_name,
                          int rc)
{
  if (rc >= 0) {
    return rc;
  }

  if (pbl_errno == PBL_ERROR_NOT_FOUND) {
    return PBL_ERROR_NOT_FOUND;
  }

  static char buffer[4096];
  ostrstream msg(buffer, sizeof(buffer));
  msg << function_name << ": " << pbl_errstr << " (" << pbl_errno << ")" << ends;
  throw runtime_error(buffer);
}

inline int warn_on_error(const char *function_name,
                         int rc)
{
  if (rc >= 0) {
    return rc;
  }

  if (pbl_errno == PBL_ERROR_NOT_FOUND) {
    return PBL_ERROR_NOT_FOUND;
  }

  cerr << "warning: pbl reported error: "
       << function_name
       << ": "
       << pbl_errstr
       << " (" << pbl_errno << ")"
       << endl;
  return rc;
}

FrequencyDBImpl *FrequencyDBImpl_pbl::factory(const DatabaseConfig *config)
{
  return new FrequencyDBImpl_pbl();
}

FrequencyDBImpl_pbl::FrequencyDBImpl_pbl()
  : m_kf(0),
    m_isReadOnly(false),
    m_inTransaction(false)
{
}

FrequencyDBImpl_pbl::~FrequencyDBImpl_pbl()
{
  close();
}

inline int copy_key(const string &str,
                    unsigned char *ptr)
{
  int len = str.size();
  if (len >= 254) {
    len = 254;
  }
  memcpy(ptr, str.c_str(), len);
  ptr[len] = (unsigned char)0;
  return len + 1;
}

bool FrequencyDBImpl_pbl::open(const string &arg_filename,
                               bool read_only,
                               int open_mode)
{
  close();
  assert(m_kf == 0);

  File db_file(arg_filename);
  db_file.setSuffix(SEARCH_SUFFIX);
  string filename(db_file.getPath());
  char *filename_ptr = const_cast<char *>(filename.c_str());

  if (!read_only) {
    m_kf = pblKfCreate(filename_ptr, 0);
  }
  if (!m_kf) {
    m_kf = pblKfOpen(filename_ptr, read_only ? 0 : 1, 0);
  }

  m_isReadOnly = read_only;
  m_inTransaction = false;

  if (is_debug) {
    cerr << "DATABASE OPENED " << db_file.getPath() << endl;
  }

  return m_kf != 0;
}

void FrequencyDBImpl_pbl::close()
{
  if (m_kf) {
    quietAbortTransaction();
    warn_on_error("pblKfFlush", pblKfFlush(m_kf));
    warn_on_error("pblKfClose", pblKfClose(m_kf));
    m_kf = 0;
  }
}

void FrequencyDBImpl_pbl::flush()
{
  if (m_kf) {
    throw_on_error("pblKfFlush", pblKfFlush(m_kf));
  }
}

void FrequencyDBImpl_pbl::quietAbortTransaction()
{
  assert(m_kf);

  if (m_inTransaction) {
    warn_on_error("pblKfCommit", pblKfCommit(m_kf, 1));
    m_inTransaction = false;
  }
}

void FrequencyDBImpl_pbl::beginTransaction()
{
  assert(m_kf);
  assert(!m_inTransaction);

  if (!m_inTransaction) {
    if (is_debug) {
      cerr << "starting transaction" << endl;
    }
    throw_on_error("pblKfStartTransaction", pblKfStartTransaction(m_kf));
    m_inTransaction = true;
  }
}

void FrequencyDBImpl_pbl::endTransaction(bool commit)
{
  assert(m_kf);
  assert(m_inTransaction);

  if (m_inTransaction) {
    if (is_debug) {
      cerr << "ending transaction: " << commit << endl;
    }
    throw_on_error("pblKfCommit", pblKfCommit(m_kf, commit ? 0 : 1));
    m_inTransaction = false;
  }
}

void FrequencyDBImpl_pbl::writeWord(const string &word,
                                    const WordData &counts)
{
  assert(m_kf);
  assert(!m_isReadOnly);

  bool delete_word = counts.totalCount() <= 0;
  unsigned char key[BUFFER_SIZE];
  int keylen = copy_key(word, key);

  WordData db_counts(counts);
  db_counts.toDatabaseOrder();

  int rc = throw_on_error("pblKfFind", pblKfFind(m_kf, PBLEQ, key, keylen, 0, 0));
  if (rc == PBL_ERROR_NOT_FOUND) {
    if (!delete_word) {
      if (is_debug) {
        cerr << "inserting word " << word << ": keylen " << keylen << endl;
      }
      throw_on_error("pblKfInsert", pblKfInsert(m_kf, key, keylen, (unsigned char *)&db_counts, sizeof(db_counts)));
    } else {
      if (is_debug) {
        cerr << "ignoring zero count word " << word << ": keylen " << keylen << endl;
      }
    }
  } else {
    assert(rc >= 0);
    if (delete_word) {
      if (is_debug) {
        cerr << "deleting word " << word << ": keylen " << keylen << endl;
      }
      throw_on_error("pblKfDelete", pblKfDelete(m_kf));
    } else {
      if (is_debug) {
        cerr << "updating word " << word << ": keylen " << keylen << endl;
      }
      throw_on_error("pblKfUpdateData", pblKfUpdate(m_kf, (unsigned char *)&db_counts, sizeof(db_counts)));
    }
  }
}

bool FrequencyDBImpl_pbl::readWord(const string &word,
                                   WordData &counts)
{
  assert(m_kf);

  unsigned char key[BUFFER_SIZE];
  int keylen = copy_key(word, key);

  int rc = throw_on_error("pblKfFind", pblKfFind(m_kf, PBLEQ, key, keylen, 0, 0));
  if (rc == PBL_ERROR_NOT_FOUND) {
    return false;
  }

  assert(rc == sizeof(counts));
  throw_on_error("pblKfRead", pblKfRead(m_kf, (unsigned char *)&counts, sizeof(counts)));

  counts.toHostOrder();
  return true;
}

bool FrequencyDBImpl_pbl::getWord(int pbl_code,
                                  string &word,
                                  WordData &counts)
{
  char key[BUFFER_SIZE];
  int keylen = 0;
  int rc = 0;

  switch (pbl_code) {
  case PBLFIRST:
    rc = throw_on_error("pblKfGetAbs", pblKfGetAbs(m_kf, 0, key, &keylen));
    break;

  case PBLTHIS:
    rc = throw_on_error("pblKfThis", pblKfThis(m_kf, key, &keylen));
    break;

  default:
    assert(pbl_code == PBLNEXT);
    rc = throw_on_error("pblKfNext", pblKfNext(m_kf, key, &keylen));
    break;
  }

  if (rc == PBL_ERROR_NOT_FOUND) {
    return false;
  }

  word.assign(key);
  throw_on_error("pblKfRead", pblKfRead(m_kf, (unsigned char *)&counts, sizeof(counts)));

  counts.toHostOrder();
  return true;
}

bool FrequencyDBImpl_pbl::firstWord(string &word,
                                    WordData &counts)
{
  return getWord(PBLFIRST, word, counts);
}

bool FrequencyDBImpl_pbl::nextWord(string &word,
                                   WordData &counts)
{
  return getWord(PBLNEXT, word, counts);
}

string FrequencyDBImpl_pbl::getDatabaseType() const
{
  return "DBM-pbl";
}

void FrequencyDBImpl_pbl::sweepOutOldTerms(const CleanupManager &cleanman)
{
  string word;
  WordData counts;

  assert(!m_isReadOnly);
  assert(!m_inTransaction);

  beginTransaction();
  try {
    int records_processed = 0;
    bool again = firstWord(word, counts);
    while (again) {
      bool delete_word = false;
      if (word.length() >= 3 && word[0] == '_' && word[1] == '_') {
        if (starts_with(word, "__MD5") && counts.totalCount() == 0) {
          // go ahead and remove digests that have a count of zero
          delete_word = true;
        } else {
          // ignore special words like __MD5 and __COUNT__
        }
      } else {
        delete_word = cleanman.shouldDelete(counts);
      }
      if (delete_word) {
        if (is_debug) {
          cerr << "sweepOutJunk: removing term " << word
               << " with total count " << counts.totalCount()
               << " and age " << counts.age()
               << endl;
        }
        throw_on_error("pblKfDelete", pblKfDelete(m_kf));
        again = getWord(PBLTHIS, word, counts);
        ++records_processed;
      } else {
        again = nextWord(word, counts);
      }
      if (records_processed >= CLEANUP_RECORDS_PER_TRANSACTION) {
        endTransaction(true);
        beginTransaction();
        records_processed = 0;
      }
    }
  } catch (runtime_error &ex) {
    quietAbortTransaction();
    throw;
  }
  endTransaction(true);

  flush();
}

#endif // USE_PBL
