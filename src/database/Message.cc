///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Message.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MessageHeaderList.h"
#include "Message.h"

typedef vector<Token *>::size_type vtokindex_t;
typedef vector<Token *>::const_iterator vtokiter_t;
typedef map<string,Token *>::const_iterator mtokiter_t;

enum { RESERVE_TOKENS_SIZE = 1000 };

Message::Message()
{
  m_tokensByIndex.reserve(RESERVE_TOKENS_SIZE);
  m_tokensInOrder.reserve(RESERVE_TOKENS_SIZE);
  m_topTokens.reserve(RESERVE_TOKENS_SIZE);
}

Message::~Message()
{
  clear();
}

void Message::clear()
{
  for (mtokiter_t i = m_tokensByName.begin(); i != m_tokensByName.end(); ++i) {
    delete i->second;
  }
  m_id.erase();
  m_tokensByIndex.clear();
  m_tokensByName.clear();
  m_tokensInOrder.clear();
}

int Message::getAllTokensCount() const
{
  int total = 0;
  for (vtokiter_t i = m_tokensByIndex.begin(); i != m_tokensByIndex.end(); ++i) {
    total += (*i)->getCount();
  }
  return total;
}

Token *Message::getToken(int index) const
{
  assert(index >= 0);
  assert((vtokindex_t)index < m_tokensByIndex.size());

  return m_tokensByIndex[index];
}

Token *Message::getInOrderToken(int index) const
{
  assert(index >= 0);
  assert((vtokindex_t)index < m_tokensInOrder.size());

  return m_tokensInOrder[index];
}

Token *Message::getToken(const string &word) const
{
  assert(m_tokensByName.size() == m_tokensByIndex.size());

  mtokiter_t i = m_tokensByName.find(word);
  return (i != m_tokensByName.end()) ? i->second : 0;
}

void Message::addToken(const string &word,
                       const string &prefix,
                       int flags)
{
  assert(word.length() > 0);

  if (prefix.length() > 0) {
    addToken(prefix + word, flags);
  } else {
    addToken(word, flags);
  }
}

void Message::addToken(const string &word,
                       int flags)
{
  assert(word.length() > 0);

  Token *tok = getToken(word);
  if (tok) {
    tok->incrementCount();
    m_tokensInOrder.push_back(tok);
  } else if (m_maxTokenCount <= 0 || m_tokensByName.size() < m_maxTokenCount) {
    tok = new Token(word, flags);
    m_tokensByIndex.push_back(tok);
    m_tokensByName.insert(make_pair(word, tok));
    m_tokensInOrder.push_back(tok);
  }
  if (is_debug) {
    cerr << "ADD TOKEN " << word << endl;
  }
}

Token *Message::getTopToken(int i)
{
  assert(i >= 0);
  assert(i < (int)m_topTokens.size());
  return m_topTokens[i];
}

const string &Message::getHeader(const string &name,
                                 string &buffer) const
{
  if (m_source.isNull()) {
    buffer = EMPTY_STRING;
    return buffer;
  }

  return m_source->head()->getHeaderString(name, EMPTY_STRING, buffer);
}
