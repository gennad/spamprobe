///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_create_config.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_create_config.h"

Command_create_config::Command_create_config()
: AbstractCommand("create-config",
                  "create-config Creates a new config file if none currently exists.",
                  "create-config\n"
                  "    Writes a config file containing the current settings.  Any existing\n"
                  "    config file will be overwritten.\n",
                  false)
{
}

bool Command_create_config::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 0;
}

int Command_create_config::execute(const ConfigManager &config,
                                   SpamFilter &filter)
{
  config.writeToFile(config.configFile());
  return 0;
}
