///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlSyntaxChecker.cc 272 2007-01-06 19:37:27Z brian $
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

#include "HdlArgumentConstraint.h"
#include "HdlError.h"
#include "HdlToken.h"
#include "HdlStatement.h"
#include "HdlRecursiveStatementConstraint.h"
#include "HdlStandardStatementConstraint.h"
#include "HdlSyntaxChecker.h"

static void validate_token_name(const CRef<HdlStatement> &stmt,
                                const string &name)
{
  if (stmt->name()->strValue() != name) {
    throw HdlError(string("expected ") + name, stmt->name());
  }
}

HdlSyntaxChecker::HdlSyntaxChecker(const CRef<HdlStatement> &syntax_tree)
{
  initFromSyntaxTree(syntax_tree);
}

HdlSyntaxChecker::~HdlSyntaxChecker()
{
}

void HdlSyntaxChecker::checkSyntax(const CRef<HdlStatement> &stmt)
{
  m_constraint->validateStatement(stmt);
}

void HdlSyntaxChecker::initFromSyntaxTree(const CRef<HdlStatement> &stmt)
{
  validate_token_name(stmt, "language");

  verifyArgCount(stmt, 1);
  verifyArgType(stmt, 0, HdlToken::IDENTIFIER);

  Ref<HdlStandardStatementConstraint> syntax(new HdlStandardStatementConstraint(stmt->argument(0)->strValue(), true));
  for (int i = 0; i < stmt->numChildren(); ++i) {
    const CRef<HdlStatement> &child = stmt->child(i);
    string name = child->name()->strValue();
    if (name == "statement") {
      parseStatement(syntax, child);
    } else if (name == "block") {
      parseBlock(syntax, child);
    } else if (name == "unchecked") {
      parseUnchecked(syntax, child);
    } else {
      throw HdlError("expected unchecked, block, or statement", child->name());
    }
  }

  m_constraint = syntax;
}

int HdlSyntaxChecker::parseArgs(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                                const CRef<HdlStatement> &stmt,
                                int index)
{
  if (stmt->numChildren() < 1) {
    throw HdlError("expected args statement", stmt->name());
  }

  const CRef<HdlStatement> &args = stmt->child(0);
  validate_token_name(args, "args");

  if (args->numArguments() < 2) {
    throw HdlError("expected at least 2 arguments", args->name());
  }

  verifyArgType(args, 0, HdlToken::INTEGER);
  verifyArgType(args, 1, HdlToken::INTEGER);

  int min_args = args->argument(0)->intValue();
  int max_args = args->argument(1)->intValue();
  stmt_constraint->setArgCounts(min_args, max_args);

  int expected = min_args;
  if (max_args > min_args) {
    expected += 1;
  }

  if (args->numArguments() < (expected + 2)) {
    throw HdlError("not enough arguments to args statement", args->name());
  }

  vector<Ref<HdlArgumentConstraint> > tables;
  for (int i = 2; expected > 0; ++i) {
    const CRef<HdlToken> &arg = args->argument(i);
    Ref<HdlArgumentConstraint> arg_constraint(new HdlArgumentConstraint);
    if (arg->strValue() == "string") {
      arg_constraint->acceptString();
    } else if (arg->strValue() == "int") {
      arg_constraint->acceptInteger();
    } else if (arg->strValue() == "double") {
      arg_constraint->acceptDouble();
    } else if (arg->strValue() == "anyid") {
      arg_constraint->acceptID();
    } else if (arg->strValue() == "id") {
      i += 1;
      if (i >= args->numArguments()) {
        throw HdlError("identifier expected after id", args->name());
      }
      verifyArgType(args, i, HdlToken::IDENTIFIER);
      arg_constraint->acceptID(args->argument(i));
    } else if (arg->strValue() == "table") {
      tables.push_back(arg_constraint);
    } else {
      throw HdlError("expected string, int, anyid, id, or table", arg);
    }
    stmt_constraint->addArgument(arg_constraint);
    expected -= 1;
  }

  int expected_children = 1 + tables.size();
  if (stmt->numChildren() < expected_children) {
    throw HdlError("insufficient children for specified number of tables", stmt->name());
  }

  for (int i = 1; i < expected_children; ++i) {
    Ref<HdlArgumentConstraint> &table = tables[i - 1];
    const CRef<HdlStatement> &child = stmt->child(i);
    if (child->name()->strValue() != "table") {
      throw HdlError("expected table", child->name());
    }
    parseTable(table, child);
  }

  return expected_children;
}

