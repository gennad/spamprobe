///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FrequencyDBImpl_h
#define _FrequencyDBImpl_h

#include "FrequencyDB.h"

class CleanupManager;
class DatabaseConfig;
class WordData;

class FrequencyDBImpl
{
public:
  virtual ~FrequencyDBImpl()
  {
  }

  virtual bool open(const string &filename,
                    bool read_only,
                    int create_mode) = 0;
  virtual void close() = 0;

  virtual void flush() = 0;

  virtual void beginTransaction()
  {
  }

  virtual void endTransaction(bool commit)
  {
  }

  virtual void writeWord(const string &word,
                         const WordData &word_data) = 0;

  virtual bool readWord(const string &word,
                        WordData &word_data) = 0;

  virtual bool readWord(const string &word,
                        WordData &word_data,
			bool &is_shared)
  {
    is_shared = false;
    return readWord(word, word_data);
  }

  virtual bool firstWord(string &word,
                         WordData &counts) = 0;

  virtual bool nextWord(string &word,
                        WordData &counts) = 0;

  virtual string getDatabaseType() const = 0;

  virtual void sweepOutOldTerms(const CleanupManager &cleanman) = 0;

  virtual bool canCacheTerm(const string &word)
  {
    return true;
  }

  static const int SHARED_DB_MODE;
  static const int PRIVATE_DB_MODE;
};

typedef FrequencyDBImpl *(*FrequencyDBImplFactory)(const DatabaseConfig *config);

#endif // _FrequencyDBImpl_h
