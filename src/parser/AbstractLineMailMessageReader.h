///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractLineMailMessageReader.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _AbstractLineMailMessageReader_h
#define _AbstractLineMailMessageReader_h

#include "AbstractMailMessageReader.h"

class AbstractLineMailMessageReader : public AbstractMailMessageReader
{
public:
  AbstractLineMailMessageReader() : m_reader(0) {}
  virtual ~AbstractLineMailMessageReader() {}

  void setLineReader(LineReader *reader)
  {
    m_reader = reader;
  }

  LineReader *reader()
  {
    return m_reader;
  }

private:
  bool readMBXRecordHeader(LineReader *reader);

private:
  LineReader *m_reader;
};

#endif
