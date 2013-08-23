///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_counts.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_counts.h"

Command_counts::Command_counts()
: AbstractCommand("counts",
                  "counts        Prints number of good and spam in database.",
                  "counts\n"
                  "    Prints number of good and spam messages in database.\n",
                  true)
{
}

bool Command_counts::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 0;
}

int Command_counts::execute(const ConfigManager &config,
                            SpamFilter &filter)
{
  openDatabase(config, filter);
  int spam_count, good_count;
  filter.getDB()->getMessageCounts(good_count, spam_count);
  cout << "GOOD " << good_count << " SPAM " << spam_count << endl;
  return 0;
}