void HdlSyntaxChecker::parseTable(const Ref<HdlArgumentConstraint> &table,
                                  const CRef<HdlStatement> &stmt)
{
  int id_index = 0;
  for (int i = 0; i < stmt->numArguments(); ++i) {
    const CRef<HdlToken> &token = stmt->argument(i);
    if (token->strValue() == "string") {
      table->acceptString();
    } else if (token->strValue() == "int") {
      table->acceptInteger();
    } else if (token->strValue() == "double") {
      table->acceptDouble();
    } else if (token->strValue() == "anyid") {
      table->acceptID();
    } else if (token->strValue() == "id") {
      i += 1;
      if (i >= stmt->numArguments()) {
        throw HdlError("expected identifier after id", stmt->name());
      }
      verifyArgType(stmt, i, HdlToken::IDENTIFIER);
      table->acceptID(stmt->argument(i));
    } else {
      throw HdlError("expected string, int, anyid, or id", token);
    }
  }
}

void HdlSyntaxChecker::parseBlock(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                                  const CRef<HdlStatement> &stmt)
{
  verifyIsBlock(stmt, true);
  if (stmt->numArguments() == 0) {
    throw HdlError("at least one argument expected", stmt->name());
  }
  verifyArgType(stmt, 0, HdlToken::IDENTIFIER);

  Ref<HdlStandardStatementConstraint> block(new HdlStandardStatementConstraint(stmt->argument(0)->strValue(), true));

  for (int i = 1; i < stmt->numArguments(); ++i) {
    const CRef<HdlToken> &arg = stmt->argument(i);
    if (arg->strValue() == "recursive") {
      block->addChild(Ref<HdlStatementConstraint>(new HdlRecursiveStatementConstraint(block.ptr())));
    } else {
      throw HdlError("unknown flag for block declaration", arg);
    }
  }

  for (int next_child = parseArgs(block, stmt, 0); next_child < stmt->numChildren(); ++next_child) {
    const CRef<HdlStatement> &child = stmt->child(next_child);
    string name = child->name()->strValue();
    if (name == "statement") {
      parseStatement(block, child);
    } else if (name == "block") {
      parseBlock(block, child);
    } else if (name == "unchecked") {
      parseUnchecked(block, child);
    } else {
      throw HdlError("expected unchecked, block, or statement", child->name());
    }
  }

  stmt_constraint->addChild(block);
}

void HdlSyntaxChecker::parseStatement(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                                      const CRef<HdlStatement> &stmt)
{
  verifyIsBlock(stmt, true);
  verifyArgCount(stmt, 1);
  verifyArgType(stmt, 0, HdlToken::IDENTIFIER);

  Ref<HdlStandardStatementConstraint> block(new HdlStandardStatementConstraint(stmt->argument(0)->strValue(), false));

  int next_child = parseArgs(block, stmt, 0);
  if (next_child != stmt->numChildren()) {
    throw HdlError("statement declarations can only contain arg and table statements", stmt->name());
  }

  stmt_constraint->addChild(block);
}

void HdlSyntaxChecker::parseUnchecked(const Ref<HdlStandardStatementConstraint> &stmt_constraint,
                                      const CRef<HdlStatement> &stmt)
{
  verifyIsBlock(stmt, false);
  verifyArgCount(stmt, 1);
  verifyArgType(stmt, 0, HdlToken::IDENTIFIER);
  stmt_constraint->addChild(Ref<HdlStatementConstraint>(new HdlStatementConstraint(stmt->argument(0)->strValue())));
}

void HdlSyntaxChecker::verifyArgCount(const CRef<HdlStatement> &stmt,
                                      int count)
{
  if (stmt->numArguments() != count) {
    throw HdlError(string("statement requires ") + num_to_string(count) + " arguments but has " + num_to_string(stmt->numArguments()),
                   stmt->name());
  }
}

void HdlSyntaxChecker::verifyArgEquals(const CRef<HdlStatement> &stmt,
                                       int index,
                                       const CRef<HdlToken> &token)
{
  if (!stmt->argument(index)->equals(token)) {
    throw HdlError(string("arg ") + num_to_string(index + 1) + " expected '" + token->strValue() + "'",
                   stmt->argument(index));
  }
}

void HdlSyntaxChecker::verifyArgType(const CRef<HdlStatement> &stmt,
                                     int index,
                                     HdlToken::TokenType type)
{
  if (stmt->argument(index)->tokenType() != type) {
    throw HdlError(string("arg ") + num_to_string(index + 1) + " expected '" + 
                   HdlToken::typeName(type) + " found " +
                   stmt->argument(index)->typeName(),
                   stmt->argument(index));
  }
}

void HdlSyntaxChecker::verifyIsBlock(const CRef<HdlStatement> &stmt,
                                     bool is_block)
{
  if (is_block != stmt->isBlock()) {
    throw HdlError(string("statement must") + (is_block ? " " : " not ") + "be a block", stmt->name());
  }
}
