///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_remove.cc 272 2007-01-06 19:37:27Z brian $
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

#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_remove.h"

Command_remove::Command_remove()
  : AbstractMessageCommand("remove",
                          "remove        Removes messages from database",
                          "remove [filename...]\n"
                           "    Scans each file (or stdin if no file is specified) and removes its\n"
                           "    term counts from the database.  Messages which are not in the\n"
                           "    database (recognized using their MD5 digest) are ignored.\n",
                          false,
                          true)
{
}

void Command_remove::processMessage(const ConfigManager &config,
                                  SpamFilter &filter,
                                  const File *stream_file,
                                  Message &message,
                                  int message_number,
                                  bool &is_spam)
{
  filter.removeMessage(message);
  logMessageProcessing(true, message, stream_file);
}
