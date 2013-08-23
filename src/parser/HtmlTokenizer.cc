///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HtmlTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "AbstractTokenReceiver.h"
#include "StringReader.h"
#include "RegularExpression.h"
#include "TempPtr.h"
#include "HtmlTokenizer.h"

static const char SEPARATOR = ' ';
static const int MAX_ENTITY_LENGTH = 6;
static const string URL_REGEX("[^a-z0-9_](href|src)[ \t\r\n]*=[ \t\r\n]*('[^>' \t\r\n]+|\"[^>\" \t\r\n]+|[^> \t\r\n]+)");
static const string TAG_BASE_PREFIX("T");
static const string TAG_BASE_SUFFIX("_");
static const string URL_PREFIX("U_");

HtmlTokenizer::HtmlTokenizer(AbstractTokenizer *textTokenizer,
                             AbstractTokenizer *tagTokenizer,
                             int maxTagLength)
: m_textTokenizer(textTokenizer),
  m_tagTokenizer(tagTokenizer),
  m_maxTagLength(maxTagLength),
  m_reader(0),
  m_receiver(0)
{
}

HtmlTokenizer::~HtmlTokenizer()
{
}

void HtmlTokenizer::tokenize(AbstractTokenReceiver *receiver,
                             AbstractCharReader *reader,
                             const string &prefix)
{
  TempPtr<AbstractCharReader> tmp_reader(m_reader, reader);
  TempPtr<AbstractTokenReceiver> tmp_receiver(m_receiver, receiver);
  m_prefix = prefix;
  m_textTokenizer->tokenize(m_receiver, this, m_prefix);
}

const string &HtmlTokenizer::decodeUrl(const string &url,
                                       string &buffer)
{
  const int len = url.length();
  const int hex_limit = len - 2;
  const char *chars = url.c_str();
  buffer.erase();
  buffer.reserve(len);
  for (int i = 0; i < len; ++i) {
    char ch = chars[i];
    if (ch == '%' && i < hex_limit && is_xdigit(chars[i+1]) && is_xdigit(chars[i+2])) {
      buffer += (char)(hex_to_int(chars[i+1]) << 4 | hex_to_int(chars[i+2]));
      i += 2;
    } else {
      buffer += ch;
    }
  }
  if (is_debug) {
    cerr << "ORIG URL '" << url << "' DECODED '" << buffer << "'" << endl;
  }
  return buffer;
}

void HtmlTokenizer::processTagUrls(const string &tag)
{
  static RegularExpression url_regex(URL_REGEX, 3, true);

  if (isCommentTag(tag)) {
    return;
  }

  int offset = 0;
  string url, decoded_url;
  RegularExpression::MatchData match;
  while (url_regex.match(tag.c_str() + offset)) {
    url_regex.getMatch(2, url);
    url_regex.getMatch(2, match);
    StringReader reader(decodeUrl(url, decoded_url));
    m_tagTokenizer->tokenize(m_receiver, &reader, URL_PREFIX);
    offset += match.end_pos;
    assert(offset <= tag.length());
  }
}

static const string &make_tag_prefix(const string &tag_body,
                                     string &prefix)
{
  assert(tag_body.length() > 0);
  assert(!is_space(tag_body[0]));

  prefix = TAG_BASE_PREFIX;
  if (starts_with(tag_body, "!--")) {
    prefix += "CMT";
  } else {
    for (const char *s = tag_body.c_str(); *s && !is_space(*s) && prefix.length() < 8; ++s) {
      if (is_alnum(*s)) {
        prefix += to_lower(*s);
      }
    }
  }
  prefix += TAG_BASE_SUFFIX;
  return prefix;
}

void HtmlTokenizer::processTagBody(const string &tag)
{
  if (is_debug) {
    cerr << "PROCESSING TAG BODY: " << tag << endl;
  }
  string tag_prefix;
  StringReader reader(tag);
  m_tagTokenizer->tokenize(m_receiver, &reader, make_tag_prefix(tag, tag_prefix));
  // TODO: leaving this in a for a while as a backward compatibility - remove after 3/31/2006
  m_tagTokenizer->tokenize(m_receiver, &reader, URL_PREFIX);
  // TODO: end backward compatibility hook
  if (is_debug) {
    cerr << "PROCESSED TAG BODY: " << tag << endl;
  }
}

