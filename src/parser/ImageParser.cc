///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id$
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

#include <stdexcept>
#include "AbstractTokenizer.h"
#include "MD5Digester.h"
#include "Message.h"
#include "StringReader.h"
#include "ImageParser.h"

ImageParser::ImageParser(Message *message,
			 AbstractTokenizer *tokenizer,
			 AbstractTokenReceiver *receiver,
			 const string &prefix,
			 const Buffer<unsigned char> &bytes)
  : m_message(message),
    m_tokenizer(tokenizer),
    m_receiver(receiver),
    m_prefix(prefix),
    m_bytes(bytes)
{
}

ImageParser::~ImageParser()
{
}

void ImageParser::sendToken(const string &token)
{
  m_message->addToken(token, m_prefix, Token::FLAG_NORMAL);
}

void ImageParser::digestImage()
{
  MD5Digester digester;
  digester.start();
  digester.add(m_bytes.get(), m_bytes.length());
  digester.stop();
  sendToken(string("digest_") + digester.asString());
}
