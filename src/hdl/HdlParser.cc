///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlParser.cc 272 2007-01-06 19:37:27Z brian $
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

#include <fstream>
#include "IstreamCharReader.h"
#include "StringReader.h"
#include "HdlError.h"
#include "HdlToken.h"
#include "HdlTokenizer.h"
#include "HdlStatement.h"
#include "HdlParser.h"

const Ref<HdlStatement> HdlParser::parseString(const string &src)
{
  StringReader reader(src);
  m_tokenizer = make_ref(new HdlTokenizer("<string>", &reader));
  return parse();
}

const Ref<HdlStatement> HdlParser::parseFile(const File &file)
{
  ifstream fin(file.getPath().c_str());
  IstreamCharReader reader(&fin);
  m_tokenizer = make_ref(new HdlTokenizer(file.getPath(), &reader));
  return parse();
}

const Ref<HdlStatement> HdlParser::parse()
{
  Ref<HdlStatement> stmt;

  Ref<HdlToken> id(skipAndEnsureIdentifier());

  if (id->isBegin()) {
    stmt = parseBlock();
  } else {
    stmt = parseStatement(id, false);
  }

  m_tokenizer.clear();
  return stmt;
}

const Ref<HdlStatement> HdlParser::parseBlock()
{
  Ref<HdlToken> id(skipAndEnsureIdentifier());
  Ref<HdlStatement> stmt(parseStatement(id, true));

  for (;;) {
    id = skipAndEnsureIdentifier();
    if (id->isBegin()) {
      Ref<HdlStatement> child(parseBlock());
      stmt->addChild(child);
    } else if (id->isEnd()) {
      skipAndEnsureSemiColon();
      return stmt;
    } else {
      Ref<HdlStatement> child(parseStatement(id, false));
      stmt->addChild(child);
    }
  }
}

void HdlParser::skipAndEnsureSemiColon()
{
  if (!m_tokenizer->nextToken()) {
    throw HdlError("unexpected eof", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
  }
  if (!m_tokenizer->isSemiColon()) {
    throw HdlError("expected semi-colon", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
  }
}

const Ref<HdlToken> HdlParser::skipAndEnsureIdentifier()
{
  if (!m_tokenizer->nextToken()) {
    throw HdlError("unexpected eof", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
  }

  if (!m_tokenizer->hasToken() || m_tokenizer->token().isNull() || !m_tokenizer->token()->isIdentifier()) {
    throw HdlError("expected identifier", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
  }

  return m_tokenizer->takeToken();
}

const Ref<HdlStatement> HdlParser::parseStatement(const Ref<HdlToken> &id,
                                                  bool is_block)
{
  if (id->isBegin() || id->isEnd()) {
    throw HdlError(string("invalid statement name: ") + id->strValue(), m_tokenizer->filename(), m_tokenizer->lineNumber());
  }

  Ref<HdlStatement> stmt(new HdlStatement(id, is_block));
  parseArgumentsForStatement(stmt);
  return stmt;
}

void HdlParser::parseArgumentsForStatement(const Ref<HdlStatement> &stmt)
{
  for (;;) {
    if (!m_tokenizer->nextToken()) {
      throw HdlError("unexpected eof", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
    }

    if (m_tokenizer->isSemiColon()) {
      break;
    }

    if (!m_tokenizer->hasToken()) {
      // this probably can't happen but just being safe
      throw HdlError("token expected", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
    }

    Ref<HdlToken> id(m_tokenizer->takeToken());
    if (id->isBegin() || id->isEnd()) {
      throw HdlError("invalid use of begin/end", m_tokenizer->filename(), m_tokenizer->lineNumber()); 
    }

    stmt->addArgument(id);
  }
}
