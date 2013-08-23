///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractFileCommand.cc 272 2007-01-06 19:37:27Z brian $
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

#include <fstream>
#include "File.h"
#include "LockFile.h"
#include "ConfigManager.h"
#include "CommandConfig.h"
#include "SpamFilter.h"
#include "MailMessageReaderFactory.h"
#include "AbstractFileCommand.h"

bool AbstractFileCommand::argsOK(const ConfigManager &config)
{
  bool answer = true;
  const CommandConfig *cmd_config = config.commandConfig();
  const int num_args = cmd_config->numArgs();
  for (int i = 0; i < num_args; ++i) {
    File f(cmd_config->arg(i));
    if (!f.exists()) {
      answer = false;
      cerr << "error: " << f.getPath() << ": file not found" << endl;
    }
  }
  return answer;
}

int AbstractFileCommand::execute(const ConfigManager &config,
                                 SpamFilter &filter)
{
  if (needsDatabase()) {
    openDatabase(config, filter);
  }
  const CommandConfig *cmd_config = config.commandConfig();
  if (cmd_config->numArgs() == 0) {
    processFile(config, filter, 0);
  } else {
    for (int i = 0, limit = cmd_config->numArgs(); i < limit; ++i) {
      File f(cmd_config->arg(i));
      if (!MailMessageReaderFactory::isAcceptableFile(f)) {
        cerr << "error: skipped " << f.getPath() << ": not a supported file or directory" << endl;
      } else {
        LockFile lock(f);
        if (f.isFile()) {
          lock.lock(LockFD::SHARED_LOCK);
        }
        processFile(config, filter, &f);
      }
    }
  }
  return 0;
}
