///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: TraditionalMailMessageParser.cc 272 2007-01-06 19:37:27Z brian $
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

#include "GifParser.h"
#include "PngParser.h"
#include "JpegParser.h"
#include "ParserConfig.h"
#include "AbstractMultiLineString.h"
#include "AbstractTokenizer.h"
#include "AbstractTokenReceiver.h"
#include "MessageHeader.h"
#include "MessageHeaderList.h"
#include "MailMessage.h"
#include "MailMessageList.h"
#include "SimpleTokenizer.h"
#include "PhrasingTokenizer.h"
#include "UrlOnlyHtmlTokenizer.h"
#include "TokenFilteringTokenizer.h"
#include "SimpleMultiLineStringCharReader.h"
#include "StringReader.h"
#include "MimeDecoder.h"
#include "TraditionalMailMessageParser.h"

static const string URL_PREFIX("U_");
static const string IP_ADDRESS_REGEX("[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+");
static const string IP_ADDRESS_TERM("IP_ADDRESS");
static const string LINE_SEPARATOR(" ");

TraditionalMailMessageParser::TraditionalMailMessageParser(ParserConfig *config)
  : m_config(config),
    m_ipRegex(IP_ADDRESS_REGEX)
{
}

TraditionalMailMessageParser::~TraditionalMailMessageParser()
{
}

OWNED AbstractTokenizer *TraditionalMailMessageParser::createTokenizer()
{
  Ptr<AbstractTokenizer> answer;
  answer.set(new SimpleTokenizer(m_config->getNonAsciiCharReplacement()));
  answer.set(new TokenFilteringTokenizer(answer.release(), m_config->getMinTermLength(), m_config->getMaxTermLength(), false));
  answer.set(new PhrasingTokenizer(answer.release(), m_config->getMinPhraseTerms(), m_config->getMaxPhraseTerms(), m_config->getMinPhraseChars(), m_config->getMaxPhraseChars()));
  return answer.release();
}

OWNED Message *TraditionalMailMessageParser::parseMailMessage(MailMessage *source)
{
  m_message.set(new Message());
  m_message->setMaxTokenCount(m_config->getMaxTermsPerMessage());
  Ptr<AbstractTokenizer> html_text_tokenizer(createTokenizer());
  m_textTokenizer.set(createTokenizer());
  if (m_config->getRemoveHTML()) {
    m_htmlTokenizer.set(new UrlOnlyHtmlTokenizer(m_textTokenizer.get(), html_text_tokenizer.get(), 256, m_config->getKeepSuspiciousTags()));
  } else {
    m_htmlTokenizer.set(new HtmlTokenizer(m_textTokenizer.get(), html_text_tokenizer.get(), 256));
  }
  parseBody(source);
  return m_message.release();
}

void TraditionalMailMessageParser::receiveToken(const string &prefix,
                                                const string &token)
{
  if (prefix != m_prefix) {
    addTerm(prefix, token, Token::FLAG_NORMAL);
    addDerivedTerms(prefix, token);
  }
  addTerm(m_prefix, token, Token::FLAG_NORMAL);
  addDerivedTerms(m_prefix, token);
}

void TraditionalMailMessageParser::addDerivedTerms(const string &prefix,
                                                   const string &token)
{
  if (isPhrase(token)) {
    return;
  }

  if (m_ipRegex.match(token)) {
    addTerm(prefix, IP_ADDRESS_TERM, Token::FLAG_DERIVED);
  }

  addTokenParts(prefix, token);
}

void TraditionalMailMessageParser::addTokenParts(const string &prefix,
                                                 const string &token)
{
  const char *word_start = token.c_str();
  const char *s = word_start;
  while (*s) {
    while (*s && !is_alnum(*s) && !(*s & 0x80)) {
      ++s;
    }

    bool all_digits = true;
    const char *start = s;
    while (*s && (is_alnum(*s) || (*s & 0x80))) {
      all_digits = all_digits && is_digit(*s);
      ++s;
    }
    const char *end = s;

    if (!all_digits) {
      if (start != word_start) {
        addTerm(prefix, start, Token::FLAG_DERIVED);
      }

      if (((end - start) > 1) && *end && !all_digits) {
        addTerm(prefix, string(start, end), Token::FLAG_DERIVED);
      }
    }
  }
}

void TraditionalMailMessageParser::addTerm(const string &prefix,
                                           const string &term,
                                           int flags)
{
  m_message->addToken(term, prefix, flags);
}

bool TraditionalMailMessageParser::isPhrase(const string &token)
{
  return token.find(' ') != string::npos;
}

