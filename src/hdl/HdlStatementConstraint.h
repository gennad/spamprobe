///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlStatementConstraint.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlStatementConstraint_h
#define _HdlStatementConstraint_h

#include "util.h"

class HdlStatementConstraint
{
public:
  HdlStatementConstraint(const string &name)
    : m_name(name)
  {
  }

  virtual ~HdlStatementConstraint()
  {
  }

  virtual void validateStatement(const CRef<HdlStatement> &stmt) const;

  const string &name() const
  {
    return m_name;
  }

private:
  /// Not implemented.
  HdlStatementConstraint(const HdlStatementConstraint &);

  /// Not implemented.
  HdlStatementConstraint& operator=(const HdlStatementConstraint &);

private:
  string m_name;
};

#endif // _HdlStatementConstraint_h
