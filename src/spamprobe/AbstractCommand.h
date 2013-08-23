///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractCommand.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _AbstractCommand_h
#define _AbstractCommand_h

#include "util.h"

class ConfigManager;
class File;
class FrequencyDB;
class Message;
class SpamFilter;

class AbstractCommand
{
public:
  AbstractCommand(const string &name,
                  const string &short_help,
                  const string &long_help,
                  bool read_only)
    : m_name(name),
      m_shortHelp(short_help),
      m_longHelp(long_help),
      m_readOnly(read_only),
      m_isSingleMessageCommand(false)
  {
  }

  virtual ~AbstractCommand()
  {
  }

  const string &name() const
  {
    return m_name;
  }

  const string &shortHelp() const
  {
    return m_shortHelp;
  }

  const string &longHelp() const
  {
    return m_longHelp;
  }

  bool isReadOnly() const
  {
    return m_readOnly;
  }

  bool isSingleMessageCommand() const
  {
    return m_isSingleMessageCommand;
  }

  virtual bool isMessageSpam() const;

  virtual bool argsOK(const ConfigManager &config);

  virtual int execute(const ConfigManager &config,
                      SpamFilter &filter) = 0;

protected:
  void createDbDir(const ConfigManager &config);
  void openDatabase(const ConfigManager &config,
                    SpamFilter &filter);
  const char *scoreToString(double score);
  void printTerms(FrequencyDB *db,
                  ostream &out,
                  Message &msg,
                  const string &indent);
  void printMessageScore(const ConfigManager &config,
                         SpamFilter &filter,
                         bool is_spam,
                         double score,
                         Message &msg,
                         const File *source = 0);

  void setIsSingleMessageCommand(bool value)
  {
    m_isSingleMessageCommand = value;
  }

  void logMessageProcessing(bool processed,
                            Message &message,
                            const File *stream_file);

private:
  string m_name;
  string m_shortHelp;
  string m_longHelp;
  bool m_readOnly;
  bool m_isSingleMessageCommand;
};

#endif // _AbstractCommand_h
