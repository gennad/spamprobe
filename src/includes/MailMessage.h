///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MailMessage.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _MailMessage_h
#define _MailMessage_h

#include "util.h"
#include "Buffer.h"

class AbstractMultiLineString;
class MessageHeaderList;
class MailMessageList;

class MailMessage
{
public:
  MailMessage(const CRef<AbstractMultiLineString> &full_text);
  MailMessage(const CRef<AbstractMultiLineString> &head_text,
              const CRef<AbstractMultiLineString> &body_text,
              const CRef<AbstractMultiLineString> &full_text);
  ~MailMessage();

  CRef<AbstractMultiLineString> &headText()
  {
    return m_headText;
  }

  CRef<AbstractMultiLineString> &bodyText()
  {
    return m_bodyText;
  }

  CRef<AbstractMultiLineString> &fullText()
  {
    return m_fullText;
  }

  const MessageHeaderList *head();
  const MailMessageList *body();

  bool isMultiPart();
  bool hasParts();

  const AbstractMultiLineString *asText(bool &is_html);
  bool asData(Buffer<unsigned char> &buffer);

private:
  bool isMultiPartType();
  bool isHtml(const AbstractMultiLineString *text);

private:
  CRef<AbstractMultiLineString> m_headText;
  CRef<AbstractMultiLineString> m_bodyText;
  CRef<AbstractMultiLineString> m_fullText;
  Ref<MessageHeaderList> m_head;
  Ref<MailMessageList> m_body;

  CRef<AbstractMultiLineString> m_decodedText;
  bool m_isHtml;
};

#endif
