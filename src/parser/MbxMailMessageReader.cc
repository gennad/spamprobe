///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MbxMailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include <stdexcept>
#include "MailMessage.h"
#include "MailMessageList.h"
#include "LineReader.h"
#include "MultiLineString.h"
#include "MbxMailMessageReader.h"

static const int MBX_FILE_FIRST_LINE_LENGTH = 7;
static const int MBX_FILE_HEADER_LENGTH = 2048;
static const int MBX_SKIP_HEADER_COUNT = MBX_FILE_HEADER_LENGTH - MBX_FILE_FIRST_LINE_LENGTH;
static const unsigned long MBX_DELETED_FLAG = 0x0002;
static const string MBX_RECORD_HEADER_REGEX("^[ 0-9][0-9]+-[A-Za-z]+-[0-9]+ [0-9]+:[0-9]+:[0-9]+ .?[0-9]+,([0-9]+);[--9a-zA-Z]+([--9a-zA-Z][--9a-zA-Z][--9a-zA-Z][--9a-zA-Z])-[0-9a-zA-Z]+$");

MbxMailMessageReader::MbxMailMessageReader()
  : m_headerRegex(MBX_RECORD_HEADER_REGEX, 3),
    m_recordLength(0)
{
}

MbxMailMessageReader::~MbxMailMessageReader()
{
}

bool MbxMailMessageReader::readMBXRecordHeader()
{
  bool again = true;
  while (again) {
    m_recordLength = 0;
    if (!reader()->hasLine()) {
      return false;
    }

    if (is_debug) {
      cerr << "MBX: HDR: " << reader()->currentLine() << endl;
    }

    if (!m_headerRegex.match(reader()->currentLine())) {
      throw runtime_error(string("IMAP-MBX: regex for record header did not match: ") + reader()->currentLine());
    }

    string matched;
    m_recordLength = atoi(m_headerRegex.getMatch(1, matched).c_str());
    unsigned long flags = strtoul(m_headerRegex.getMatch(2, matched).c_str(), 0, 16);
    if (is_debug) {
      cerr << "MBX: FLAGS " << hex << flags << " RECLEN " << dec << m_recordLength << endl;
    }
    if (!(flags & MBX_DELETED_FLAG)) {
      return true;
    }

    // message is marked deleted
    reader()->skipChars(m_recordLength);
    reader()->forward();
    if (is_debug) {
      cerr << "MBX: SKIPPED DELETED MESSAGE" << endl;
    }
  }
}

OWNED MailMessage *MbxMailMessageReader::readMessage()
{
  if (!readMBXRecordHeader()) {
    if (is_debug) {
      cerr << "MBX: END OF FILE" << endl;
    }
    return 0;
  }

  Ref<MultiLineString> text(new MultiLineString());
  while (reader()->forward(m_recordLength)) {
    text->addLine(reader()->currentLine());
  }
  reader()->forward();
  return (text->lineCount() > 0) ? new MailMessage(text) : 0;
}

bool MbxMailMessageReader::isMbxFile(AbstractCharReader *in)
{
  Ptr<AbstractCharReaderPosition> start_pos(in->createMark());
  if (start_pos.isNull()) {
    return false;
  }

  if (in->matchChar('*') &&
      in->matchChar('m') &&
      in->matchChar('b') &&
      in->matchChar('x') &&
      in->matchChar('*') &&
      in->matchChar('\r') &&
      in->matchChar('\n')) {
    in->skip(MBX_SKIP_HEADER_COUNT);
    return true;
  }

  in->returnToMark(start_pos.get());
  return false;
}
