///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MultiLineSubString.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MultiLineSubString.h"

MultiLineSubString::MultiLineSubString(const CRef<AbstractMultiLineString> &target,
                                       int start_index,
                                       int limit_index)
: m_target(target)
{
  assert(m_target.isNotNull());
  assert(start_index >= 0);
  assert(limit_index >= 0);

  start_index = max(0, start_index);
  limit_index = max(0, min(limit_index, m_target->lineCount()));

  if (m_target->lineCount() == 0 || start_index >= m_target->lineCount() || start_index >= limit_index) {
    m_startIndex = m_target->lineCount(); // force out_of_range exception if ::line() called
    m_length = 0;
  } else {
    m_startIndex = start_index;
    m_length = limit_index - start_index;
  }

  assert(m_startIndex >= 0);
  assert(m_startIndex <= m_target->lineCount());
  assert(m_length >= 0);
  assert(m_length <= m_target->lineCount());
}

MultiLineSubString::~MultiLineSubString()
{
}

int MultiLineSubString::lineCount() const
{
  return m_length;
}

const string &MultiLineSubString::line(int index) const
{
  assert(index >= 0);
  assert(index < m_length);
  return m_target->line(m_startIndex + index);
}

