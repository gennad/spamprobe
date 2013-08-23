///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlStatement.cc 272 2007-01-06 19:37:27Z brian $
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

HdlStatement::HdlStatement(const Ref<HdlToken> &name,
                           bool is_block)
  : m_name(name),
    m_isBlock(is_block)
{
}

HdlStatement::~HdlStatement()
{
}

const CRef<HdlStatement> HdlStatement::child(const CRef<HdlToken> &token) const
{
  typedef ChildVector::const_iterator i_iter;
  for (i_iter i = m_children.begin(); i != m_children.end(); ++i) {
    const CRef<HdlStatement> &child = *i;
    const CRef<HdlToken> &child_token = child->name();
    if (*child_token == *token) {
      return child;
    }
  }

  return CRef<HdlStatement>();
}

const CRef<HdlStatement> HdlStatement::child(const string &name) const
{
  typedef ChildVector::const_iterator i_iter;
  for (i_iter i = m_children.begin(); i != m_children.end(); ++i) {
    const CRef<HdlStatement> &child = *i;
    if (child->name()->strValue() == name) {
      return child;
    }
  }

  return CRef<HdlStatement>();
}
