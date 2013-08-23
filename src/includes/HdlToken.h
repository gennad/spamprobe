///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlToken.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlToken_h
#define _HdlToken_h

#include "util.h"

class HdlToken
{
public:
  typedef enum {
    STRING = 1,
    IDENTIFIER,
    INTEGER,
    DOUBLE
  } TokenType;

  static inline const Ref<HdlToken> intToken(int value)
  {
    return make_ref(new HdlToken(INTEGER, "", value, "", 0));
  }

  static inline const Ref<HdlToken> intToken(int value,
                                             const string &filename,
                                             int line)
  {
    return make_ref(new HdlToken(INTEGER, "", value, filename, line));
  }

  static inline const Ref<HdlToken> doubleToken(double value)
  {
    return make_ref(new HdlToken(DOUBLE, "", value, "", 0));
  }

  static inline const Ref<HdlToken> doubleToken(double value,
                                                const string &filename,
                                                int line)
  {
    return make_ref(new HdlToken(DOUBLE, "", value, filename, line));
  }

  static inline const Ref<HdlToken> idToken(const string &value)
  {
    return make_ref(new HdlToken(IDENTIFIER, value, 0, "", 0));
  }

  static inline const Ref<HdlToken> idToken(const string &value,
                                            const string &filename,
                                            int line)
  {
    return make_ref(new HdlToken(IDENTIFIER, value, 0, filename, line));
  }

  static inline const Ref<HdlToken> strToken(const string &value)
  {
    return make_ref(new HdlToken(STRING, value, 0, "", 0));
  }

  static inline const Ref<HdlToken> strToken(const string &value,
                                             const string &filename,
                                             int line)
  {
    return make_ref(new HdlToken(STRING, value, 0, filename, line));
  }

  TokenType tokenType() const
  {
    return m_type;
  }

  int intValue() const
  {
    return (int)m_double;
  }

  double doubleValue() const
  {
    return m_double;
  }

  const string &strValue() const
  {
    return m_string;
  }

  const string &filename() const
  {
    return m_filename;
  }

  int line() const
  {
    return m_line;
  }

  bool equals(const CRef<HdlToken> &other) const;
  bool operator==(const HdlToken &other) const;

  const string &asString(string &answer) const;

  static const string &typeName(TokenType type);

  const string &typeName() const;

  bool isDouble() const
  {
    return m_type == DOUBLE;
  }

  bool isInteger() const
  {
    return m_type == INTEGER;
  }

  bool isNumber() const
  {
    return isDouble() || isInteger();
  }

  bool isIdentifier() const
  {
    return m_type == IDENTIFIER;
  }

  bool isString() const
  {
    return m_type == STRING;
  }

  bool isBegin() const
  {
    return m_type == IDENTIFIER && m_string == "begin";
  }

  bool isEnd() const
  {
    return m_type == IDENTIFIER && m_string == "end";
  }

  bool isTrueValue() const
  {
    return (m_type == IDENTIFIER && m_string != "false") || (isNumber() && m_double != 0);;
  }

private:
  HdlToken(TokenType type,
           const string &str_value,
           double num_value,
           const string &filename,
           int line);

private:
  /// Not implemented.
  HdlToken(const HdlToken &);

  /// Not implemented.
  HdlToken& operator=(const HdlToken &);

private:
  TokenType m_type;
  string m_string;
  double m_double;
  string m_filename;
  int m_line;
};

#endif // _HdlToken_h
