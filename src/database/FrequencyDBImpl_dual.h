///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_dual.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FrequencyDBImpl_dual_h
#define _FrequencyDBImpl_dual_h

#include "FrequencyDBImpl.h"

class FrequencyDBImpl_dual : public FrequencyDBImpl
{
public:
  FrequencyDBImpl_dual(FrequencyDBImpl *shared_db,
                       FrequencyDBImpl *private_db,
                       const string &shared_db_filename);
  ~FrequencyDBImpl_dual();

  bool open(const string &filename,
            bool read_only,
            int create_mode);
  void close();

  void flush();

  void writeWord(const string &word,
                 const WordData &word_data);

  bool readWord(const string &word,
                WordData &word_data);

  bool readWord(const string &word,
                        WordData &word_data,
			bool &is_shared);

  bool firstWord(string &word,
                 WordData &counts);

  bool nextWord(string &word,
                WordData &counts);

  string getDatabaseType() const;

  void sweepOutOldTerms(const CleanupManager &cleanman);

  bool canCacheTerm(const string &word);

private:
  /// Not implemented.
  FrequencyDBImpl_dual(const FrequencyDBImpl_dual &);

  /// Not implemented.
  FrequencyDBImpl_dual& operator=(const FrequencyDBImpl_dual &);

private:
  Ptr<FrequencyDBImpl> m_sharedDB;
  Ptr<FrequencyDBImpl> m_privateDB;
  string m_sharedFilename;
};

#endif // _FrequencyDBImpl_dual_h
