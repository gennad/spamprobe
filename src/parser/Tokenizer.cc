///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Tokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "Tokenizer.h"

Tokenizer::Tokenizer()
  : m_state(STATE_START)
{
}

Tokenizer::~Tokenizer()
{
}

inline bool is_letter_char(char ch)
{
  return (ch & 0x80) || is_alnum(ch) || (ch == '%');
}

inline bool is_special_char(char ch)
{
  switch (ch) {
  case '.':
  case '-':
  case '+':
  case ',':
  case '_':
  case '$':
    return true;

  default:
    return false;
  }
}

bool Tokenizer::addChar(char ch)
{
  switch (m_state) {
  case STATE_START:
    m_word.erase();
    m_state = STATE_WAITING;
    // fall through

  case STATE_WAITING:
    assert(m_word.length() == 0);
    assert(m_pending.length() == 0);

    if (ch == '$' || is_letter_char(ch)) {
      m_word += to_lower(ch);
      m_state = STATE_IN_WORD;
      return false;
    }

    return false;

  case STATE_IN_WORD:
    assert(m_word.length() > 0);
    assert(m_pending.length() == 0);

    if (is_letter_char(ch)) {
      m_word += to_lower(ch);
      return false;
    }

    if (is_special_char(ch)) {
      m_state = STATE_PENDING;
      m_pending += ch;
      return false;
    }

    m_state = STATE_START;
    return true;

  case STATE_PENDING:
    assert(m_word.length() > 0);
    assert(m_pending.length() > 0);

    if (is_letter_char(ch)) {
      m_state = STATE_IN_WORD;
      m_word += m_pending;
      m_word += to_lower(ch);
      m_pending.erase();
      return false;
    }

    if (is_special_char(ch)) {
      m_pending += ch;
      return false;
    }

    m_state = STATE_START;
    m_pending.erase();
    return true;

  default:
    assert(!"not a valid state");
    break;
  }

  assert(!"not reached");
  return false;
}

void Tokenizer::start()
{
  m_state = STATE_START;
  m_pending.erase();
}

bool Tokenizer::stop()
{
  return (m_state != STATE_START) && (m_word.length() > 0);
}