void TraditionalMailMessageParser::parseHtmlBodyText(const AbstractMultiLineString *text)
{
    if (is_debug) {
        cerr << "PARSING HTML BODY TEXT LINES: " << text->lineCount() << endl;
    }
    m_prefix.erase();
    SimpleMultiLineStringCharReader reader(text, LINE_SEPARATOR);
    m_htmlTokenizer->tokenize(this, &reader, m_prefix);
    if (is_debug) {
        cerr << "FINISHED PARSING HTML BODY TEXT LINES: " << text->lineCount() << endl;
    }
}

void TraditionalMailMessageParser::parsePlainBodyText(const AbstractMultiLineString *text)
{
    if (is_debug) {
        cerr << "PARSING PLAIN BODY TEXT LINES: " << text->lineCount() << endl;
    }
    m_prefix.erase();
    SimpleMultiLineStringCharReader reader(text, LINE_SEPARATOR);
    m_textTokenizer->tokenize(this, &reader, m_prefix);
    if (is_debug) {
        cerr << "FINISHED PARSING PLAIN BODY TEXT LINES: " << text->lineCount() << endl;
    }
}

void TraditionalMailMessageParser::parseBodyText(MailMessage *source)
{
#if defined(HAVE_UNGIF) || defined(HAVE_PNG) || defined(HAVE_JPEG)
  Ptr<ImageParser> image_parser;
  Buffer<unsigned char> buffer(1024);

  #if defined(HAVE_UNGIF)
  if (source->head()->hasType("image/gif") && source->asData(buffer) && buffer.length() > 0) {
    image_parser.set(new GifParser(m_message.get(), m_textTokenizer.get(), this, "Igif_", buffer));
  }
  #endif

  #if defined(HAVE_PNG)
  if (source->head()->hasType("image/png") && source->asData(buffer) && buffer.length() > 0) {
    image_parser.set(new PngParser(m_message.get(), m_textTokenizer.get(), this, "Ipng_", buffer));
  }
  #endif

  #if defined(HAVE_JPEG)
  if (source->head()->hasType("image/jpeg") && source->asData(buffer) && buffer.length() > 0) {
    image_parser.set(new JpegParser(m_message.get(), m_textTokenizer.get(), this, "Ijpeg_", buffer));
  }
  #endif

  if (image_parser.isNotNull()) {
    image_parser->parseImage();
    return;
  }
#endif // defined(HAVE_UNGIF) || defined(HAVE_PNG) || defined(HAVE_JPEG)

  bool is_html;
  const AbstractMultiLineString *text = source->asText(is_html);
  if (!text) {
    if (is_debug) {
      cerr << "IGNORING NON-TEXT PART" << endl;
    }
    return;
  }

  if (is_debug) {
    for (int i = 0; i < text->lineCount(); ++i) {
      cerr << "TEXT: " << i << ": " << text->line(i) << "\n";
    }
  }

  if (is_html) {
    parseHtmlBodyText(text);
  } else {
    parsePlainBodyText(text);
  }
}

void TraditionalMailMessageParser::parseCharset(MailMessage *source,
                                                const string &prefix)
{
    string charset;
    source->head()->getCharsetString(charset);
    if (charset.length() > 0) {
        m_prefix = prefix;
        StringReader reader(charset);
        m_textTokenizer->tokenize(this, &reader, m_prefix);
    }
}

void TraditionalMailMessageParser::parseHeader(const MessageHeader *header,
                                               MimeDecoder *decoder)
{
  CRef<AbstractMultiLineString> lines(decoder->decodeHeaderString(header->lines()));
  SimpleMultiLineStringCharReader reader(lines.ptr());
  m_textTokenizer->tokenize(this, &reader, m_prefix);
}

void TraditionalMailMessageParser::parseHeaders(MailMessage *source)
{
  MimeDecoder decoder;
  m_config->headers()->resetHeaderCounts();
  const MessageHeaderList *head = source->head();
  for (int i = 0; i < head->headerCount(); ++i) {
    const MessageHeader *header = head->header(i);
    if (!starts_with(header->lowerName(), "from ")) {
      if (m_config->headers()->shouldProcessHeader(header->lowerName(), m_prefix)) {
        parseHeader(header, &decoder);
      }
    }
  }
  parseCharset(source, "CS_");
}

void TraditionalMailMessageParser::parseBody(MailMessage *source)
{
  if (is_debug) {
    cerr << "parseBody: begins " << source->bodyText()->line(0) << endl;
  }
  parseHeaders(source);

  if (source->bodyText()->lineCount() == 0) {
    if (is_debug) {
      cerr << "parseBody: ignoring empty body" << endl;
    }
    return;
  }

  if (!m_config->getIgnoreBody()) {
    if (source->hasParts()) {
      for (int i = 0; i < source->body()->messageCount(); ++i) {
        parseBody(source->body()->message(i));
      }
    } else {
      parseBodyText(source);
    }
  }

  if (is_debug) {
    cerr << "parseBody: ends " << source->bodyText()->line(source->bodyText()->lineCount() - 1) << endl;
  }
}
