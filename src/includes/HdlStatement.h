///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlStatement.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlStatement_h
#define _HdlStatement_h

#include <vector>
#include "util.h"

class HdlToken;

class HdlStatement
{
public:
  HdlStatement(const Ref<HdlToken> &name,
               bool is_block);
  ~HdlStatement();

  bool isBlock() const
  {
    return m_isBlock;
  }

  const CRef<HdlToken> &name() const
  {
    return m_name;
  }

  int numArguments() const
  {
    return (int)m_arguments.size();
  }

  const CRef<HdlToken> &argument(int index) const
  {
    assert(index >= 0);
    assert(index < numArguments());
    return m_arguments[index];
  }

  void addArgument(const Ref<HdlToken> &arg)
  {
    assert(arg.isNotNull());
    m_arguments.push_back(arg);
  }

  int numChildren() const
  {
    return (int)m_children.size();
  }

  const CRef<HdlStatement> &child(int index) const
  {
    assert(index >= 0);
    assert(index < numChildren());
    return m_children[index];
  }

  const CRef<HdlStatement> child(const CRef<HdlToken> &token) const;
  const CRef<HdlStatement> child(const string &name) const;

  void addChild(const Ref<HdlStatement> &stmt)
  {
    assert(stmt.isNotNull());
    m_children.push_back(stmt);
  }

private:
  /// Not implemented.
  HdlStatement(const HdlStatement &);

  /// Not implemented.
  HdlStatement& operator=(const HdlStatement &);

private:
  typedef vector<Ref<HdlToken> > ArgVector;
  typedef vector<Ref<HdlStatement> > ChildVector;

  Ref<HdlToken> m_name;
  bool m_isBlock;
  ArgVector m_arguments;
  ChildVector m_children;
};

#endif // _HdlStatement_h
