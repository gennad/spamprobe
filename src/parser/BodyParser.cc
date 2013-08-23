///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: BodyParser.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MessageHeader.h"
#include "MessageHeaderList.h"
#include "MultiLineString.h"
#include "MultiLineSubString.h"
#include "MailMessage.h"
#include "MailMessageList.h"
#include "BodyParser.h"

Ref<MailMessageList> BodyParser::parseBody(const MessageHeaderList *headers,
                                              const CRef<AbstractMultiLineString> &body_text)
{
  m_headers = headers;
  m_bodyText = body_text;
  m_bodyParts = make_ref(new MailMessageList());
  parse();
  m_bodyText.clear();
  return m_bodyParts;
}

int BodyParser::skipToBlank(const CRef<AbstractMultiLineString> &lines,
                            int &offset)
{
  while (offset < lines->lineCount()) {
    if (lines->line(offset).length() == 0) {
      return offset;
    }
    ++offset;
  }
  return lines->lineCount();
}

int BodyParser::skipToNonBlank(const CRef<AbstractMultiLineString> &lines,
                               int &offset)
{
  while (offset < lines->lineCount()) {
    if (lines->line(offset).length() > 0) {
      return offset;
    }
    ++offset;
  }
  return lines->lineCount();
}

void BodyParser::addPart(const CRef<AbstractMultiLineString> &lines)
{
  int offset = 0;
  int header_start = skipToNonBlank(lines, offset);
  int header_end = skipToBlank(lines, offset);
  int body_start = skipToNonBlank(lines, offset);
  int body_end = lines->lineCount();

  CRef<AbstractMultiLineString> head(new MultiLineSubString(lines, header_start, header_end));
  CRef<AbstractMultiLineString> body(new MultiLineSubString(lines, body_start, body_end));
  Ptr<MailMessage> message(new MailMessage(head, body, lines));
  m_bodyParts->addMessage(message.release());
}

void BodyParser::addPartsForBoundary(const CRef<AbstractMultiLineString> &lines,
                                     const string &boundary)
{
  string part_boundary = "--" + boundary;
  string end_boundary = part_boundary + "--";

  int offset = 0;
  offset = skipToNonBlank(lines, offset);

  int prev_offset = -1;
  while (offset < lines->lineCount()) {
    const string &line(lines->line(offset));
    if (line == part_boundary || line == end_boundary) {
      if (prev_offset >= 0 && offset > prev_offset) {
        Ref<AbstractMultiLineString> part(new MultiLineSubString(lines, prev_offset, offset));
        addPart(part);
      }
      if (line == end_boundary) {
        // ignore anything after end boundary
        return;
      }
      prev_offset = offset + 1;
    }
    ++offset;
  }

  // a safety net in case the loser mail client didn't include the end boundary
  if (prev_offset >= 0 && offset > prev_offset) {
    Ref<AbstractMultiLineString> part(new MultiLineSubString(lines, prev_offset, offset));
    addPart(part);
  }
}

void BodyParser::parse()
{
  string boundary;
  m_headers->getBoundaryString(boundary);
  if (boundary.length() > 0) {
    addPartsForBoundary(m_bodyText, boundary);
  } else {
    addPart(m_bodyText);
  }
}
