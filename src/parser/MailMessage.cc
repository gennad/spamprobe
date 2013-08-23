///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MailMessage.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MultiLineString.h"
#include "MultiLineSubString.h"
#include "MessageHeaderList.h"
#include "MimeDecoder.h"
#include "RegularExpression.h"
#include "BodyParser.h"
#include "HeaderParser.h"
#include "MailMessage.h"
#include "MailMessageList.h"

static const string IS_HTML_REGEX("<!|</?html>|<p>|<br/?>|</?tr>|</?td>|</?font |</?b>|<a ");

static void dump(MailMessage *msg)
{
  cerr << "PARSING MESSAGE: " << endl;
  for (int i = 0; i < msg->headText()->lineCount(); ++i) {
    cerr << "HEAD: '" << msg->headText()->line(i) << "'" << endl;
  }
  for (int i = 0; i < msg->bodyText()->lineCount(); ++i) {
    cerr << "BODY: '" << msg->bodyText()->line(i) << "'" << endl;
  }
}

MailMessage::MailMessage(const CRef<AbstractMultiLineString> &full_text)
  : m_fullText(full_text)
{
    int first_blank = 0;
    while (first_blank < full_text->lineCount()) {
        if (full_text->line(first_blank).length() == 0) {
            break;
        }
        ++first_blank;
    }
    m_headText = make_ref(new MultiLineSubString(full_text, 0, first_blank));
    m_bodyText = make_ref(new MultiLineSubString(full_text, first_blank + 1));
}

MailMessage::MailMessage(const CRef<AbstractMultiLineString> &head_text,
                         const CRef<AbstractMultiLineString> &body_text,
                         const CRef<AbstractMultiLineString> &full_text)
: m_headText(head_text),
  m_bodyText(body_text),
  m_fullText(full_text)
{
}

MailMessage::~MailMessage()
{
}

const MessageHeaderList *MailMessage::head()
{
    if (m_head.isNull()) {
        HeaderParser parser;
        m_head = parser.parseHeader(m_headText);
    }
    return m_head.ptr();
}

const MailMessageList *MailMessage::body()
{
    if (m_body.isNull()) {
        if (isMultiPartType()) {
            BodyParser parser;
            m_body = parser.parseBody(head(), m_bodyText);
        } else {
            m_body = make_ref(new MailMessageList());
        }
    }
    return m_body.ptr();
}

bool MailMessage::isMultiPart()
{
    return isMultiPartType();
}

bool MailMessage::hasParts()
{
    return body()->messageCount() >= 1;
}

bool MailMessage::isHtml(const AbstractMultiLineString *text)
{
    string content_type;
    if (head()->getContentTypeString(content_type).length() > 0) {
        return starts_with(content_type, "text/html");
    }

    if (text->lineCount() == 0) {
        return false;
    }

    RegularExpression html_regex(IS_HTML_REGEX, 1, true);
    for (int i = 0; i < 4 && i < text->lineCount(); ++i) {
        if (html_regex.match(text->line(i))) {
            return true;
        }
    }

    return false;
}

bool MailMessage::isMultiPartType()
{
    return head()->hasBoundaryString() || head()->isMessageType();
}

const AbstractMultiLineString *MailMessage::asText(bool &is_html)
{
    if (body()->messageCount() == 1) {
        return body()->message(0)->asText(is_html);
    }

    if (body()->messageCount() != 0) {
        if (is_debug) {
            cerr << "IGNORED MULTI-PART BODY" << endl;
        }
        return 0;
    }

    if (!head()->isTextType()) {
        if (is_debug) {
            cerr << "IGNORED NON-TEXT BODY" << endl;
        }
        return 0;
    }

    if (m_decodedText.isNull()) {
        MimeDecoder decoder;
        m_decodedText = decoder.decodeText(head(), m_bodyText);
        m_isHtml = isHtml(m_decodedText.ptr());
        if (is_debug) {
            cerr << "DECODED BODY" << endl;
        }
    }
    is_html = m_isHtml;
    return m_decodedText.ptr();
}

bool MailMessage::asData(Buffer<unsigned char> &buffer)
{
  MimeDecoder decoder;
  if (!decoder.isBase64(head()->header("content-transfer-encoding"))) {
    return false;
  }

  buffer.reset(60 * m_bodyText->lineCount());
  decoder.decodeData(m_bodyText, buffer);
  return true;
}
