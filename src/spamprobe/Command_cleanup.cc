///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_cleanup.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_cleanup.h"

Command_cleanup::Command_cleanup()
: AbstractCommand("cleanup",
                  "cleanup       Removes some or all terms from database.",
                  "cleanup [min_count min_age]...\n"
                  "    Scans the database and removes all terms with junk_count or less\n"
                  "    (default 2) which have not had their counts modified in at least\n"
                  "    max_age days (default 7).  You can specify multiple count/age pairs\n"
                  "    on a single command line but must specify both a count and an age\n"
                  "    for all but the last count.  This should be run periodically to\n"
                  "    keep the database from growing endlessly.\n",
                  false)
{
}

void Command_cleanup::parseArgs(const ConfigManager &config,
                                CleanupManager &cleanman)
{
  if (config.commandConfig()->numArgs() == 0) {
    cleanman.addLimit(2, 7);
  }

  for (int i = 0, limit = config.commandConfig()->numArgs() - 1; i < limit; i += 2) {
    int junk_count = atoi(config.commandConfig()->arg(i).c_str());
    int max_age = atoi(config.commandConfig()->arg(i + 1).c_str());
    cleanman.addLimit(junk_count, max_age);
  }
}

void Command_cleanup::cleanup(SpamFilter &filter,
                              CleanupManager &cleanman)
{
  filter.getDB()->sweepOutOldTerms(cleanman);
}

bool Command_cleanup::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();

  if (num_args % 2 != 0) {
    return false;
  }

  for (int i = 0; i < num_args; ++i) {
    if (!is_digits(config.commandConfig()->arg(i))) {
      return false;
    }
  }
  return true;
}

int Command_cleanup::execute(const ConfigManager &config,
                             SpamFilter &filter)
{
  assert(argsOK(config));
  openDatabase(config, filter);
  CleanupManager cleanman;
  parseArgs(config, cleanman);
  cleanup(filter, cleanman);
  return 0;
}
