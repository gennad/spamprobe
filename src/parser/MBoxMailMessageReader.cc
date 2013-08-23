///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MBoxMailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "RegularExpression.h"
#include "MBoxMailMessageReader.h"

static const string NEW_MESSAGE_REGEX("^From[ \t]+[^ \t]+[ \t]+[a-zA-Z][a-zA-Z][a-zA-Z][ \t]+[a-zA-Z][a-zA-Z][a-zA-Z][ \t]+[0-9][0-9]?[ \t]+[0-9][0-9][ \t]*:[ \t]*[0-9][0-9][ \t]*:[ \t]*[0-9][0-9][ \t]+[0-9][0-9][0-9][0-9]");

MBoxMailMessageReader::MBoxMailMessageReader(bool use_content_length)
: MailMessageReader(use_content_length), m_isPrevLineBlank(false), m_newMessage(NEW_MESSAGE_REGEX)
{
}

MBoxMailMessageReader::~MBoxMailMessageReader()
{
}

bool MBoxMailMessageReader::isNewMessageBoundary(const string &line)
{
    bool answer = m_isPrevLineBlank && m_newMessage.match(line);
    m_isPrevLineBlank = (line.length() == 0);
    return answer;
}
