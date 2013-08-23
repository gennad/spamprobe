///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: LineReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include <climits>
#include "AbstractCharReader.h"
#include "LineReader.h"

static const char CR = '\r';
static const char LF = '\n';

LineReader::LineReader(AbstractCharReader *reader)
: m_reader(reader),
  m_buffer(40960),
  m_needsStart(true),
  m_hasLine(false)
{
}

LineReader::~LineReader()
{
}

bool LineReader::forward()
{
  int remaining = INT_MAX;
  return forward(remaining);
}

bool LineReader::forwardChar(int &remaining)
{
  if (remaining <= 0) {
    return false;
  }

  --remaining;
  return m_reader->forward();
}

bool LineReader::forward(int &chars_remaining)
{
  if (m_needsStart) {
    forwardChar(chars_remaining);
    m_needsStart = false;
  }

  if (chars_remaining <= 0 || m_reader->atEnd()) {
    m_line.erase();
    m_hasLine = false;
    return false;
  }

  m_buffer.clear();
  while (chars_remaining >= 0 && m_reader->hasChar()) {
    char ch = m_reader->currentChar();
    forwardChar(chars_remaining);
    if (isLineTerminator(ch, chars_remaining)) {
      break;
    }
    m_buffer.append(ch);
    if (chars_remaining == 0) {
      break;
    }
  }

  m_line.assign(m_buffer.get(), m_buffer.get() + m_buffer.length());
  m_hasLine = true;
  return true;
}

bool LineReader::isLineTerminator(char ch,
                                  int &chars_remaining)
{
  if (ch == CR) {
    if (m_reader->hasChar() && m_reader->currentChar() == LF) {
      forwardChar(chars_remaining);
    }
    return true;
  }

  return ch == LF;
}

bool LineReader::hasLine()
{
  return m_hasLine;
}
