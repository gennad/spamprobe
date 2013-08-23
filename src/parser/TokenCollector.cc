///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: TokenCollector.cc 272 2007-01-06 19:37:27Z brian $
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

#include "TokenCollector.h"

TokenCollector::~TokenCollector()
{
}

void TokenCollector::receiveToken(const string &token)
{
    MapType::iterator i = m_tokens.find(token);
    if (i == m_tokens.end()) {
        m_tokens[token] = 1;
    } else {
        i->second += 1;
    }
}

void TokenCollector::getTokens(TokenCollector::ListType &tokens)
{
    for (MapType::const_iterator i = m_tokens.begin(); i != m_tokens.end(); ++i) {
        tokens.push_back(i->first);
    }
}
