///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: DatabaseConfig.cc 272 2007-01-06 19:37:27Z brian $
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

#include <stdexcept>
#include "File.h"
#include "WordData.h"
#include "FrequencyDBImpl.h"
#include "FrequencyDBImpl_split.h"
#include "FrequencyDBImpl_hash.h"
#include "FrequencyDBImpl_cache.h"
#include "FrequencyDBImpl_dual.h"
#include "FrequencyDBImpl_dbm.h"
#include "FrequencyDBImpl_bdb.h"
#include "FrequencyDBImpl_pbl.h"
#include "FrequencyDBImpl_null.h"
#include "DatabaseConfig.h"

struct DBType {
  const string prefix;
  const char *suffix;
  FrequencyDBImplFactory factory;
};

static DBType DBTYPES[] = {
#ifdef USE_MMAP
  { "split", 0, FrequencyDBImpl_split::factory },
  { "hash", FrequencyDBImpl_hash::SEARCH_SUFFIX, FrequencyDBImpl_hash::factory },
#endif
#ifdef USE_PBL
  { "pbl", FrequencyDBImpl_pbl::SEARCH_SUFFIX, FrequencyDBImpl_pbl::factory },
#endif
#ifdef USE_DB
  { "bdb", 0, FrequencyDBImpl_bdb::factory },
#endif
#ifdef USE_DBM
  { "gdbm", 0, FrequencyDBImpl_dbm::factory },
#endif
#ifdef USE_PBL
  { "", FrequencyDBImpl_pbl::SEARCH_SUFFIX, FrequencyDBImpl_pbl::factory },
#endif
#ifdef USE_DB
  { "", 0, FrequencyDBImpl_bdb::factory },
#endif
#ifdef USE_DBM
  { "", 0, FrequencyDBImpl_dbm::factory },
#endif
#ifdef USE_MMAP
  { "", FrequencyDBImpl_hash::SEARCH_SUFFIX, FrequencyDBImpl_hash::factory },
#endif
  { "", 0 }
};

DatabaseConfig::DatabaseConfig()
  : m_targetSizeMB(32),
    m_maxCacheTerms(15000)
{
  File basedir(File::getHomeDir(), ".spamprobe");
  File sp_dir(basedir, "sp_words");
  m_privateFilename = sp_dir.getPath();
}

DatabaseConfig::~DatabaseConfig()
{
}

void DatabaseConfig::parseCommandLineArg(string &arg,
                                         string &database_type,
                                         int &target_size_mb)
{
  database_type = "";
  target_size_mb = m_targetSizeMB;

  string::size_type first_colon = arg.find(":");
  if (first_colon == string::npos) {
    return;
  }

  database_type = arg.substr(0, first_colon);

  string::size_type last_colon = arg.find(":", first_colon + 1);
  if (last_colon == string::npos) {
    arg.erase(0, first_colon + 1);
  } else {
    target_size_mb = atoi(arg.substr(first_colon + 1, last_colon - first_colon - 1).c_str());
    arg.erase(0, last_colon + 1);
  }
}

OWNED FrequencyDBImpl *DatabaseConfig::createDatabaseImpl(bool read_only) const
{
  string private_filename(m_privateFilename);
  Ptr<FrequencyDBImpl> private_db(createDatabaseImplUsingBestFit(m_databaseType, private_filename));

  Ptr<FrequencyDBImpl> db;
  if (m_sharedFilename.length() == 0) {
    db.set(private_db.release());
  } else {
    string shared_filename(m_sharedFilename);
    Ptr<FrequencyDBImpl> shared_db(createDatabaseImplUsingBestFit("", shared_filename));
    db.set(new FrequencyDBImpl_dual(shared_db.release(), private_db.release(), shared_filename));
  }
  if (m_maxCacheTerms > 0) {
    db.set(new FrequencyDBImpl_cache(db.release(), m_maxCacheTerms));
  }
  return db->open(private_filename, read_only, FrequencyDBImpl::PRIVATE_DB_MODE) ? db.release() : 0;
}

OWNED FrequencyDBImpl *DatabaseConfig::createDatabaseImplUsingBestFit(const string &type,
                                                                      string &filename) const
{
  Ptr<FrequencyDBImpl> db;

  if (db.isNull() && type.length() > 0) {
    db.set(createDatabaseImplForType(type, filename));
  }

  if (db.isNull()) {
    db.set(createDatabaseImplUsingExistingFile(filename));
  }

  if (db.isNull()) {
    db.set(createDatabaseImplUsingDefaultType(filename));
  }

  if (db.isNull()) {
    throw runtime_error(string("no database type known for filename: ") + filename);
  }

  return db.release();
}

OWNED FrequencyDBImpl *DatabaseConfig::createDatabaseImplForType(const string &type,
                                                                 string &filename) const
{
  File search_file(filename);

  for (DBType *dbt = DBTYPES; dbt->factory; ++dbt) {
    if (dbt->prefix == type) {
      if (dbt->suffix) {
        search_file.setSuffix(dbt->suffix);
      }
      if (is_debug) {
        cerr << "USING REQUESTED DATABASE TYPE '" << dbt->prefix << "' PATH " << search_file.getPath() << endl;
      }
      filename = search_file.getPath();
      return (dbt->factory)(this);
    }
  }

  return 0;
}

OWNED FrequencyDBImpl *DatabaseConfig::createDatabaseImplUsingDefaultType(string &filename) const
{
  return createDatabaseImplForType("", filename);
}

OWNED FrequencyDBImpl *DatabaseConfig::createDatabaseImplUsingExistingFile(string &filename) const
{
  File search_file(filename);

  for (DBType *dbt = DBTYPES; dbt->factory; ++dbt) {
    if (dbt->suffix) {
      search_file.setSuffix(dbt->suffix);
      if (search_file.isFile()) {
        if (is_debug) {
          cerr << "USING DISCOVERED DATABASE TYPE '" << dbt->prefix << "' PATH " << search_file.getPath() << endl;
        }
        filename = search_file.getPath();
        return (dbt->factory)(this);
      }
    }
  }

  return 0;
}
