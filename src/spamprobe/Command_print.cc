///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id$
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
#include "Command_print.h"

Command_print::Command_print()
: AbstractCommand("print",
                  "print          Prints counts for specified terms in database.",
                  "print term...\n"
                  "    Prints the values of the specified terms in the word counts database\n"
                  "    one term per line in human readable format with spam probability, \n"
                  "    good count, spam count, flags, and word in columns separated by whitespace.\n",
                  true)
{
}

void Command_print::dumpWords(SpamFilter &filter,
                             const CommandConfig *command_config)
{
  int good_message_count, spam_message_count;
  filter.getDB()->getMessageCounts(good_message_count, spam_message_count);

  string word;
  WordData counts;
  for (int i = 0, limit = command_config->numArgs(); i < limit; ++i) {
    word = command_config->arg(i);
    if (filter.getDB()->readWord(word, counts)) {
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
  }
}

bool Command_print::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args > 0;
}

int Command_print::execute(const ConfigManager &config,
                          SpamFilter &filter)
{
  openDatabase(config, filter);
  dumpWords(filter, config.commandConfig());
  return 0;
}
