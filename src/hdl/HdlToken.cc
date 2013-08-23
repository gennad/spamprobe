///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlToken.cc 272 2007-01-06 19:37:27Z brian $
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
#include "HdlToken.h"

static string IDENTIFIER_NAME("identifier");
static string INTEGER_NAME("integer");
static string DOUBLE_NAME("double");
static string STRING_NAME("string");

HdlToken::HdlToken(TokenType type,
                   const string &str_value,
                   double num_value,
                   const string &filename,
                   int line)
  : m_type(type),
    m_string(str_value),
    m_double(num_value),
    m_filename(filename),
    m_line(line)
{
}


bool HdlToken::equals(const CRef<HdlToken> &other) const
{
  assert(other.isNotNull());

  if (m_type != other->m_type) {
    return false;
  }

  switch (m_type) {
  case IDENTIFIER:
  case STRING:
    return m_string == other->m_string;

  case DOUBLE:
  case INTEGER:
    return m_double == other->m_double;

  default:
    throw runtime_error("invalid token type encountered");
  }
}

bool HdlToken::operator==(const HdlToken &other) const
{
  if (m_type != other.m_type) {
    return false;
  }

  switch (m_type) {
  case IDENTIFIER:
  case STRING:
    return m_string == other.m_string;

  case DOUBLE:
  case INTEGER:
    return m_double == other.m_double;

  default:
    throw runtime_error("invalid token type encountered");
  }
}

const string &HdlToken::asString(string &answer) const
{
  switch (m_type) {
  case IDENTIFIER:
    answer += m_string;
    break;

  case STRING:
    answer += '"';
    answer += m_string;
    answer += '"';
    break;

  case DOUBLE:
  case INTEGER:
    answer += num_to_string(m_double);
    break;

  default:
    throw runtime_error("invalid token type encountered");
  }

  return answer;
}

const string &HdlToken::typeName(TokenType type)
{
  switch (type) {
  case IDENTIFIER:
    return IDENTIFIER_NAME;

  case STRING:
    return STRING_NAME;

  case INTEGER:
    return INTEGER_NAME;

  case DOUBLE:
    return DOUBLE_NAME;

  default:
    throw runtime_error("invalid token type encountered");
  }
}

const string &HdlToken::typeName() const
{
  return typeName(m_type);
}
