///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_edit_term.cc 272 2007-01-06 19:37:27Z brian $
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

#include "SpamFilter.h"
#include "FrequencyDB.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_edit_term.h"

Command_edit_term::Command_edit_term()
: AbstractCommand("edit-term",
                  "edit-term     Modifies counts for terms in database.",
                  "edit-term term good_count spam_count\n"
                  "    Can be used to specifically set the good and spam counts of a term.\n"
                  "    Whether this is truly useful is doubtful but it is provided for\n"
                  "    completeness sake.  For example it could be used to force a\n"
                  "    particular word to be very spammy or very good:\n"
                  "\n"
                  "        spamprobe edit-term nigeria 0 1000000\n"
                  "        spamprobe edit-term burton  10000000 0\n",
                  false)
{
}

void Command_edit_term::editTerm(const ConfigManager &config,
                                 SpamFilter &filter)
{
  const CommandConfig *cmd_config = config.commandConfig();
  const string &term = cmd_config->arg(0);
  int good_count = atoi(cmd_config->arg(1).c_str());
  int spam_count = atoi(cmd_config->arg(2).c_str());

  if (is_debug) {
    cerr << "edit-term '" << term << "' gc " << good_count << " sc " << spam_count << endl;
  }

  filter.getDB()->setWordCounts(term, good_count, spam_count);
}

bool Command_edit_term::argsOK(const ConfigManager &config)
{
  const CommandConfig *cmd_config = config.commandConfig();
  const int num_args = cmd_config->numArgs();
  return num_args == 3 && is_digits(cmd_config->arg(1)) && is_digits(cmd_config->arg(2));
}

int Command_edit_term::execute(const ConfigManager &config,
                             SpamFilter &filter)
{
  assert(argsOK(config));
  openDatabase(config, filter);
  editTerm(config, filter);
  return 0;
}
