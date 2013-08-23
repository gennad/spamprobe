///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: CommandConfig.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _CommandConfig_h
#define _CommandConfig_h

#include <vector>
#include "AbstractCommand.h"

class CommandConfig
{
public:
  CommandConfig();

  int messagesPerPurge() const
  {
    return m_messagesPerPurge;
  }

  void setMessagesPerPurge(int value)
  {
    m_messagesPerPurge = value;
  }

  bool shouldShowTerms() const
  {
    return m_shouldShowTerms;
  }

  void setShouldShowTerms(bool value)
  {
    m_shouldShowTerms = value;
  }

  bool streamIsPretokenized() const
  {
    return m_streamIsPretokenized;
  }

  void setStreamIsPretokenized(bool value)
  {
    m_streamIsPretokenized = value;
  }

  bool ignoreFrom() const
  {
    return m_ignoreFrom;
  }

  void setIgnoreFrom(bool value)
  {
    m_ignoreFrom = value;
  }

  bool ignoreContentLength() const
  {
    return m_ignoreContentLength;
  }

  void setIgnoreContentLength(bool value)
  {
    m_ignoreContentLength = value;
  }

  bool shouldCreateDbDir() const
  {
    return m_shouldCreateDbDir;
  }

  void setShouldCreateDbDir(bool value)
  {
    m_shouldCreateDbDir = value;
  }

  const string &statusFieldName() const
  {
    return m_statusFieldName;
  }

  void setStatusFieldName(const string &value)
  {
    m_statusFieldName = value;
  }

  int numCommands() const
  {
    return (int)m_commands.size();
  }

  const Ref<AbstractCommand> command(int index) const
  {
    assert(index >= 0);
    assert(index < numCommands());
    return m_commands[index];
  }

  const Ref<AbstractCommand> findCommand(const string &name) const;

  void addCommand(const Ref<AbstractCommand> &command)
  {
    m_commands.push_back(command);
  }

  void addArg(const string &arg)
  {
    m_args.push_back(arg);
  }

  void addArgs(char **args)
  {
    while (*args) {
      addArg(*args++);
    }
  }

  int numArgs() const
  {
    return (int)m_args.size();
  }

  const string &arg(int index) const
  {
    assert(index >= 0);
    assert(index < numArgs());
    return m_args[index];
  }

private:
  int m_messagesPerPurge;
  bool m_shouldShowTerms;
  bool m_streamIsPretokenized;
  bool m_ignoreFrom;
  bool m_ignoreContentLength;
  bool m_shouldCreateDbDir;
  string m_statusFieldName;
  vector<string> m_args;
  typedef vector<Ref<AbstractCommand> > CommandVector;
  CommandVector m_commands;
};

#endif // _CommandConfig_h
