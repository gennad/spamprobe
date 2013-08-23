///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MessageHeader.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _MessageHeader_h
#define _MessageHeader_h

#include "AbstractMultiLineString.h"

class MessageHeader
{
public:
  MessageHeader(const string &name,
                const CRef<AbstractMultiLineString> &lines);
  ~MessageHeader();

  const string &name() const
  {
    return m_name;
  }

  const string &lowerName() const
  {
    return m_lowerName;
  }

  bool hasName(const string &name)
  {
    return m_lowerName == name;
  }

  const CRef<AbstractMultiLineString> &lines() const
  {
    return m_lines;
  }

private:
  string m_name;
  string m_lowerName;
  CRef<AbstractMultiLineString> m_lines;
};

#endif
