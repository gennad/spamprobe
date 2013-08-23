///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_purge_terms.cc 272 2007-01-06 19:37:27Z brian $
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

#include <iomanip>
#include "WordData.h"
#include "RegularExpression.h"
#include "SpamFilter.h"
#include "FrequencyDB.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_purge_terms.h"

Command_purge_terms::Command_purge_terms()
: AbstractCommand("purge-terms",
                  "purge-terms   Removes some or all terms from database.",
                  "purge-terms regex\n"
                  "    Similar to purge except that it removes from the database all terms\n"
                  "    which match the specified regular expression.  Be careful with this\n"
                  "    command because it could remove many more terms than you expect.\n"
                  "    Use dump with the same regex before running this command to see\n"
                  "    exactly what will be deleted.\n",
                  false)
{
}

void Command_purge_terms::purgeTerms(SpamFilter &filter,
                                     const string &regex_str)
{
  RegularExpression regex;
  regex.setExpression(regex_str);

  string word;
  WordData counts;
  bool again = filter.getDB()->firstWord(word, counts);
  while (again) {
    if (regex.match(word)) {
      filter.getDB()->removeWord(word, counts.goodCount(), counts.spamCount());
    }
    again = filter.getDB()->nextWord(word, counts);
  }
}

bool Command_purge_terms::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 1;
}

int Command_purge_terms::execute(const ConfigManager &config,
                                 SpamFilter &filter)
{
  assert(argsOK(config));
  openDatabase(config, filter);
  purgeTerms(filter, config.commandConfig()->arg(0));
  return 0;
}
