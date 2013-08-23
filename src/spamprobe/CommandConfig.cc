///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: CommandConfig.cc 272 2007-01-06 19:37:27Z brian $
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

CommandConfig::CommandConfig()
  : m_messagesPerPurge(0),
    m_shouldShowTerms(false),
    m_streamIsPretokenized(false),
    m_ignoreFrom(false),
    m_ignoreContentLength(false),
    m_shouldCreateDbDir(false)
{
}

const Ref<AbstractCommand> CommandConfig::findCommand(const string &name) const
{
  for (CommandVector::const_iterator i = m_commands.begin(); i != m_commands.end(); ++i) {
    const Ref<AbstractCommand> &command = *i;
    if (command->name() == name) {
      return command;
    }
  }
  return Ref<AbstractCommand>();
}
