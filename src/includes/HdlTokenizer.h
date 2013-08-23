///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HdlTokenizer.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HdlTokenizer_h
#define _HdlTokenizer_h

#include "util.h"

class AbstractCharReader;
class HdlToken;

class HdlTokenizer
{
public:
  HdlTokenizer(const string &filename,
               AbstractCharReader *reader);
  ~HdlTokenizer();

  bool nextToken();
  bool isSemiColon();
  const CRef<HdlToken> &token() const;
  const Ref<HdlToken> takeToken();

  bool hasToken() const
  {
    return m_haveToken;
  }

  const string &filename() const
  {
    return m_filename;
  }

  int lineNumber() const
  {
    return m_lineNumber;
  }

private:
  bool readNumber();
  bool readIdentifier();
  bool readString(char end_quote);
  bool onInvalidInterTokenCharacter();
  void skipToEOL();

private:
  /// Not implemented.
  HdlTokenizer(const HdlTokenizer &);

  /// Not implemented.
  HdlTokenizer& operator=(const HdlTokenizer &);

private:
  string m_filename;
  int m_lineNumber;
  bool m_haveToken;
  bool m_isSemiColon;
  Ref<HdlToken> m_token;
  AbstractCharReader *m_reader;
};

#endif // _HdlTokenizer_h
