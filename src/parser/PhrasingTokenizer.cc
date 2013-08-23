///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: PhrasingTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "TempPtr.h"
#include "AbstractTokenReceiver.h"
#include "PhrasingTokenizer.h"

static const string SEPARATOR(" ");
static const string::size_type MAX_START(2048);
static const string::size_type RESERVE_LENGTH(3072);

PhrasingTokenizer::PhrasingTokenizer(AbstractTokenizer *tokenizer,
                                     int min_words,
                                     int max_words,
                                     int min_char_length,
                                     int max_char_length)
: m_tokenizer(tokenizer),
  m_minWords(min_words),
  m_maxWords(max_words),
  m_minCharLength(min_char_length),
  m_maxCharLength(max_char_length),
  m_receiver(0)
{
  m_maxWordsInList = (min_char_length <= 0) ? m_maxWords : min_char_length;
  m_chars.reserve(RESERVE_LENGTH);
}

PhrasingTokenizer::~PhrasingTokenizer()
{
}

string::size_type PhrasingTokenizer::charsStart()
{
  assert((m_chars.length() == 0) == (m_offsets.size() == 0));

  if (m_chars.length() == 0 || m_offsets.size() == 0) {
    return 0;
  } else {
    assert(m_offsets.back() <= m_offsets.front());
    assert(m_offsets.size() == 1 || m_offsets.front() > m_offsets.back());
    string::size_type answer = m_offsets.back();
    assert(answer < m_chars.length());
    return answer;
  }
}

void PhrasingTokenizer::compactChars()
{
  string::size_type start = charsStart();
  if (start > MAX_START) {
    for (deque<string::size_type>::iterator i = m_offsets.begin(), limit = m_offsets.end(); i != limit; ++i) {
      assert(*i >= start);
      *i -= start;
    }
    m_chars.erase(0, start);
  }
}

void PhrasingTokenizer::receiveToken(const string &prefix,
                                     const string &token)
{
  while (m_offsets.size() >= m_maxWordsInList) {
    m_offsets.pop_back();
  }
  compactChars();
  m_chars += SEPARATOR;
  m_offsets.push_front(m_chars.length());
  m_chars += token;

  int num_words = 1;
  for (deque<string::size_type>::const_iterator i = m_offsets.begin(), limit = m_offsets.end(); i != limit; ++i) {
    assert(*i < m_chars.length());
    const char *phrase = m_chars.c_str() + *i;
    string::size_type phrase_length = m_chars.length() - *i;
    if (m_maxCharLength > 0 && phrase_length > m_maxCharLength) {
      break;
    }
    if ((num_words > m_maxWords) && (m_minCharLength <= 0 || phrase_length > m_minCharLength)) {
      break;
    }
    if (num_words >= m_minWords) {
      m_receiver->receiveToken(prefix, phrase);
    }
    ++num_words;
  }
}

void PhrasingTokenizer::tokenize(AbstractTokenReceiver *receiver,
                                 AbstractCharReader *reader,
                                 const string &prefix)
{
  assert(!m_receiver);

  m_chars.erase();
  m_offsets.clear();
  TempPtr<AbstractTokenReceiver> tmp_receiver(m_receiver, receiver);
  m_tokenizer->tokenize(this, reader, prefix);
}
