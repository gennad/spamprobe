///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_exec.cc 272 2007-01-06 19:37:27Z brian $
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
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "Command_exec.h"

Command_exec::Command_exec(bool shared)
: AbstractCommand(shared ? "exec-shared" : "exec",
                  shared ? "exec-shared   Execute command with shared lock." :
                  "exec          Execute command with exclusive lock.",
                  "exec[-shared] command [args...]\n"
                  "    Obtains a lock on the database and then executes the\n"
                  "    command using system(3).  If multiple arguments are given after\n"
                  "    'exec' they are combined to form the command to be executed.  This\n"
                  "    command can be used when you want to perform some operation on the\n"
                  "    database without interference from incoming mail.  For example, to\n"
                  "    back up your .spamprobe directory using tar you could do something\n"
                  "    like this:\n"
                  "\n"
                  "        cd\n"
                  "        spamprobe exec tar cf spamprobe-data.tar.gz .spamprobe\n"
                  "\n"
                  "    If you simply want to hold the lock while interactively running\n"
                  "    commands in a different xterm you could use 'spamprobe exec read'.\n"
                  "    The linux read program simply reads a line of text from your\n"
                  "    terminal so the lock would effectively be held until you pressed\n"
                  "    the enter key.  Another option would be to use a shell as the\n"
                  "    command and type the commands into that shell:\n"
                  "\n"
                  "        spamprobe /bin/bash\n"
                  "        ls\n"
                  "        date\n"
                  "        exit\n"
                  "\n"
                  "    Be careful not to run spamprobe in the shell though since the\n"
                  "    spamprobe in the shell will wind up deadlocked waiting for the\n"
                  "    spamprobe running the exec command to release its lock.\n",
                  shared)
{
}

bool Command_exec::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args >= 1;
}

int Command_exec::execute(const ConfigManager &config,
                          SpamFilter &filter)
{
  openDatabase(config, filter);
  string command;
  const CommandConfig *cmd_config = config.commandConfig();
  for (int i = 0, limit = cmd_config->numArgs(); i < limit; ++i) {
    if (i > 0) {
      command += ' ';
    }
    command += cmd_config->arg(i);
  }
  if (is_debug) {
    cerr << "EXEC: " << command << endl;
  }
  int rc = system(command.c_str());
  if (is_debug) {
    cerr << "EXEC RC: " << rc << endl;
  }
  return 0;
}
