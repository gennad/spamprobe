///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: TraditionalMailMessageParser.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _TraditionalMailMessageParser_h
#define _TraditionalMailMessageParser_h

#include "Message.h"
#include "MailMessage.h"
#include "RegularExpression.h"
#include "AbstractTokenReceiver.h"
#include "AbstractMailMessageParser.h"

class AbstractTokenizer;
class ParserConfig;
class MessageHeader;
class MimeDecoder;

class TraditionalMailMessageParser : public AbstractMailMessageParser, public AbstractTokenReceiver
{
public:
  TraditionalMailMessageParser(ParserConfig *config);
  virtual ~TraditionalMailMessageParser();

  virtual OWNED Message *parseMailMessage(MailMessage *source);

  void receiveToken(const string &prefix, const string &token);

protected:
  virtual void addDerivedTerms(const string &prefix,
                               const string &token);
  virtual void addTokenParts(const string &prefix,
                             const string &token);

private:
  void addTerm(const string &prefix,
               const string &term,
               int flags);
  bool isPhrase(const string &token);
  void parseHeader(const MessageHeader *header,
                   MimeDecoder *decoder);
  void parseHtmlBodyText(const AbstractMultiLineString *text);
  void parsePlainBodyText(const AbstractMultiLineString *text);
  void parseBodyText(MailMessage *source);
  void parseCharset(MailMessage *source,
                    const string &prefix);
  virtual void parseHeaders(MailMessage *source);
  virtual void parseBody(MailMessage *source);
  virtual OWNED AbstractTokenizer *createTokenizer();

private:
  string m_prefix;
  Ptr<Message> m_message;
  Ptr<AbstractTokenizer> m_textTokenizer;
  Ptr<AbstractTokenizer> m_htmlTokenizer;
  RegularExpression m_ipRegex;

  ParserConfig *m_config;
};

#endif
