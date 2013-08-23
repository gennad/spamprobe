///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_bdb.cc 272 2007-01-06 19:37:27Z brian $
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

#ifdef USE_DB

#include <unistd.h>
#include <stdexcept>
#include <strstream>
#include "CleanupManager.h"
#include "LockFile.h"
#include "WordData.h"
#include "FrequencyDBImpl_bdb.h"

struct MyDBT : public DBT
{
  MyDBT()
  {
    memset(this, 0, sizeof(*this));
  }

  MyDBT(const string &word)
  {
    memset(this, 0, sizeof(*this));
    size = word.length() + 1;
    data = (char *)word.c_str();
  }

  MyDBT(const WordData &counts)
  {
    memset(this, 0, sizeof(*this));
    size = sizeof(WordData);
    data = (char *)&counts;
  }
};

inline int throw_on_error(const char *function_name,
                          int rc)
{
  if (rc == DB_NOTFOUND) {
    return rc;
  }
  if (rc != 0) {
    static char buffer[4096];
    ostrstream msg(buffer, sizeof(buffer));
    msg << function_name << ": " << db_strerror(rc) << " (" << rc << ")" << ends;
    throw runtime_error(buffer);
  }
  return rc;
}

inline int warn_on_error(const char *function_name,
                         int rc)
{
  if (rc == DB_NOTFOUND) {
    return rc;
  }
  if (rc != 0) {
    cerr << "warning: berkeley db reported error: "
         << function_name
         << ": "
         << db_strerror(rc)
         << " (" << rc << ")"
         << endl;
  }
  return rc;
}

FrequencyDBImpl *FrequencyDBImpl_bdb::factory(const DatabaseConfig *config)
{
  return new FrequencyDBImpl_bdb();
}

FrequencyDBImpl_bdb::FrequencyDBImpl_bdb()
  : m_env(0),
    m_file(0),
    m_cursor(0)
{
}

FrequencyDBImpl_bdb::~FrequencyDBImpl_bdb()
{
  close();
}

bool FrequencyDBImpl_bdb::open(const string &arg_filename,
                               bool read_only,
                               int create_mode)
{
  close();

  m_isReadOnly = read_only;

  File db_file(arg_filename);
  if (!openEnvironment(db_file, read_only, create_mode)) {
    return false;
  }

  if (!openDatabase(db_file, read_only, create_mode)) {
    closeEnvironment();
    return false;
  }

  if (is_debug) {
    cerr << "DATABASE OPENED " << db_file.getPath() << endl;
  }

  return true;
}

bool FrequencyDBImpl_bdb::openDatabase(const File &db_file,
                                       bool read_only,
                                       int create_mode)
{
  if (is_debug) {
    cerr << "OPENING DATABASE " << db_file.getPath() << endl;
  }

  int ret = db_create(&m_file, m_env, 0);
  if (ret != 0) {
    cerr << "error: unable to create database " << db_file.getPath() << ": " << db_strerror(ret) << endl;
    return false;
  }

  // SleepyCat in their infinite wisdom decided to change the open
  // function's signature in the 4.1 release.  Gee thanks for breaking
  // my code guys. That was real smart and so much better than
  // introducing a second open function rather than force me to embed
  // hideous ifdefs into my code.
  string filename(m_env ? db_file.getName() : db_file.getPath());
  int flags = read_only ? DB_RDONLY : DB_CREATE;
#if DB_VERSION_MAJOR >= 4 && DB_VERSION_MINOR >= 1
  ret = m_file->open(m_file, NULL, filename.c_str(), NULL, DB_BTREE, flags, create_mode);
#else
  ret = m_file->open(m_file, filename.c_str(), NULL, DB_BTREE, flags, create_mode);
#endif
  if (ret != 0) {
    cerr << "error: unable to open database " << db_file.getPath() << ": " << db_strerror(ret) << endl;
    m_file = 0;
    return false;
  }

  if (is_debug) {
    cerr << "OPENED DATABASE " << db_file.getPath() << endl;
  }

  return true;
}

bool FrequencyDBImpl_bdb::openEnvironment(const File &db_file,
                                          bool read_only,
                                          int create_mode)
{
#if USE_CDB
  File env_dir(db_file.parent());

  if (is_debug) {
    cerr << "OPENING ENVIRONMENT " << env_dir.getPath() << endl;
  }

  int ret = db_env_create(&m_env, 0);
  if (ret != 0) {
    cerr << "error: unable to create environment " << db_file.getPath() << ": " << db_strerror(ret) << endl;
    m_env = 0;
    return false;
  }

  int env_flags = DB_INIT_CDB | DB_INIT_MPOOL | DB_CREATE;
  ret = m_env->open(m_env, db_file.parent().getPath().c_str(), env_flags, create_mode);
  if (ret != 0) {
    if (read_only) {
      // we can still operate without the environment if we're in read-only mode
      m_env = 0;
    } else {
      cerr << "error: unable to open environment " << env_dir.getPath() << ": " << db_strerror(ret) << endl;
      m_env = 0;
      return false;
    }
  }
#else
  m_env = 0;
#endif

  return true;
}

