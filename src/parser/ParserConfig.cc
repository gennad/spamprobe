///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: ParserConfig.cc 272 2007-01-06 19:37:27Z brian $
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

#include "ParserConfig.h"
#include "RegularExpression.h"

ParserConfig::ParserConfig()
  : m_minTermLength(1),
    m_maxTermLength(90),
    m_replaceNonAsciiChars(true),
    m_nonAsciiChar('z'),
    m_removeHTML(true),
    m_keepSuspiciousTags(false),
    m_ignoreBody(false),
    m_minPhraseTerms(1),
    m_maxPhraseTerms(2),
    m_minPhraseChars(0),
    m_maxPhraseChars(0),
    m_maxTermsPerMessage(0),
    m_spamprobeFieldName("x-spamprobe")
{
#ifdef USE_8BIT
    setReplaceNonAsciiChars(0);
#endif
    headers()->addSimpleHeaderPrefix("from");
    headers()->addSimpleHeaderPrefix("to");
    headers()->addSimpleHeaderPrefix("cc");
    headers()->addSimpleHeaderPrefix("subject");
    headers()->addHeaderPrefix("received", "recv", "recvx");
}

ParserConfig::~ParserConfig()
{
}
