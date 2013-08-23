///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: TokenCollector.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _TokenCollector_h
#define _TokenCollector_h

#include <map>
#include <vector>
#include "util.h"
#include "AbstractTokenReceiver.h"

class TokenCollector : public AbstractTokenReceiver
{
public:
    virtual ~TokenCollector();

    virtual void receiveToken(const string &token);

    int tokenCount(const string &token)
    {
        MapType::const_iterator i = m_tokens.find(token);
        return (i == m_tokens.end()) ? 0 : i->second;
    }

    void clear()
    {
        m_tokens.clear();
    }

    bool hasToken(const string &token)
    {
        MapType::const_iterator i = m_tokens.find(token);
        return i != m_tokens.end();
    }

    typedef vector<string> ListType;

    void getTokens(ListType &tokens);

private:
    typedef map<string,int> MapType;
    MapType m_tokens;
};

#endif

