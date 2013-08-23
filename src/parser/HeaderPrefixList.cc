///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HeaderPrefixList.cc 272 2007-01-06 19:37:27Z brian $
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

#include "HeaderPrefixList.h"

static const char *SKIPPED_HEADERS[] = {
  "x-spam",
  "x-razor",
  "status",
  "x-status",
  "x-imap",
  0
};

const string PREFIX_START("H");
const string PREFIX_END("_");

static bool is_skipped_header(const string &line)
{
  for (const char **s = SKIPPED_HEADERS; *s; ++s) {
    if (starts_with(line, *s)) {
      return true;
    }
  }
  return false;
}

string HeaderPrefixList::const_iterator::getPrefixBody(const string &prefix)
{
  return prefix.substr(PREFIX_START.length(), prefix.length() - PREFIX_START.length() - PREFIX_END.length());
}

HeaderPrefixList::HeaderPrefixList()
  : m_visitAllHeaders(false),
    m_ignoreXHeaders(false),
    m_forceBlankPrefixes(false)
{
}

HeaderPrefixList::~HeaderPrefixList()
{
}

void HeaderPrefixList::resetHeaderCounts()
{
  for (IteratorType i = m_values.begin(); i != m_values.end(); ++i) {
    i->second.visit_count = 0;
  }
}

bool HeaderPrefixList::shouldProcessHeader(const string &name,
                                           string &prefix)
{
  IteratorType i = m_values.find(name);
  if (i != m_values.end()) {
    HeaderData &hd(i->second);
    hd.visit_count += 1;
    if (m_forceBlankPrefixes) {
      prefix.erase();
    } else if (hd.visit_count > 1) {
      prefix = hd.other_prefix;
    } else {
      prefix = hd.first_prefix;
    }
    return true;
  }

  if (!m_visitAllHeaders) {
    return false;
  }

  if (m_ignoreXHeaders && starts_with(name, "x-")) {
    return false;
  }

  if (is_skipped_header(name)) {
    return false;
  }

  if (m_forceBlankPrefixes) {
    prefix.erase();
  } else {
    prefix = PREFIX_START + name + PREFIX_END;
  }
  addHeaderPrefix(name, name, name);
  return true;
}

void HeaderPrefixList::setBlankPrefixesMode()
{
  m_forceBlankPrefixes = true;
}

void HeaderPrefixList::setDefaultHeadersMode()
{
  m_visitAllHeaders = false;
  m_ignoreXHeaders = false;
  m_forceBlankPrefixes = false;
  m_values.clear();

  addHeaderPrefix("reply-to", "reply-to", "reply-to");
  addHeaderPrefix("sender", "sender", "sender");
  addHeaderPrefix("originator", "originator", "originator");
  addHeaderPrefix("subject", "subject", "subject");
  addHeaderPrefix("from", "from", "from");
  addHeaderPrefix("to", "to", "to");
  addHeaderPrefix("cc", "cc", "cc");
  addHeaderPrefix("message-id", "message-id", "message-id");
  addHeaderPrefix("received", "recv", "recvx");
  addHeaderPrefix("content-type", "", "");
}

void HeaderPrefixList::setNonXHeadersMode()
{
  m_visitAllHeaders = true;
  m_ignoreXHeaders = true;
  m_forceBlankPrefixes = false;
  m_values.clear();
}

void HeaderPrefixList::setAllHeadersMode()
{
  m_visitAllHeaders = true;
  m_ignoreXHeaders = false;
  m_forceBlankPrefixes = false;
  m_values.clear();
}

void HeaderPrefixList::setNoHeadersMode()
{
  m_visitAllHeaders = false;
  m_ignoreXHeaders = false;
  m_forceBlankPrefixes = false;
  m_values.clear();
}

void HeaderPrefixList::addSimpleHeaderPrefix(const string &name)
{
  addHeaderPrefix(name, name, name);
}

void HeaderPrefixList::addHeaderPrefix(const string &name,
                                       const string &first_prefix,
                                       const string &other_prefix)
{
  HeaderData hd;
  if (first_prefix.length() > 0) {
    hd.first_prefix = PREFIX_START + to_lower(first_prefix) + PREFIX_END;
  }
  if (other_prefix.length() > 0) {
    hd.other_prefix = PREFIX_START + to_lower(other_prefix) + PREFIX_END;
  }
  hd.visit_count = 0;
  m_values.insert(make_pair(to_lower(name), hd));
}

void HeaderPrefixList::removeHeaderPrefix(const string &name)
{
  string lower_name(to_lower(name));
  MapType::iterator i = m_values.find(lower_name);
  if (i != m_values.end()) {
    m_values.erase(i);
  }
}
