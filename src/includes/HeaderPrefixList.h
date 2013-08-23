///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HeaderPrefixList.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HeaderPrefixList_h
#define _HeaderPrefixList_h

#include <map>
#include "util.h"

class HeaderPrefixList
{
private:
  struct HeaderData
  {
    string first_prefix;
    string other_prefix;
    int visit_count;
  };

  typedef map<string,HeaderData> MapType;
  typedef MapType::iterator IteratorType;

public:
  HeaderPrefixList();
  ~HeaderPrefixList();

  /// Resets the counts kept by shouldProcessHeader()
  void resetHeaderCounts();

  /// Determines if the header should be processed and what prefix to use.
  /// Remembers the number of times the header has been seen already.
  bool shouldProcessHeader(const string &name,
                           string &prefix);

  void removePrefixedHeaders();

  void setBlankPrefixesMode();
  void setDefaultHeadersMode();
  void setNonXHeadersMode();
  void setAllHeadersMode();
  void setNoHeadersMode();

  bool getBlankPrefixesMode()
  {
    return m_forceBlankPrefixes;
  }

  bool getNonXHeadersMode()
  {
    return m_visitAllHeaders && m_ignoreXHeaders;
  }

  bool getAllHeadersMode()
  {
    return m_visitAllHeaders;
  }

  bool getNoHeadersMode()
  {
    return m_visitAllHeaders == false && m_values.size() == 0;
  }

  void addSimpleHeaderPrefix(const string &name);

  void addHeaderPrefix(const string &name,
                       const string &first_prefix,
                       const string &other_prefix);

  void removeHeaderPrefix(const string &name);

  class const_iterator
  {
    friend class HeaderPrefixList;

  public:
    bool operator!=(const const_iterator &other)
    {
      return m_iter != other.m_iter;
    }

    const_iterator &operator++()
    {
      ++m_iter++;
      return *this;
    }

    const string &name()
    {
      return (*m_iter).first;
    }

    string firstPrefix()
    {
      return getPrefixBody((*m_iter).second.first_prefix);
    }

    string otherPrefix()
    {
      return getPrefixBody((*m_iter).second.other_prefix);
    }

  private:
    const_iterator(MapType::const_iterator iter) : m_iter(iter) {}
    string getPrefixBody(const string &prefix);

    MapType::const_iterator m_iter;
  };

  const_iterator begin()
  {
    return const_iterator(m_values.begin());
  }

  const_iterator end()
  {
    return const_iterator(m_values.end());
  }

private:
  /// Not implemented.
  HeaderPrefixList(const HeaderPrefixList &);

  /// Not implemented.
  HeaderPrefixList& operator=(const HeaderPrefixList &);

private:
  bool m_visitAllHeaders;
  bool m_ignoreXHeaders;
  bool m_forceBlankPrefixes;
  MapType m_values;
};

#endif // _HeaderPrefixList_h
