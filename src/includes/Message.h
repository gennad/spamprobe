///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Message.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Message_h
#define _Message_h

#include <vector>
#include <map>
#include "Token.h"
#include "MailMessage.h"

class Message
{
public:
  Message();
  ~Message();

  void clear();

  int getTokenCount() const
  {
    return m_tokensByName.size();
  }

  int getInOrderTokenCount() const
  {
    return m_tokensInOrder.size();
  }

  Token *getInOrderToken(int index) const;

  int getAllTokensCount() const;

  Token *getToken(int index) const;
  Token *getToken(const string &word) const;

  const string &getID(string &buffer) const
  {
    return getHeader("message-id", buffer);
  }

  void setDigest(const string &value)
  {
    m_digest = value;
  }

  string getDigest() const
  {
    return m_digest;
  }

  void addToken(const string &word,
                const string &prefix,
                int flags);
  void addToken(const string &word,
                int flags);

  bool isEmpty() const
  {
    return m_tokensByName.size() == 0;
  }

  void clearTopTokens()
  {
    m_topTokens.clear();
  }

  void addTopToken(Token *tok)
  {
    m_topTokens.push_back(tok);
  }

  int getTopTokenCount()
  {
    return m_topTokens.size();
  }

  Token *getTopToken(int index);

  void setSource(OWNED MailMessage *source)
  {
    m_source.set(source);
  }

  void setMaxTokenCount(int value)
  {
    m_maxTokenCount = value;
  }

  MailMessage *source()
  {
    return m_source.get();
  }

  const string &getHeader(const string &name, string &buffer) const;

private:
  /// Not implemented.
  Message(const Message &);

  /// Not implemented.
  Message& operator=(const Message &);

private:
  string m_id;
  string m_digest;
  vector<Token *> m_tokensByIndex;
  vector<Token *> m_tokensInOrder;
  map<string,Token *> m_tokensByName;
  vector<Token *> m_topTokens;
  Ptr<MailMessage> m_source;
  int m_maxTokenCount;
};

#endif // _Message_h
