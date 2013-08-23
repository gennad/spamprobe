///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: StringReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "StringReader.h"

class StringReaderPosition : public AbstractCharReaderPosition
{
public:
    StringReaderPosition(int _index) : index(_index)
    {
    }

    virtual ~StringReaderPosition()
    {
    }

    int index;
};

StringReader::StringReader(const string &source)
    : m_source(source), m_index(-1)
{
}

StringReader::~StringReader()
{
}

bool StringReader::forward()
{
    return skip(1);
}

bool StringReader::hasChar()
{
    return m_index >= 0 && m_index < m_source.length();
}

bool StringReader::skip(int nchars)
{
    m_index += nchars;
    if (m_index >= m_source.length()) {
        return false;
    }
    setCurrentChar(m_source[m_index]);
    return m_index < m_source.length();
}

bool StringReader::atEnd()
{
    return m_index >= m_source.length();
}

OWNED AbstractCharReaderPosition *StringReader::createMark()
{
    return new StringReaderPosition(m_index);
}

void StringReader::returnToMark(AbstractCharReaderPosition *pos)
{
    m_index = dynamic_cast<StringReaderPosition*>(pos)->index;
}

