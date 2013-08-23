///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractMessageCommand.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _AbstractMessageCommand_h
#define _AbstractMessageCommand_h

#include "AbstractFileCommand.h"

class AutoPurger;
class Message;

class AbstractMailMessageReader;
class MailMessageReaderFactory;

class AbstractMessageCommand : public AbstractFileCommand
{
public:
  AbstractMessageCommand(const string &name,
                         const string &short_help,
                         const string &long_help,
                         bool read_only,
                         bool needs_database)
    : AbstractFileCommand(name, short_help, long_help, read_only, needs_database)
  {
  }

  bool isMessageSpam() const;

  void processFile(const ConfigManager &config,
                   SpamFilter &filter,
                   const File *stream_file);

  virtual void processMessage(const ConfigManager &config,
                              SpamFilter &filter,
                              const File *stream_file,
                              Message &message,
                              int message_number,
                              bool &is_spam) = 0;

private:
  bool shouldProcessMessage(const ConfigManager &config,
                            Message &message);

  void processMailReader(const ConfigManager &config,
                         SpamFilter &filter,
                         AutoPurger &purger,
                         const File *stream_file,
                         Ptr<AbstractMailMessageReader> &reader);

  void processTokenStream(const ConfigManager &config,
                          SpamFilter &filter,
                          AutoPurger &purger,
                          const File *stream_file,
                          istream &stream);

  bool m_isMessageSpam;
};

#endif // _AbstractMessageCommand_h