bool HtmlTokenizer::isCommentTag(const string &tag)
{
  return starts_with(tag, "!--") && ends_with(tag, "--");
}

bool HtmlTokenizer::isInvisibleTag(const string &tag)
{
  return isCommentTag(tag);
}

bool HtmlTokenizer::isOpenCommentTag(const string &tag)
{
  return starts_with(tag, "!--");
}

bool HtmlTokenizer::isIncompleteCommentTag(const string &tag)
{
  return starts_with(tag, "!--") && !ends_with(tag, "--");
}

bool HtmlTokenizer::processedTag(string &tag)
{
  if (m_reader->currentChar() != '<') {
    return false;
  }

  tag.erase();
  Ptr<AbstractCharReaderPosition> pos(m_reader->createMark());
  while (m_reader->forward() && (static_cast<int>(tag.length()) < m_maxTagLength || isOpenCommentTag(tag))) {
    if (m_reader->currentChar() == '>' && !isIncompleteCommentTag(tag)) {
      if (tag.length() > 0) {
        processTagBody(tag);
        processTagUrls(tag);
        return true;
      } else {
        break;
      }
    }

    char ch = processedEntity() ? currentChar() : m_reader->currentChar();
    if (tag.length() > 0 || !is_space(ch)) {
      tag += ch;
    }
  }

  m_reader->returnToMark(pos.get());
  return false;
}

char HtmlTokenizer::parseEntityInteger(const string &entity)
{
  char answer = ' ';
  if (entity.length() > 1 && (entity[1] == 'x' || entity[1] == 'X')) {
    answer = (char)hex_to_int(entity.c_str() + 1);
  } else {
    answer = (char)atoi(entity.c_str());
  }
  return answer;
}

bool HtmlTokenizer::processEntity(const string &entity)
{
  if (entity == "amp") {
    setCurrentChar('&');
    return true;
  }

  if (entity == "apos") {
    setCurrentChar('\'');
    return true;
  }

  if (entity == "quot") {
    setCurrentChar('"');
    return true;
  }

  if (entity == "lt") {
    setCurrentChar('<');
    return true;
  }

  if (entity == "gt") {
    setCurrentChar('>');
    return true;
  }

  if (entity == "nbsp") {
    setCurrentChar(' ');
    return true;
  }

  if (entity[0] == '#') {
    setCurrentChar(parseEntityInteger(entity));
    return true;
  }

  return false;
}

bool HtmlTokenizer::processedEntity()
{
  if (m_reader->currentChar() != '&') {
    return false;
  }

  Ptr<AbstractCharReaderPosition> startPos(m_reader->createMark());

  string entity;
  while (m_reader->forward() && static_cast<int>(entity.length()) < MAX_ENTITY_LENGTH) {
    if (m_reader->currentChar() == ';') {
      if (!processEntity(entity)) {
        break;
      }
      return true;
    }
    entity += m_reader->currentChar();
  }

  m_reader->returnToMark(startPos.get());
  return false;
}

bool HtmlTokenizer::forward()
{
  while (true) {
    if (!m_reader->forward()) {
      return false;
    }

    if (processedEntity()) {
      return true;
    }

    string tagText;
    if (!processedTag(tagText)) {
      setCurrentChar(m_reader->currentChar());
      return true;
    }

    if (!isInvisibleTag(tagText)) {
      setCurrentChar(' ');
      return true;
    }
  }
}

bool HtmlTokenizer::hasChar()
{
  return m_reader->hasChar();
}

bool HtmlTokenizer::atEnd()
{
  return m_reader->atEnd();
}

bool HtmlTokenizer::skip(int nchars)
{
  bool have_char = true;
  while (have_char && nchars-- > 0) {
    have_char = forward();
  }
  return have_char;
}
