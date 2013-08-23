///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlPrinter.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlPrinter_h
#define _HdlPrinter_h

#include "util.h"

class HdlPrinter
{
public:
  HdlPrinter();
  HdlPrinter(const string &indent,
             const string &increment);
  ~HdlPrinter();

  void beginBlock(ostream &out);
  void endBlock(ostream &out);

  void beginStatement(ostream &out,
                      const string &name);
  void endStatement(ostream &out);

  void addArgument(ostream &out,
                   const string &str);

  void addID(ostream &out,
             const string &str);

  void addString(ostream &out,
                 const string &str);

  void addInteger(ostream &out,
                  int value);

  void addDouble(ostream &out,
                 double value);

  void reset();

  void printBoolStatement(ostream &out,
                          const string &stmt_id,
                          bool value);

  void printIntStatement(ostream &out,
                         const string &stmt_id,
                         int value);

  void printDoubleStatement(ostream &out,
                            const string &stmt_id,
                            double value);

  void printIDStatement(ostream &out,
                        const string &stmt_id,
                        const string &value);

  void printStringStatement(ostream &out,
                            const string &stmt_id,
                            const string &value);

  void printStringStatement(ostream &out,
                            const string &stmt_id,
                            char value);

  void printToken(ostream &out,
                  const CRef<HdlToken> &token);

  void printStatement(ostream &out,
                      const CRef<HdlStatement> &stmt);

private:
  void printIndent(ostream &out);

private:
  /// Not implemented.
  HdlPrinter(const HdlPrinter &);

  /// Not implemented.
  HdlPrinter& operator=(const HdlPrinter &);

private:
  string m_indent;
  string m_increment;
  int m_depth;
  bool m_atNewLine;
};

#endif // _HdlPrinter_h
