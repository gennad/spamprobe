///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: UrlOnlyHtmlTokenizer.cc 272 2007-01-06 19:37:27Z brian $
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

#include "UrlOnlyHtmlTokenizer.h"

static bool is_suspicious_tag(const string &tag)
{
  static RegularExpression suspicious_expr("^font|^img", 1, true, true);
  bool answer = suspicious_expr.match(tag);
  return answer;
}

UrlOnlyHtmlTokenizer::UrlOnlyHtmlTokenizer(AbstractTokenizer *textTokenizer,
                                           AbstractTokenizer *htmlTokenizer,
                                           int maxTagLength,
                                           bool keep_suspicious_tags)
  : HtmlTokenizer(textTokenizer, htmlTokenizer, maxTagLength),
    m_keepSuspiciousTags(keep_suspicious_tags)
{
}

UrlOnlyHtmlTokenizer::~UrlOnlyHtmlTokenizer()
{
}

void UrlOnlyHtmlTokenizer::processTagBody(const string &tag)
{
  if (m_keepSuspiciousTags && is_suspicious_tag(tag)) {
    HtmlTokenizer::processTagBody(tag);
  }
}
