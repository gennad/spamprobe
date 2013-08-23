///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: DatabaseConfig.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _DatabaseConfig_h
#define _DatabaseConfig_h

#include "util.h"

class FrequencyDBImpl;

class DatabaseConfig
{
public:
  DatabaseConfig();
  ~DatabaseConfig();

  void setDatabaseType(const string &value)
  {
    m_databaseType = value;
  }

  const string &databaseType() const
  {
    return m_databaseType;
  }

  void setPrivateFilename(const string &filename)
  {
    m_privateFilename = filename;
  }

  const string &privateFilename() const
  {
    return m_privateFilename;
  }

  void setSharedFilename(const string &filename)
  {
    m_sharedFilename = filename;
  }

  const string &sharedFilename() const
  {
    return m_sharedFilename;
  }

  void setTargetSizeMB(int value)
  {
    m_targetSizeMB = value;
  }

  int targetSizeMB() const
  {
    return m_targetSizeMB;
  }

  void setMaxCacheTerms(int value)
  {
    m_maxCacheTerms = value;
  }

  int maxCacheTerms() const
  {
    return m_maxCacheTerms;
  }

  void parseCommandLineArg(string &arg,
                           string &database_type,
                           int &target_size_mb);

  OWNED FrequencyDBImpl *createDatabaseImpl(bool read_only) const;

  OWNED FrequencyDBImpl *createDatabaseImplUsingDefaultType(string &filename) const;

private:
  /// Not implemented.
  DatabaseConfig(const DatabaseConfig &);

  /// Not implemented.
  DatabaseConfig& operator=(const DatabaseConfig &);


  OWNED FrequencyDBImpl *createDatabaseImplUsingBestFit(const string &type,
                                                        string &filename) const;

  OWNED FrequencyDBImpl *createDatabaseImplForType(const string &type,
                                                   string &filename) const;

  OWNED FrequencyDBImpl *createDatabaseImplUsingExistingFile(string &filename) const;

private:
  string m_databaseType;
  string m_privateFilename;
  string m_sharedFilename;
  int m_targetSizeMB;
  int m_maxCacheTerms;
};

#endif // _DatabaseConfig_h
