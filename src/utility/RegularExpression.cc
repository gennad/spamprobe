///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: RegularExpression.cc 272 2007-01-06 19:37:27Z brian $
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

#ifndef HAVE_REGEX_H
#error This module requires POSIX regular expressions
#endif

#include "RegularExpression.h"
#include <regex.h>

#define MAX_MATCHES 25

void dumpstr(const string &str)
{
  cout << "dump: len " << dec << str.length() << ": " << str << endl;
  for (string::size_type pos = 0; pos < str.length(); ++pos) {
    if (pos % 20 == 0 && pos != 0) {
      cout << endl;
    }
    cout << str[pos] << " " << hex << (unsigned)(str[pos] & 0xff) << " ";
  }
  cout << endl;
}

class RegularExpressionImpl
{
public:
  RegularExpressionImpl()
    : m_haveExpr(false)
  {
  }

  ~RegularExpressionImpl()
  {
    if (m_haveExpr) {
      regfree(&m_expr);
    }
  }

  void release()
  {
    if (m_haveExpr) {
      regfree(&m_expr);
      m_numSubs = 0;
      m_haveExpr = false;
    }
  }

  bool compile(const string &expr,
               int num_subs,
               bool ignore_case,
               bool ignore_newline)
  {
    assert(num_subs < MAX_MATCHES);

    release();

    int flags = REG_EXTENDED;
    if (ignore_case) {
      flags |= REG_ICASE;
    }
    if (ignore_newline) {
      flags |= REG_NEWLINE;
    }
    if (num_subs == 0) {
      flags |= REG_NOSUB;
    }
    if (regcomp(&m_expr, expr.c_str(), flags)) {
      return false;
    }

    m_numSubs = num_subs;
    m_haveExpr = true;
    return true;
  }

  bool match(const string &text,
             vector<RegularExpression::MatchData> &match_vec)
  {
    assert(m_haveExpr);

    match_vec.clear();

    regmatch_t matches[MAX_MATCHES + 1];
    int rc = regexec(&m_expr, to_7bits(text).c_str(), m_numSubs + 1, matches, 0);
    if (rc != 0) {
      return false;
    }

    for (int i = 0; i <= m_numSubs; ++i) {
      RegularExpression::MatchData md;
      if (matches[i].rm_so == -1) {
        md.start_pos = 0;
        md.end_pos = 0;
      } else {
        md.start_pos = matches[i].rm_so;
        md.end_pos = matches[i].rm_eo;
      }
      match_vec.push_back(md);
    }

    return true;
  }

private:
  bool m_haveExpr;
  int m_numSubs;
  regex_t m_expr;
};

RegularExpression::RegularExpression()
{
}

RegularExpression::RegularExpression(const string &expr,
                                     int num_subs,
                                     bool ignore_case,
                                     bool ignore_newline)
{
  setExpression(expr, num_subs, ignore_case, ignore_newline);
}


RegularExpression::~RegularExpression()
{
}

bool RegularExpression::isValid()
{
  return m_expr.get() != 0;
}

bool RegularExpression::setExpression(const string &expr,
                                      int num_subs,
                                      bool ignore_case,
                                      bool ignore_newline)
{
  Ptr<RegularExpressionImpl> new_expr(new RegularExpressionImpl);
  if (!new_expr->compile(expr, num_subs, ignore_case, ignore_newline)) {
    return false;
  }
  m_expr.set(new_expr.release());
  return true;
}

bool RegularExpression::match(const string &text)
{
  assert(isValid());

  m_text = text;
  return m_expr->match(m_text, m_matches);
}

int RegularExpression::matchCount() const
{
  return (int)m_matches.size();
}

void RegularExpression::removeMatch(int match_index,
                                    string &text)
{
  assert(match_index < matchCount());

  MatchData *match_data = &m_matches[match_index];
  text.erase(match_data->start_pos, match_data->end_pos - match_data->start_pos + 1);
}

void RegularExpression::replaceMatch(int match_index,
                                     string &text,
                                     const string &replace_with)
{
  assert(match_index < matchCount());

  MatchData *match_data = &m_matches[match_index];
  text.replace(match_data->start_pos, match_data->end_pos - match_data->start_pos + 1, replace_with);
}

void RegularExpression::getMatch(int match_index,
                                 MatchData &match_data)
{
  assert(match_index < matchCount());

  match_data = m_matches[match_index];
}

const string &RegularExpression::getMatch(int match_index,
                                          string &matched_string)
{
  assert(match_index < matchCount());

  MatchData &md(m_matches[match_index]);
  matched_string = m_text.substr(md.start_pos, md.end_pos - md.start_pos);
  return matched_string;
}

const string &RegularExpression::getMatch(int match_index,
                                          string &matched_string,
                                          const string &default_value)
{
  if (match_index < matchCount()) {
    MatchData &md(m_matches[match_index]);
    matched_string = m_text.substr(md.start_pos, md.end_pos - md.start_pos);
  } else {
    matched_string = default_value;
  }
  return matched_string;
}

ostream &RegularExpression::dumpMatches(ostream &out)
{
  string buffer;
  for (int i = 0; i < matchCount(); ++i) {
    out << "  " << i << ": " << getMatch(i, buffer) << endl;
  }
  return out;
}
