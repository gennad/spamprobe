///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: WordData.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _WordData_h
#define _WordData_h

#if defined(FORCE_NETWORK_ORDER)
  #include <netinet/in.h>
  #define to_database_order(x) htonl(x)
  #define to_host_order(x) ntohl(x)
#endif

#include <time.h>
#include "util.h"

class WordData
{
public:
  WordData()
    : m_goodCount(0),
      m_spamCount(0),
      m_flags(0)
  {
  }

  WordData(int _good_count,
           int _spam_count)
    : m_goodCount(_good_count),
      m_spamCount(_spam_count),
      m_flags(0)
  {
  }

  WordData(const WordData &other)
    : m_goodCount(other.m_goodCount),
      m_spamCount(other.m_spamCount),
      m_flags(other.m_flags)
  {
  }

  bool equals(const WordData &other)
  {
    return
      m_goodCount == other.m_goodCount &&
      m_spamCount == other.m_spamCount &&
      m_flags == other.m_flags;
  }

  bool hasSameCounts(const WordData &other)
  {
    return m_spamCount == other.m_spamCount && m_goodCount == other.m_goodCount;
  }

  int spamCount() const
  {
    return m_spamCount;
  }

  int goodCount() const
  {
    return m_goodCount;
  }

  int totalCount() const
  {
    return m_goodCount + m_spamCount;
  }

  void setFlags(unsigned long flags)
  {
    m_flags = flags;
  }

  unsigned long flags() const
  {
    return m_flags;
  }

  void adjustGoodCount(int delta)
  {
    m_goodCount += delta;
    if (m_goodCount < 0) {
      m_goodCount = 0;
    }
  }

  void adjustSpamCount(int delta)
  {
    m_spamCount += delta;
    if (m_spamCount < 0) {
      m_spamCount = 0;
    }
  }

  void clear()
  {
    m_goodCount = 0;
    m_spamCount = 0;
    m_flags = 0;
  }

  void reset(int good_count,
             int spam_count,
             int flags)
  {
    m_goodCount = good_count;
    m_spamCount = spam_count;
    m_flags = flags;
  }

  void adjustDate()
  {
    assert(s_today > 0);
    m_flags = (m_flags & ~TIME_STAMP_MASK) | s_today;
  }

  /// Returns day the record was last modified (days since epoch)
  int timeStamp() const
  {
    return (int)(m_flags & TIME_STAMP_MASK);
  }

  /**
     returns age of the record in days using the current s_today
     value as the base value
  */
  int age() const
  {
    assert(timeStamp() <= (int)s_today);
    return (int)s_today - timeStamp();
  }

  /**
     Initializes the current day number for use by adjust*Count().
     Must be called before either of them are called.
   */
  static void setTodayDate(time_t date_offset = 0)
  {
    const time_t base_time = 1029124800; // August 12, 2002
    const time_t seconds_per_day = 60 * 60 * 24;
    time_t now = time(0) - base_time;
    now -= date_offset * seconds_per_day;
    s_today = (unsigned long)(0xffff & (now / seconds_per_day));
    if (is_debug) {
      cerr << "WordData::setTodayDate: set date to " << s_today << endl;
    }
  }

  void toDatabaseOrder()
  {
    #if defined(FORCE_NETWORK_ORDER)
    m_goodCount = to_database_order(m_goodCount);
    m_spamCount = to_database_order(m_spamCount);
    m_flags = to_database_order(m_flags);
    #endif
  }

  void toHostOrder()
  {
    #if defined(FORCE_NETWORK_ORDER)
    m_goodCount = to_host_order(m_goodCount);
    m_spamCount = to_host_order(m_spamCount);
    m_flags = to_host_order(m_flags);
    #endif
  }

private:
  enum {
    // low 16 bits of flags hold days since epoch
    TIME_STAMP_MASK = 0xffff
  };

  int m_goodCount;
  int m_spamCount;
  unsigned long m_flags;

  static unsigned long s_today;
};

#endif // _WordData_h
