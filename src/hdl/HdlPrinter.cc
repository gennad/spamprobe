///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlPrinter.cc 272 2007-01-06 19:37:27Z brian $
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

#include "HdlToken.h"
#include "HdlStatement.h"
#include "HdlPrinter.h"

HdlPrinter::HdlPrinter()
  : m_increment("    "),
    m_depth(0),
    m_atNewLine(true)
{
}

HdlPrinter::HdlPrinter(const string &indent,
                       const string &increment)
  : m_indent(indent),
    m_increment(increment),
    m_depth(0),
    m_atNewLine(true)
{
}

HdlPrinter::~HdlPrinter()
{
}

void HdlPrinter::beginBlock(ostream &out)
{
  printIndent(out);
  out << "begin ";
  m_depth += 1;
}

void HdlPrinter::endBlock(ostream &out)
{
  m_depth -= 1;
  printIndent(out);
  out << "end;\n";
  m_atNewLine = true;
}

void HdlPrinter::beginStatement(ostream &out,
                                const string &name)
{
  printIndent(out);
  out << name;
}

void HdlPrinter::endStatement(ostream &out)
{
  out << ";\n";
  m_atNewLine = true;
}

void HdlPrinter::addArgument(ostream &out,
                             const string &str)
{
  out << ' ' << str;
}

void HdlPrinter::addID(ostream &out,
                       const string &str)
{
  out << ' ' << str;
}

void HdlPrinter::addString(ostream &out,
                           const string &str)
{
  if (str.find('"') != string::npos) {
    out << " '" << str << '\'';
  } else {
    out << " \"" << str << '"';
  }
}

void HdlPrinter::addInteger(ostream &out,
                            int value)
{
  out << ' ' << value;
}

void HdlPrinter::addDouble(ostream &out,
                           double value)
{
  out << ' ' << value;
}

void HdlPrinter::reset()
{
  m_depth = 0;
  m_atNewLine = true;
}

void HdlPrinter::printToken(ostream &out,
                            const CRef<HdlToken> &token)
{
  if (token->isInteger()) {
    addInteger(out, token->intValue());
  } else if (token->isIdentifier()) {
    addID(out, token->strValue());
  } else {
    addString(out, token->strValue());
  }
}

void HdlPrinter::printStatement(ostream &out,
                                const CRef<HdlStatement> &stmt)
{
  if (stmt->isBlock()) {
    beginBlock(out);
  }

  beginStatement(out, stmt->name()->strValue());
  for (int i = 0; i < stmt->numArguments(); ++i) {
    printToken(out, stmt->argument(i));
  }
  endStatement(out);

  for (int i = 0; i < stmt->numChildren(); ++i) {
    printStatement(out, stmt->child(i));
  }

  if (stmt->isBlock()) {
    endBlock(out);
  }
}

void HdlPrinter::printIndent(ostream &out)
{
  if (m_atNewLine) {
    out << m_indent;
    for (int i = 0; i < m_depth; ++i) {
      out << m_increment;
    }
    m_atNewLine = false;
  }
}


void HdlPrinter::printBoolStatement(ostream &out,
                                    const string &name,
                                    bool value)
{
  beginStatement(out, name);
  addID(out, value ? "true" : "false");
  endStatement(out);
}

void HdlPrinter::printIDStatement(ostream &out,
                                  const string &name,
                                  const string &value)
{
  beginStatement(out, name);
  addID(out, value);
  endStatement(out);
}

void HdlPrinter::printStringStatement(ostream &out,
                                      const string &name,
                                      const string &value)
{
  beginStatement(out, name);
  addString(out, value);
  endStatement(out);
}

void HdlPrinter::printStringStatement(ostream &out,
                                      const string &name,
                                      char ch_value)
{
  string value;
  value += ch_value;
  printStringStatement(out, name, value);
}

void HdlPrinter::printIntStatement(ostream &out,
                                   const string &name,
                                   int value)
{
  beginStatement(out, name);
  addInteger(out, value);
  endStatement(out);
}

void HdlPrinter::printDoubleStatement(ostream &out,
                                      const string &name,
                                      double value)
{
  beginStatement(out, name);
  addDouble(out, value);
  endStatement(out);
}
