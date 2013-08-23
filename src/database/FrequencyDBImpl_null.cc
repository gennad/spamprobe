///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDBImpl_null.cc 272 2007-01-06 19:37:27Z brian $
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

#include "FrequencyDBImpl_null.h"

static const string DATABASE_TYPE("null");

FrequencyDBImpl_null::FrequencyDBImpl_null()
{
}

FrequencyDBImpl_null::~FrequencyDBImpl_null()
{
}

bool FrequencyDBImpl_null::open(const string &filename,
                                bool read_only,
                                int create_mode)
{
  return true;
}

void FrequencyDBImpl_null::close()
{
}

void FrequencyDBImpl_null::flush()
{
}

void FrequencyDBImpl_null::clear()
{
}

void FrequencyDBImpl_null::writeWord(const string &word,
                                      const WordData &counts)
{
}

bool FrequencyDBImpl_null::readWord(const string &word,
                                     WordData &counts)
{
  return false;
}

bool FrequencyDBImpl_null::firstWord(string &word,
                                      WordData &counts)
{
  return false;
}

bool FrequencyDBImpl_null::nextWord(string &word,
                                     WordData &counts)
{
  return false;
}

string FrequencyDBImpl_null::getDatabaseType() const
{
  return DATABASE_TYPE;
}

void FrequencyDBImpl_null::sweepOutOldTerms(const CleanupManager &cleanman)
{
}
