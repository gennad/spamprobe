///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "RegularExpression.h"
#include "MailMessage.h"
#include "MailMessageList.h"
#include "LineReader.h"
#include "MultiLineString.h"
#include "MailMessageReader.h"

// Defines how many chars to deduct from content-length so that we can
// find a blank line before the message delimiter line that follows
// the message.
static const int CONTENT_LENGTH_PADDING = 10;

MailMessageReader::MailMessageReader(bool use_content_length)
  : m_useContentLength(use_content_length)
{
}

MailMessageReader::~MailMessageReader()
{
}

bool MailMessageReader::findContentLength(MultiLineString *text,
                                          int &content_length)
{
  RegularExpression regex("^content-length: *([0-9]+)", 2, true);
  for (int i = 0; i < text->lineCount(); ++i) {
    if (regex.match(text->line(i))) {
      string buffer;
      content_length = max(0, atoi(regex.getMatch(1, buffer).c_str()) - CONTENT_LENGTH_PADDING);
      return true;
    }
  }
  return false;
}

OWNED MailMessage *MailMessageReader::readMessage()
{
  Ref<MultiLineString> text(new MultiLineString());
  if (m_useContentLength) {
    readMessageWithContentLength(text.ptr());
  } else {
    readMessageWithBoundary(text.ptr(), false);
  }
  return (text->lineCount() > 0) ? new MailMessage(text) : 0;
}

void MailMessageReader::readMessageWithBoundary(MultiLineString *text,
                                                bool append_to_last_line)
{
  while (reader()->hasLine()) {
    if (isNewMessageBoundary(reader()->currentLine())) {
      break;
    }
    if (is_debug) {
      cerr << "LINE: '" << reader()->currentLine() << "'" << endl;
    }
    if (append_to_last_line) {
      text->appendToLastLine(reader()->currentLine());
      append_to_last_line = false;
    } else {
      text->addLine(reader()->currentLine());
    }
    reader()->forward();
  }
}

void MailMessageReader::readMessageWithContentLength(MultiLineString *text)
{
  while (reader()->hasLine()) {
    if (is_debug) {
      cerr << "HDR: '" << reader()->currentLine() << "'" << endl;
    }
    text->addLine(reader()->currentLine());
    if (reader()->currentLine().length() == 0) {
      break;
    }
    reader()->forward();
  }

  bool should_append = false;
  int content_length = 0;
  if (findContentLength(text, content_length)) {
    should_append = content_length > 0;
    while (reader()->forward(content_length)) {
      if (is_debug) {
        cerr << "CL: " << content_length << ": " << reader()->currentLine() << endl;
      }
      text->addLine(reader()->currentLine());
    }
    reader()->forward();
  }

  readMessageWithBoundary(text, should_append);
}

bool MailMessageReader::isNewMessageBoundary(const string &line)
{
  return false;
}
