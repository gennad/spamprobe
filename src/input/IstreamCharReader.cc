///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: IstreamCharReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "IstreamCharReader.h"

class IstreamReaderPosition : public AbstractCharReaderPosition
{
public:
  IstreamReaderPosition(streampos _position) : position(_position)
  {
  }

  virtual ~IstreamReaderPosition()
  {
  }

  streampos position;
};

IstreamCharReader::IstreamCharReader(istream *stream,
                                     bool can_seek)
  : m_stream(stream),
    m_rdbuf(stream->rdbuf()),
    m_canSeek(can_seek)
{
}

IstreamCharReader::~IstreamCharReader()
{
}

bool IstreamCharReader::forward()
{
  int ichr = m_rdbuf->sbumpc();
  if (ichr == EOF) {
    return false;
  }
  setCurrentChar(safe_char(ichr));
  return true;
}

bool IstreamCharReader::hasChar()
{
  return m_rdbuf->sgetc() != EOF;
}

bool IstreamCharReader::atEnd()
{
  return m_rdbuf->sgetc() == EOF;
}

bool IstreamCharReader::skip(int nchars)
{
  while (nchars-- > 0) {
    if (!forward()) {
      return false;
    }
  }
  return true;
}

OWNED AbstractCharReaderPosition *IstreamCharReader::createMark()
{
  return m_canSeek ? new IstreamReaderPosition(m_rdbuf->pubseekoff(0,ios::cur,ios::in)) : 0;
}

void IstreamCharReader::returnToMark(AbstractCharReaderPosition *pos)
{
  if (m_canSeek && pos) {
    m_rdbuf->pubseekpos(dynamic_cast<IstreamReaderPosition*>(pos)->position);
  }
}
