///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "AbstractCharReader.h"
#include "HdlError.h"
#include "HdlToken.h"
#include "HdlTokenizer.h"

static const string invalid_char_message(const string base_msg,
                                         char ch)
{
  return base_msg + ": '" + ch + "'";
}

HdlTokenizer::HdlTokenizer(const string &filename,
                           AbstractCharReader *reader)
  : m_filename(filename),
    m_lineNumber(1),
    m_haveToken(false),
    m_isSemiColon(false),
    m_reader(reader)
{
  m_reader->forward();
}

HdlTokenizer::~HdlTokenizer()
{
}

bool HdlTokenizer::nextToken()
{
  m_haveToken = false;
  m_isSemiColon = false;

  for (;;) {
    if (m_reader->atEnd()) {
      return false;
    }

    char ch = m_reader->currentChar();
    if (ch == ';') {
      m_haveToken = true;
      m_isSemiColon = true;
      m_reader->forward();
      return true;
    }

    if (is_digit(ch)) {
      return readNumber();
    }

    if (ch == '"' || ch == '\'') {
      return readString(ch);
    }

    if (is_alpha(ch)) {
      return readIdentifier();
    }

    if (ch == '#') {
      skipToEOL();
      ch = m_reader->currentChar();
    }

    if (!is_space(ch)) {
      throw HdlError(invalid_char_message("unexpected character", ch), m_filename, m_lineNumber);
    }

    if (ch == '\n') {
      m_lineNumber += 1;
    }

    m_reader->forward();
  }
}

void HdlTokenizer::skipToEOL()
{
  while (m_reader->hasChar() && m_reader->currentChar() != '\n') {
    m_reader->forward();
  }
}

bool HdlTokenizer::onInvalidInterTokenCharacter()
{
  if (!m_reader->hasChar()) {
    return false;
  }

  char ch = m_reader->currentChar();
  if (ch == ';' || is_space(ch)) {
    return false;
  }

  return true;
}

bool HdlTokenizer::readNumber()
{
  bool have_decimal = false;
  string digits;

  while (m_reader->hasChar() && is_digit(m_reader->currentChar())) {
    digits += m_reader->currentChar();
    m_reader->forward();
  }

  if (m_reader->hasChar() && m_reader->currentChar() == '.') {
    digits += '.';
    have_decimal = true;
    m_reader->forward();
    while (m_reader->hasChar() && is_digit(m_reader->currentChar())) {
      digits += m_reader->currentChar();
      m_reader->forward();
    }
  }

  if (onInvalidInterTokenCharacter()) {
    throw HdlError(invalid_char_message("invalid character after number", m_reader->currentChar()), m_filename, m_lineNumber);
  }

  m_haveToken = true;
  if (have_decimal) {
    m_token = HdlToken::doubleToken(atof(digits.c_str()), m_filename, m_lineNumber);
  } else {
    m_token = HdlToken::intToken(atoi(digits.c_str()), m_filename, m_lineNumber);
  }

  return true;
}

bool HdlTokenizer::readIdentifier()
{
  string chars;

  while (m_reader->hasChar() && (is_alnum(m_reader->currentChar()) || m_reader->currentChar() == '_')) {
    chars += m_reader->currentChar();
    m_reader->forward();
  }

  if (onInvalidInterTokenCharacter()) {
    throw HdlError(invalid_char_message("invalid character after identifier", m_reader->currentChar()), m_filename, m_lineNumber);
  }

  m_haveToken = true;
  m_token = HdlToken::idToken(chars, m_filename, m_lineNumber);

  return true;
}

bool HdlTokenizer::readString(char end_quote)
{
  string chars;

  m_reader->forward(); // skip open quote

  for (;;) {
    if (!m_reader->hasChar()) {
      throw HdlError("unterminated string", m_filename, m_lineNumber);
    }

    if (m_reader->currentChar() == end_quote) {
      break;
    }

    chars += m_reader->currentChar();
    m_reader->forward();
  }

  m_reader->forward(); // skip past the quote

  if (onInvalidInterTokenCharacter()) {
    throw HdlError(invalid_char_message("invalid character after string", m_reader->currentChar()), m_filename, m_lineNumber);
  }

  m_haveToken = true;
  m_token = HdlToken::strToken(chars, m_filename, m_lineNumber);

  return true;
}

bool HdlTokenizer::isSemiColon()
{
  return m_haveToken && m_isSemiColon;
}

const CRef<HdlToken> &HdlTokenizer::token() const
{
  return m_token;
}

const Ref<HdlToken> HdlTokenizer::takeToken()
{
  return m_token.transfer();
}
