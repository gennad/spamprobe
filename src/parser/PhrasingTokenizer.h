///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: PhrasingTokenizer.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _PhrasingTokenizer_h
#define _PhrasingTokenizer_h

#include <deque>
#include "AbstractTokenizer.h"

class PhrasingTokenizer : public AbstractTokenizer, AbstractTokenReceiver
{
public:
  PhrasingTokenizer(OWNED AbstractTokenizer *tokenizer,
                    int min_words,
                    int max_words,
                    int min_char_length,
                    int max_char_length);
  virtual ~PhrasingTokenizer();

  virtual void tokenize(AbstractTokenReceiver *receiver,
                        AbstractCharReader *reader,
                        const string &prefix);
  virtual void receiveToken(const string &prefix,
                            const string &token);

private:
  string::size_type charsStart();
  void compactChars();

private:
  Ptr<AbstractTokenizer> m_tokenizer;
  deque<string::size_type> m_offsets;
  string m_chars;
  int m_maxWordsInList;
  int m_minWords;
  int m_maxWords;
  int m_minCharLength;
  int m_maxCharLength;
  AbstractTokenReceiver *m_receiver;
};

#endif
