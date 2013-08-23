///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlStandardStatementConstraint.cc 272 2007-01-06 19:37:27Z brian $
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

#include "HdlError.h"
#include "HdlToken.h"
#include "HdlStatement.h"
#include "HdlArgumentConstraint.h"
#include "HdlStandardStatementConstraint.h"

void HdlStandardStatementConstraint::validateStatement(const HdlStatement *stmt) const
{
  if (stmt->isBlock() != m_isBlock) {
    throw HdlError(m_isBlock ? "block expected" : "statement expected", stmt->name());
  }

  if (stmt->numArguments() < m_minArgCount) {
    string msg("expected at least ");
    msg += num_to_string(m_minArgCount);
    msg += " arguments but found ";
    msg += num_to_string(stmt->numArguments());
    throw HdlError(msg, stmt->name());
  }

  if (stmt->numArguments() > m_maxArgCount) {
    string msg("expected at most ");
    msg += num_to_string(m_maxArgCount);
    msg += " arguments but found ";
    msg += num_to_string(stmt->numArguments());
    throw HdlError(msg, stmt->name());
  }

  int i = 0;
  for (; i < m_minArgCount; ++i) {
    const CRef<HdlArgumentConstraint> &constraint = m_arguments[i];
    constraint->validateArgument(stmt->argument(i));
  }

  if (stmt->numArguments() > m_minArgCount) {
    if (m_minArgCount > 0 && m_minArgCount >= m_arguments.size()) {
      cerr << "m_minArgCount: " << m_minArgCount << endl;
      cerr << "num_args " << m_arguments.size() << endl;
      throw HdlError("internal error: insufficient argument definitions", stmt->name());
    }

    for (; i < stmt->numArguments(); ++i) {
      const CRef<HdlArgumentConstraint> &constraint = m_arguments[m_minArgCount];
      constraint->validateArgument(stmt->argument(i));
    }
  }

  if (m_isBlock) {
    for (i = 0; i < stmt->numChildren(); ++i) {
      const CRef<HdlStatement> &child = stmt->child(i);
      const CRef<HdlStatementConstraint> &constraint = childConstraint(child->name()->strValue());
      if (constraint.isNotNull()) {
        constraint->validateStatement(child);
      } else {
        throwInvalidStatement(child->name());
      }
    }
  }
}

void HdlStandardStatementConstraint::throwInvalidStatement(const CRef<HdlToken> &token) const
{
  string msg("invalid statement, expected");
  for (ChildVector::const_iterator i = m_children.begin(); i != m_children.end(); ++i) {
    const CRef<HdlStatementConstraint> &constraint = *i;
    msg += " '";
    msg += constraint->name();
    msg += "'";
  }
  msg += " but found '";
  msg += token->strValue();
  msg += "'";
  throw HdlError(msg, token);
}

const CRef<HdlStatementConstraint> HdlStandardStatementConstraint::childConstraint(const string &id) const
{
  for (ChildVector::const_iterator i = m_children.begin(); i != m_children.end(); ++i) {
    const CRef<HdlStatementConstraint> &constraint = *i;
    if (constraint->name() == id) {
      return constraint;
    }
  }
  return CRef<HdlStatementConstraint>();
}
