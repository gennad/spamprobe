///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_auto_train.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Command_auto_train_h
#define _Command_auto_train_h

#include "AbstractCommand.h"

class Command_auto_train : public AbstractCommand
{
public:
  static const Ref<AbstractCommand> createAutoTrainCommand();
  static const Ref<AbstractCommand> createAutoLearnCommand();

  bool argsOK(const ConfigManager &config);
  int execute(const ConfigManager &config,
              SpamFilter &filter);

protected:
  Command_auto_train(const string &name,
                     const string &short_help,
                     const string &long_help,
                     bool is_learn)
    : AbstractCommand(name, short_help, long_help, false),
      m_isLearn(is_learn)
  {
  }

private:
  void logMessage(const ConfigManager &config,
                  SpamFilter &filter,
                  const string &type,
                  bool correct,
                  Message &message);

private:
  bool m_isLearn;
};

#endif // _Command_auto_train_h
