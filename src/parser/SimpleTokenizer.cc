///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: SimpleTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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
#include "AbstractCharReader.h"
#include "SimpleTokenizer.h"

SimpleTokenizer::SimpleTokenizer(char non_ascii_char)
  : m_nonAsciiCharReplacement(non_ascii_char)
{
    reset();
}

SimpleTokenizer::~SimpleTokenizer()
{
}

char SimpleTokenizer::currentChar(AbstractCharReader *reader)
{
  char ch = reader->currentChar();
  if (m_nonAsciiCharReplacement && (ch & 0x80)) {
    ch = m_nonAsciiCharReplacement;
  }
  return ch;
}

void SimpleTokenizer::tokenize(AbstractTokenReceiver *receiver,
                               AbstractCharReader *reader,
                               const string &prefix)
{
    reset();
    m_receiver = receiver;

    while (reader->forward()) {
        if (processCharForState(currentChar(reader))) {
            if (is_debug) {
                cerr << "SEND TOKEN: " << m_token << endl;
            }
            sendToken(prefix, m_token);
        }
    }

    if (m_token.length() > 0){
        sendToken(prefix, m_token);
    }
}

void SimpleTokenizer::reset()
{
    m_state = START;
    m_token.erase();
    m_pending.erase();
}

bool SimpleTokenizer::isLetterChar(char ch)
{
    return (ch & 0x80) || is_alnum(ch) || (ch == '%');
}

bool SimpleTokenizer::isSpecialChar(char ch)
{
    switch (ch) {
    case '.':
    case '-':
    case '+':
    case ',':
    case '_':
    case '$':
        return true;

    default:
        return false;
    }

}

void SimpleTokenizer::appendChar(string &s, char ch)
{
    s += to_lower(ch);
}

bool SimpleTokenizer::processStart(char ch)
{
    assert(m_state == START);

    m_token.erase();
    m_pending.erase();
    m_state = WAITING;
    return processWaiting(ch);
}

bool SimpleTokenizer::processWaiting(char ch)
{
    assert(m_state == WAITING);

    if (ch == '$' || isLetterChar(ch)) {
        appendChar(m_token, ch);
        m_state = IN_WORD;
    }

    return false;
}

bool SimpleTokenizer::processInWord(char ch)
{
    assert(m_state == IN_WORD);

    if (isLetterChar(ch)) {
        appendChar(m_token, ch);
        return false;
    }

    if (isSpecialChar(ch)) {
        m_state = PENDING;
        appendChar(m_pending, ch);
        return false;
    }

    m_state = START;
    return true;
}

bool SimpleTokenizer::processPending(char ch)
{
    if (isLetterChar(ch)) {
        m_state = IN_WORD;
        m_token += m_pending;
        appendChar(m_token, ch);
        m_pending.erase();
        return false;
    }

    if (isSpecialChar(ch)) {
        appendChar(m_pending, ch);
        return false;
    }

    m_state = START;
    m_pending.erase();
    return true;
}

bool SimpleTokenizer::processCharForState(char ch)
{
    //cerr << "PROCESS CHAR '" << ch << "' (" << (int)ch << ") STATE " << m_state
    //     << " TOKEN '" << m_token << "' PENDING '" << m_pending << "'" << endl;
    switch (m_state) {
    case START:
        return processStart(ch);

    case WAITING:
        return processWaiting(ch);

    case IN_WORD:
        return processInWord(ch);

    case PENDING:
        return processPending(ch);

    default:
        assert(false);
        cerr << "INVALID STATE " << m_state << endl;
        return false;
    }
}

void SimpleTokenizer::sendToken(const string &prefix,
                                string &token)
{
  m_receiver->receiveToken(prefix, m_token);
  m_token.erase();
}
