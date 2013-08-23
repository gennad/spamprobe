///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Token.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Token_h
#define _Token_h

#include "util.h"

class Token
{
public:
  Token(const string &word,
        int flags)
    : m_word(word),
      m_count(1),
      m_dbGoodCount(0),
      m_dbSpamCount(0),
      m_score(0.0),
      m_tieBreakCount(0),
      m_flags(flags)
  {
  }

  ~Token()
  {
  }

  const string &getWord() const
  {
    return m_word;
  }

  int getCount() const
  {
    return m_count;
  }

  void incrementCount(int delta = 1)
  {
    m_count += delta;
  }

  double getScore() const
  {
    return m_score;
  }

  void setScore(double value)
  {
    m_score = value;
  }

  double getDistanceFromMean() const
  {
    double diff = 0.5 - m_score;
    return (diff >= 0.0) ? diff : -diff;
  }

  int getDBTotalCount() const
  {
    return m_dbGoodCount + m_dbSpamCount;
  }

  int getDBGoodCount() const
  {
    return m_dbGoodCount;
  }

  void setDBGoodCount(int value)
  {
    m_dbGoodCount = value;
  }

  int getDBSpamCount() const
  {
    return m_dbSpamCount;
  }

  void setDBSpamCount(int value)
  {
    m_dbSpamCount = value;
  }

  void setSortCount(long value)
  {
    m_sortCount = value;
  }

  long getSortCount() const
  {
    return m_sortCount;
  }

  void setTieBreakCount(long value)
  {
    m_tieBreakCount = value;
  }

  long getTieBreakCount() const
  {
    return m_tieBreakCount;
  }

  enum {
    FLAG_NORMAL  = 1 << 0,
    FLAG_DERIVED = 1 << 1,
    FLAG_PHRASE  = 1 << 2,
    FLAG_ANY     = 0xff,
  };

  int getFlags() const
  {
    return m_flags;
  }

  void setFlags(int value)
  {
    m_flags = value;
  }

private:
  /// Not implemented.
  Token(const Token &);

  /// Not implemented.
  Token& operator=(const Token &);

private:
  string m_word;
  int m_count;
  int m_dbGoodCount;
  int m_dbSpamCount;
  double m_score;
  long m_tieBreakCount;
  long m_sortCount;
  int m_flags;
};

#endif // _Token_h
