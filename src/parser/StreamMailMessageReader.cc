///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: StreamMailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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
#include "LineReader.h"
#include "MbxMailMessageReader.h"
#include "MBoxMailMessageReader.h"
#include "StreamMailMessageReader.h"

StreamMailMessageReader::StreamMailMessageReader()
  : m_stream(0)
{
}

StreamMailMessageReader::~StreamMailMessageReader()
{
  close();
}

void StreamMailMessageReader::close()
{
  m_mailReader.clear();
  m_lineReader.clear();
  m_charReader.clear();
  m_stream = 0;
}

void StreamMailMessageReader::setStream(istream *in_stream,
                                        bool can_seek,
                                        bool ignore_from,
                                        bool ignore_content_length)
{
  close();
  m_stream = in_stream;
  m_charReader.set(new IstreamCharReader(in_stream, can_seek));
  m_lineReader.set(new LineReader(m_charReader.get()));
  if (MbxMailMessageReader::isMbxFile(m_charReader.get())) {
    m_mailReader.set(new MbxMailMessageReader());
  } else if (ignore_from) {
    m_mailReader.set(new MailMessageReader());
  } else {
    m_mailReader.set(new MBoxMailMessageReader(!ignore_content_length));
  }
  m_mailReader->setLineReader(m_lineReader.get());
  m_lineReader->forward();
}

OWNED MailMessage *StreamMailMessageReader::readMessage()
{
  assert(m_mailReader.isNotNull());
  return m_mailReader->readMessage();
}
