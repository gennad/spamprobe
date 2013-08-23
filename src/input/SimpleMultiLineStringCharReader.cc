///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: SimpleMultiLineStringCharReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "AbstractMultiLineString.h"
#include "SimpleMultiLineStringCharReader.h"

class SimpleMultiLineStringCharReaderPosition : public AbstractCharReaderPosition
{
public:
  SimpleMultiLineStringCharReaderPosition(const string *terminator,
                                          const AbstractMultiLineString *target)
    : m_terminator(terminator),
      m_target(target)
  {
    assert(m_terminator);
    assert(m_target);
    m_haveTerminator = m_terminator->size() > 0;
    reset();
  }

  ~SimpleMultiLineStringCharReaderPosition()
  {
  }

  void copyPosition(const SimpleMultiLineStringCharReaderPosition *other)
  {
    assert(other);
    m_terminator = other->m_terminator;
    m_target = other->m_target;
    m_lineIndex = other->m_lineIndex;
    m_charIndex = other->m_charIndex;
    m_inTerminator = other->m_inTerminator;
    m_haveTerminator = other->m_haveTerminator;
    m_atEnd = other->m_atEnd;
    m_currentChar = other->m_currentChar;
    m_currentLine = other->m_currentLine;
    m_currentLineLength = other->m_currentLineLength;
  }

  void reset()
  {
    m_lineIndex = 0;
    m_charIndex = -1;
    m_inTerminator = false;
    m_atEnd = m_lineIndex >= m_target->lineCount();
    m_currentChar = ' ';
    m_currentLine = 0;
    m_currentLineLength = 0;
  }

  bool skip(int nchars)
  {
    m_charIndex += nchars;
    return ensureCharReady();
  }

  const string *currentLine()
  {
    if (!m_currentLine) {
      m_currentLine = m_inTerminator ? m_terminator : &m_target->line(m_lineIndex);
      m_currentLineLength = (int)m_currentLine->size();
    }
    return m_currentLine;
  }

  bool hasChar()
  {
    return m_lineIndex >= 0 && !m_atEnd;
  }

  bool atEnd() const
  {
    return m_atEnd;
  }

  char currentChar() const
  {
    return m_currentChar;
  }

  bool ensureCharReady();

  int currentLineLength() const
  {
    assert(m_currentLine);
    return m_currentLineLength;
  }

private:
  const AbstractMultiLineString *m_target;
  const string *m_terminator;
  int m_lineIndex;
  int m_charIndex;
  bool m_inTerminator;
  bool m_haveTerminator;
  bool m_atEnd;
  char m_currentChar;
  const string *m_currentLine;
  int m_currentLineLength;
};

bool SimpleMultiLineStringCharReaderPosition::ensureCharReady()
{
  if (m_atEnd) {
    return false;
  }

  assert(m_lineIndex < m_target->lineCount());
  for (;;) {
    const string *line = currentLine();
    if (m_charIndex < m_currentLineLength) {
      m_currentChar = (*line)[m_charIndex];
      return true;
    }

    m_charIndex -= m_currentLineLength;
    m_currentLine = 0;
    if (m_inTerminator || !m_haveTerminator) {
      m_inTerminator = false;
      ++m_lineIndex;
      if (m_lineIndex >= m_target->lineCount()) {
        break;
      }
    } else {
      m_inTerminator = true;
    }
  }

  m_atEnd = true;
  return false;
}

SimpleMultiLineStringCharReader::SimpleMultiLineStringCharReader(const AbstractMultiLineString *target)
: m_target(target),
  m_position(new SimpleMultiLineStringCharReaderPosition(&m_terminator, target))
{
    reset();
}

SimpleMultiLineStringCharReader::SimpleMultiLineStringCharReader(const AbstractMultiLineString *target,
                                                                 const string &terminator)
: m_target(target),
  m_terminator(terminator),
  m_position(new SimpleMultiLineStringCharReaderPosition(&m_terminator, target))
{
    reset();
}

SimpleMultiLineStringCharReader::~SimpleMultiLineStringCharReader()
{
}

bool SimpleMultiLineStringCharReader::forward()
{
  return skip(1);
}

bool SimpleMultiLineStringCharReader::skip(int nchars)
{
  bool answer = m_position->skip(nchars);
  if (answer) {
    setCurrentChar(m_position->currentChar());
  }
  return answer;
}

bool SimpleMultiLineStringCharReader::hasChar()
{
  return m_position->hasChar();
}

bool SimpleMultiLineStringCharReader::atEnd()
{
    return m_position->atEnd();
}

void SimpleMultiLineStringCharReader::reset()
{
    m_position->reset();
}

AbstractCharReaderPosition *SimpleMultiLineStringCharReader::createMark()
{
    return new SimpleMultiLineStringCharReaderPosition(*m_position.get());
}

void SimpleMultiLineStringCharReader::returnToMark(AbstractCharReaderPosition *position)
{
    m_position->copyPosition(dynamic_cast<SimpleMultiLineStringCharReaderPosition *>(position));
    setCurrentChar(m_position->currentChar());
}

const string *SimpleMultiLineStringCharReader::getCurrentLine()
{
    return m_position->currentLine();
}

bool SimpleMultiLineStringCharReader::ensureCharReady()
{
  bool answer = m_position->ensureCharReady();
  if (answer) {
    setCurrentChar(m_position->currentChar());
  }
  return answer;
}
