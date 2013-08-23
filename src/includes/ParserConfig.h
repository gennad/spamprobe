///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: ParserConfig.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _ParserConfig_h
#define _ParserConfig_h

#include "HeaderPrefixList.h"

class ParserConfig
{
public:
  ParserConfig();
  ~ParserConfig();

  void setReplaceNonAsciiChars(int value)
  {
    if (value > 0) {
      m_replaceNonAsciiChars = true;
      m_nonAsciiChar = tolower((char)value);
    } else {
      m_replaceNonAsciiChars = false;
      m_nonAsciiChar = 0;
    }
  }

  bool getReplaceNonAsciiChars() const
  {
    return m_replaceNonAsciiChars;
  }

  char getNonAsciiCharReplacement() const
  {
    return m_nonAsciiChar;
  }

  string::size_type getMinTermLength() const
  {
    return m_minTermLength;
  }

  string::size_type getMaxTermLength() const
  {
    return m_maxTermLength;
  }

  bool getRemoveHTML() const
  {
    return m_removeHTML;
  }

  void setMinTermLength(string::size_type value)
  {
    m_minTermLength = value;
  }

  void setMaxTermLength(string::size_type value)
  {
    m_maxTermLength = value;
  }

  void setRemoveHTML(bool value)
  {
    m_removeHTML = value;
  }

  void setMinPhraseTerms(int length)
  {
    m_minPhraseTerms = length;
  }

  void setMaxPhraseTerms(int length)
  {
    m_maxPhraseTerms = length;
  }

  void setMinPhraseChars(int length)
  {
    m_minPhraseChars = length;
  }

  void setMaxPhraseChars(int length)
  {
    m_maxPhraseChars = length;
  }

  int getMinPhraseTerms() const
  {
    return m_minPhraseTerms;
  }

  int getMaxPhraseTerms() const
  {
    return m_maxPhraseTerms;
  }

  int getMinPhraseChars() const
  {
    return m_minPhraseChars;
  }

  int getMaxPhraseChars() const
  {
    return m_maxPhraseChars;
  }

  void setSpamProbeFieldName(const string &value)
  {
    m_spamprobeFieldName = value;
  }

  const string &spamprobeFieldName() const
  {
      return m_spamprobeFieldName;
  }

  HeaderPrefixList *headers()
  {
    return &m_prefixedHeaders;
  }

  void setKeepSuspiciousTags(bool value)
  {
    m_keepSuspiciousTags = value;
  }

  bool getKeepSuspiciousTags() const
  {
    return m_keepSuspiciousTags;
  }

  void setIgnoreBody(bool value)
  {
    m_ignoreBody = value;
  }

  bool getIgnoreBody() const
  {
    return m_ignoreBody;
  }

  void setMaxTermsPerMessage(int value)
  {
    m_maxTermsPerMessage = value;
  }

  int getMaxTermsPerMessage() const
  {
    return m_maxTermsPerMessage;
  }

private:
  string::size_type m_minTermLength;
  string::size_type m_maxTermLength;

  bool m_replaceNonAsciiChars;
  char m_nonAsciiChar;

  bool m_removeHTML;
  bool m_keepSuspiciousTags;
  bool m_ignoreBody;
  string m_spamprobeFieldName;
  HeaderPrefixList m_prefixedHeaders;

  int m_minPhraseTerms;
  int m_maxPhraseTerms;

  int m_minPhraseChars;
  int m_maxPhraseChars;

  int m_maxTermsPerMessage;
};

#endif // _ParserConfig_h