void FrequencyDBImpl_bdb::closeCursor()
{
  if (m_cursor) {
    warn_on_error("c_close", m_cursor->c_close(m_cursor));
    m_cursor = 0;
  }
}

void FrequencyDBImpl_bdb::closeDatabase()
{
  if (m_file) {
    warn_on_error("db sync", m_file->sync(m_file, 0));
    warn_on_error("db close", m_file->close(m_file, 0));
    m_file = 0;
  }
}

void FrequencyDBImpl_bdb::closeEnvironment()
{
#ifdef USE_CDB
  if (m_env) {
    warn_on_error("env close", m_env->close(m_env, 0));
    m_env = 0;
  }
#endif
}

void FrequencyDBImpl_bdb::close()
{
  closeCursor();
  closeDatabase();
  closeEnvironment();
}

void FrequencyDBImpl_bdb::flush()
{
  if (is_debug) {
    cerr << "flushing database..." << endl;
  }
  throw_on_error("sync", m_file->sync(m_file, 0));
}

void FrequencyDBImpl_bdb::writeWord(const string &word,
                                    const WordData &counts)
{
  assert(m_file);

  MyDBT key(word);
  bool delete_word = counts.totalCount() <= 0;

  if (is_debug) {
    WordData old_counts;
    bool exists = loadKey(key, old_counts);
    if (delete_word) {
      cerr << "writeWord: deleting '" << word << "'"
           << endl;
    } else if (exists) {
      cerr << "writeWord: updating '" << word << "'"
           << " old (" << old_counts.goodCount() << "," << old_counts.spamCount() << ")"
           << " new (" << counts.goodCount() << "," << counts.spamCount() << ")"
           << endl;
    } else {
      cerr << "writeWord: inserting '" << word << "'"
           << " new (" << counts.goodCount() << "," << counts.spamCount() << ")"
           << endl;
    }
  }

  if (delete_word) {
    throw_on_error("del", m_file->del(m_file, NULL, &key, 0));
  } else {
    MyDBT value(counts);
    throw_on_error("put", m_file->put(m_file, NULL, &key, &value, 0));
  }
}

bool FrequencyDBImpl_bdb::readWord(const string &word,
                                   WordData &counts)
{
  assert(m_file);

  MyDBT key(word);
  return loadKey(key, counts);
}

bool FrequencyDBImpl_bdb::firstWord(string &word,
                                    WordData &counts)
{
  return firstWord(word, counts, true);
}

bool FrequencyDBImpl_bdb::firstWord(string &word,
                                    WordData &counts,
                                    bool read_only)
{
  closeCursor();

  assert(read_only || !m_isReadOnly);

  int cursor_flags = 0;
#ifdef USE_CDB
  if (!read_only) {
    cursor_flags |= DB_WRITECURSOR;
  }
#endif

  int ret = warn_on_error("cursor", m_file->cursor(m_file, NULL, &m_cursor, cursor_flags));
  if (ret != 0) {
    return false;
  }
  assert(m_cursor);

  return nextWord(word, counts);
}

bool FrequencyDBImpl_bdb::nextWord(string &word,
                                   WordData &counts)
{
  if (!m_cursor) {
    return false;
  }

  MyDBT key;
  MyDBT value;
  int ret = warn_on_error("c_get", m_cursor->c_get(m_cursor, &key, &value, DB_NEXT));
  if (ret != 0) {
    word.erase();
    counts.clear();
    closeCursor();
    return false;
  }

  if (!value.data || value.size != sizeof(WordData)) {
    word.erase();
    counts.clear();
    return false;
  }

  word.assign((const char *)key.data, max((u_int32_t)0, key.size - 1));
  counts = *((WordData *)value.data);
  return true;
}

bool FrequencyDBImpl_bdb::loadKey(DBT &key,
                                  string &word,
                                  WordData &counts) const
{
  if (key.data == NULL) {
    word.erase();
    counts.clear();
    return false;
  }

  if (key.size == 0) {
    word.erase();
  } else {
    word.assign((const char *)key.data, key.size - 1);
  }
  return loadKey(key, counts);
}

bool FrequencyDBImpl_bdb::loadKey(DBT &key,
                                  WordData &counts) const
{
  if (key.data == NULL) {
    counts.clear();
    return false;
  }

  MyDBT value;
  int ret = throw_on_error("get", m_file->get(m_file, NULL, &key, &value, 0));
  if (ret != 0) {
    counts.clear();
    return false;
  }

  if (!value.data || value.size != sizeof(WordData)) {
    counts.clear();
    return false;
  }

  counts = *((WordData *)value.data);
  return true;
}

string FrequencyDBImpl_bdb::getDatabaseType() const
{
  return "BerkeleyDB-btree";
}

void FrequencyDBImpl_bdb::sweepOutOldTerms(const CleanupManager &cleanman)
{
  string word;
  WordData counts;

  assert(!m_isReadOnly);

  bool again = firstWord(word, counts, false);
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
      warn_on_error("c_del", m_cursor->c_del(m_cursor, 0));
    }
    again = nextWord(word, counts);
  }

  flush();
}

#endif // USE_DBM
