///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: RegularExpression.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _RegularExpression_h
#define _RegularExpression_h

#include <vector>
#include "util.h"

class RegularExpressionImpl;

/**
   Simple shell class encapsulating a regex API.  Currently
   uses the POSIX regular expression functions.
 */
class RegularExpression
{
public:
  RegularExpression();
  explicit RegularExpression(const string &expr,
                             int num_subs = 1,
                             bool ignore_case = false,
                             bool ignore_newline = true);
  ~RegularExpression();

  bool isValid();
  bool setExpression(const string &expr,
                     int num_subs = 1,
                     bool ignore_case = false,
                     bool ignore_newline = true);

  bool match(const string &str);

  int matchCount() const;
  const string &getMatch(int match_index,
                         string &matched_string);
  const string &getMatch(int match_index,
                         string &matched_string,
                         const string &default_value);

  ostream &dumpMatches(ostream &out);

  struct MatchData {
    string::size_type start_pos;
    string::size_type end_pos;
  };
  void getMatch(int match_index,
                MatchData &match_data);
  void removeMatch(int match_index,
                   string &text);
  void replaceMatch(int match_index,
                    string &text,
                    const string &replace_with);

private:
  /// Not implemented.
  RegularExpression(const RegularExpression &);

  /// Not implemented.
  RegularExpression& operator=(const RegularExpression &);

private:
  friend class RegularExpressionImpl;

  Ptr<RegularExpressionImpl> m_expr;
  string m_text;
  vector<MatchData> m_matches;
};

#endif // _RegularExpression_h
