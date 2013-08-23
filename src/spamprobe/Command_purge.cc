///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_purge.cc 272 2007-01-06 19:37:27Z brian $
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

#include "CleanupManager.h"
#include "SpamFilter.h"
#include "FrequencyDB.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_purge.h"

Command_purge::Command_purge()
: AbstractCommand("purge",
                  "purge         Removes some or all terms from database.",
                  "purge [min_count]\n"
                  "    Similar to cleanup but forces the immediate deletion of all terms\n"
                  "    with total count less than junk_count (default is 2) no matter how\n"
                  "    long it has been since they were modified (i.e. even if they were\n"
                  "    just added today). This could be handy immediately after\n"
                  "    classifying a large mailbox of historical spam or good email to\n"
                  "    make room for the next batch.\n",
                  false)
{
}

void Command_purge::purgeTerms(SpamFilter &filter,
                               int junk_count)
{
  CleanupManager purger(junk_count, -1); // -1 forces todays to be removed too
  filter.getDB()->sweepOutOldTerms(purger);
}

bool Command_purge::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 0 || (num_args == 1 && is_digits(config.commandConfig()->arg(0)));
}

int Command_purge::execute(const ConfigManager &config,
                           SpamFilter &filter)
{
  assert(argsOK(config));
  openDatabase(config, filter);
  int junk_count = config.commandConfig()->numArgs() == 0 ? 2 : atoi(config.commandConfig()->arg(0).c_str());
  purgeTerms(filter, junk_count);
  return 0;
}
