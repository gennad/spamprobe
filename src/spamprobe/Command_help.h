///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_help.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Command_help_h
#define _Command_help_h

#include "AbstractCommand.h"

class Command_help : public AbstractCommand
{
public:
  Command_help();

  int execute(const ConfigManager &config,
              SpamFilter &filter);

private:
  void printCommandLineOptions(const CommandConfig *cmd_config);
  void printAllCommandsSummary(const CommandConfig *cmd_config);
  void printDetailedHelp(const CommandConfig *cmd_config);
  void printHelpForCommand(const Ref<AbstractCommand> &cmd);
};

#endif // _Command_help_h
