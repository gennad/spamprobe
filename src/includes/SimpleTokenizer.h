///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: SimpleTokenizer.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _SimpleTokenizer_h
#define _SimpleTokenizer_h

#include "AbstractTokenizer.h"

class SimpleTokenizer : public AbstractTokenizer
{
public:
  explicit SimpleTokenizer(char non_ascii_char);
  virtual ~SimpleTokenizer();

  virtual void tokenize(AbstractTokenReceiver *receiver,
                        AbstractCharReader *reader,
                        const string &prefix);

private:
  void reset();
  bool isLetterChar(char ch);
  bool isSpecialChar(char ch);
  char currentChar(AbstractCharReader *reader);
  void appendChar(string &s, char ch);
  bool processStart(char ch);
  bool processWaiting(char ch);
  bool processInWord(char ch);
  bool processPending(char ch);
  bool processCharForState(char ch);
  void sendToken(const string &prefix,
                 string &token);

private:
  AbstractTokenReceiver *m_receiver;
  enum {
    START,
    WAITING,
    IN_WORD,
    PENDING
  } m_state;
  string m_token;
  string m_pending;
  char m_nonAsciiCharReplacement;
};

#endif
