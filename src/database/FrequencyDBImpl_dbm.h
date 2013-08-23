///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_dbm.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FrequencyDBImpl_dbm_h
#define _FrequencyDBImpl_dbm_h

#ifdef USE_DBM

extern "C" {
#include <ndbm.h>
}

#include "FrequencyDBImpl.h"

class LockFile;

class FrequencyDBImpl_dbm : public FrequencyDBImpl
{
public:
  FrequencyDBImpl_dbm();
  ~FrequencyDBImpl_dbm();

  bool open(const string &filename,
            bool read_only,
            int create_mode);
  void close();

  void flush();

  void writeWord(const string &word,
                 const WordData &word_data);

  bool readWord(const string &word,
                WordData &word_data);

  bool firstWord(string &word,
                 WordData &counts);

  bool nextWord(string &word,
                WordData &counts);

  string getDatabaseType() const;

  void sweepOutOldTerms(const CleanupManager &cleanman);

  static FrequencyDBImpl *factory(const DatabaseConfig *);

private:
  bool loadKey(const datum &key,
               string &word,
               WordData &counts) const;

  bool loadKey(const datum &key,
               WordData &counts) const;

private:
  /// Not implemented.
  FrequencyDBImpl_dbm(const FrequencyDBImpl_dbm &);

  /// Not implemented.
  FrequencyDBImpl_dbm& operator=(const FrequencyDBImpl_dbm &);

private:
  DBM *m_file;
};

#endif // USE_DBM

#endif // _FrequencyDBImpl_dbm_h
