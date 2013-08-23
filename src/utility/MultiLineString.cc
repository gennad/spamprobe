///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MultiLineString.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MultiLineString.h"

MultiLineString::MultiLineString()
{
}

MultiLineString::~MultiLineString()
{
}

int MultiLineString::lineCount() const
{
  return m_lines.size();
}

const string &MultiLineString::line(int index) const
{
  assert(index >= 0);
  assert(index < m_lines.size());
  return m_lines[index];
}

void MultiLineString::addLine(const string &line)
{
  m_lines.push_back(line);
}

void MultiLineString::appendToLastLine(const string &line)
{
  if (m_lines.size() == 0) {
    m_lines.push_back(line);
  } else {
    m_lines[m_lines.size() - 1] += line;
  }
}

void MultiLineString::parseText(const string &text)
{
  const char *start = text.c_str();
  for (const char *s = start; *s; ++s) {
    assert(start <= s);
    switch (*s) {
    case '\r':
      {
        string line(start, s);
        addLine(line);
        if (s[1] == '\n') {
          s += 1;
        }
        start = s + 1;
      }
      break;

    case '\n':
      {
        string line(start, s);
        addLine(line);
        start = s + 1;
      }
      break;

    default:
      break;
    }
  }
  assert(start <= (text.c_str() + text.length()));
  if (*start) {
    addLine(start);
  }
}

Ref<MultiLineString> MultiLineString::fromText(const string &text)
{
  Ref<MultiLineString> answer(new MultiLineString());
  answer->parseText(text);
  return answer;
}
