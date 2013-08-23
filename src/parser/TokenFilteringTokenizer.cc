///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: TokenFilteringTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "AbstractTokenReceiver.h"
#include "TokenFilteringTokenizer.h"

static const char SEPARATOR = ' ';

TokenFilteringTokenizer::TokenFilteringTokenizer(AbstractTokenizer *tokenizer,
                                                 int min_length,
                                                 int max_length,
                                                 bool allow_numbers)
: m_tokenizer(tokenizer), m_minLength(min_length), m_maxLength(max_length), m_allowNumbers(allow_numbers)
{
}

TokenFilteringTokenizer::~TokenFilteringTokenizer()
{
}

void TokenFilteringTokenizer::receiveToken(const string &prefix,
                                           const string &token)
{
    if (token.length() < m_minLength) {
        return;
    }

    if (token.length() > m_maxLength) {
        return;
    }

    if (!m_allowNumbers && is_digits(token)) {
        return;
    }

    m_receiver->receiveToken(prefix, token);
}

void TokenFilteringTokenizer::tokenize(AbstractTokenReceiver *receiver,
                                       AbstractCharReader *reader,
                                       const string &prefix)
{
    m_receiver = receiver;
    m_tokenizer->tokenize(this, reader, prefix);
}
