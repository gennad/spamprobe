///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_split.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FrequencyDBImpl_split_h
#define _FrequencyDBImpl_split_h

#ifdef USE_MMAP

#include "FrequencyDBImpl.h"

class FrequencyDBImpl_split : public FrequencyDBImpl
{
public:
  FrequencyDBImpl_split(const DatabaseConfig *config);
  ~FrequencyDBImpl_split();

  bool open(const string &filename,
            bool read_only,
            int create_mode);
  void close();

  void flush();

  void beginTransaction();

  void endTransaction(bool commit);

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

  bool canCacheTerm(const string &word);

  static FrequencyDBImpl *factory(const DatabaseConfig *);

private:
  /// Not implemented.
  FrequencyDBImpl_split(const FrequencyDBImpl_split &);

  /// Not implemented.
  FrequencyDBImpl_split& operator=(const FrequencyDBImpl_split &);

private:
  const DatabaseConfig *m_config;
  int m_cursor;
  Ptr<FrequencyDBImpl> m_db;
  Ptr<FrequencyDBImpl> m_hash;
};

#endif // USE_MMAP

#endif // _FrequencyDBImpl_split_h
