///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlSyntaxChecker.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlSyntaxChecker_h
#define _HdlSyntaxChecker_h

#include "HdlToken.h"

class HdlArgumentConstraint;
class HdlStatement;
class HdlStatementConstraint;
class HdlStandardStatementConstraint;

class HdlSyntaxChecker
{
public:
  HdlSyntaxChecker(const CRef<HdlStatement> &syntax_tree);
  ~HdlSyntaxChecker();

  void checkSyntax(const CRef<HdlStatement> &stmt);

private:
  void initFromSyntaxTree(const CRef<HdlStatement> &syntax_tree);
  int parseArgs(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                const CRef<HdlStatement> &stmt,
                int index);
  void parseTable(const Ref<HdlArgumentConstraint> &table,
                  const CRef<HdlStatement> &stmt);
  void parseBlock(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                  const CRef<HdlStatement> &stmt);
  void parseStatement(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                      const CRef<HdlStatement> &stmt);
  void parseUnchecked(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                      const CRef<HdlStatement> &stmt);
  void verifyArgCount(const CRef<HdlStatement> &stmt,
                      int count);
  void verifyArgEquals(const CRef<HdlStatement> &stmt,
                       int index,
                       const CRef<HdlToken> &token);
  void verifyArgType(const CRef<HdlStatement> &stmt,
                     int index,
                     HdlToken::TokenType type);
  void verifyIsBlock(const CRef<HdlStatement> &stmt,
                     bool is_block);

private:
  /// Not implemented.
  HdlSyntaxChecker(const HdlSyntaxChecker &);

  /// Not implemented.
  HdlSyntaxChecker& operator=(const HdlSyntaxChecker &);

private:
  Ref<HdlStatementConstraint> m_constraint;
};

#endif // _HdlSyntaxChecker_h
