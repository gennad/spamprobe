///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_dump.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_dump.h"

Command_dump::Command_dump()
: AbstractCommand("dump",
                  "dump          Prints some or all terms in database.",
                  "dump [regex]\n"
                  "    Prints the contents of the word counts database one word per line\n"
                  "    in human readable format with spam probability, good count, spam\n"
                  "    count, flags, and word in columns separated by whitespace.  PBL and\n"
                  "    Berkeley DB sort terms alphabetically.  The standard unix sort\n"
                  "    command can be used to sort the terms as desired.\n"
                  "\n"
                  "    Optionally you can specify a regular expression.  If specified\n"
                  "    SpamProbe will only dump terms matching the regular expression.\n",
                  true)
{
}

void Command_dump::dumpWords(SpamFilter &filter,
                             const string &regex_str)
{
  int good_message_count, spam_message_count;
  filter.getDB()->getMessageCounts(good_message_count, spam_message_count);

  const bool have_regex = regex_str.length() > 0;
  RegularExpression regex;
  if (have_regex) {
    regex.setExpression(regex_str);
  }

  string word;
  WordData counts;
  bool again = filter.getDB()->firstWord(word, counts);
  while (again) {
    if (!have_regex || regex.match(word)) {
      double score = filter.scoreTerm(counts.goodCount(), counts.spamCount(),
                                      good_message_count, spam_message_count);
      cout << scoreToString(score)
           << dec
           << setw(8) << counts.goodCount()
           << setw(8) << counts.spamCount()
           << "  0x" << setfill('0') << setw(8) << hex << counts.flags() << setfill(' ')
           << "  " << word
           << "\n";
    }
    again = filter.getDB()->nextWord(word, counts);
  }
}

bool Command_dump::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 0 || num_args == 1;
}

int Command_dump::execute(const ConfigManager &config,
                          SpamFilter &filter)
{
  openDatabase(config, filter);
  if (config.commandConfig()->numArgs() > 0) {
    dumpWords(filter, config.commandConfig()->arg(0));
  } else {
    dumpWords(filter, EMPTY_STRING);
  }
  return 0;
}
