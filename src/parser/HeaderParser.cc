///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HeaderParser.cc 272 2007-01-06 19:37:27Z brian $
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
#include "MultiLineString.h"
#include "MessageHeader.h"
#include "MessageHeaderList.h"
#include "HeaderParser.h"

Ref<MessageHeaderList> HeaderParser::parseHeader(const CRef<AbstractMultiLineString> &headerText)
{
    m_headerText = headerText;
    m_headerList = make_ref(new MessageHeaderList());
    m_currentName.erase();
    m_currentLines.clear();
    parse();
    return m_headerList;
}

bool HeaderParser::isContinuationLine(const string &line)
{
    return is_space(line[0]);
}

void HeaderParser::addHeader()
{
    if (m_currentName.length() > 0 && m_currentLines.isNotNull()) {
        m_headerList->addHeader(new MessageHeader(m_currentName, m_currentLines));
    }
    m_currentName.erase();
    m_currentLines.clear();
}

void HeaderParser::addLineToCurrent(const string &line)
{
    if (m_currentLines.isNotNull()) {
        m_currentLines->addLine(line);
    }
}

void HeaderParser::startHeader(const string &line)
{
    string::size_type colon_index = line.find(':');
    if (colon_index == string::npos) {
        return;
    }

    m_currentName.assign(line, 0, colon_index);
    m_currentLines = make_ref(new MultiLineString());
    m_currentLines->addLine(trim_copy(line, colon_index + 1));
}

void HeaderParser::parse()
{
    for (int i = 0; i < m_headerText->lineCount(); ++i) {
        const string &line(m_headerText->line(i));
        if (line.length() > 0) {
            if (isContinuationLine(line)) {
                addLineToCurrent(line);
            } else {
                addHeader();
                startHeader(line);
            }
        }
    }
    addHeader();
}
