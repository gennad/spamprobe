///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_spam.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Command_spam_h
#define _Command_spam_h

#include "AbstractMessageCommand.h"

class Command_spam : public AbstractMessageCommand
{
public:
  static const Ref<AbstractMessageCommand> createGoodCommand();
  static const Ref<AbstractMessageCommand> createSpamCommand();
  static const Ref<AbstractMessageCommand> createTrainGoodCommand();
  static const Ref<AbstractMessageCommand> createTrainSpamCommand();

  void processMessage(const ConfigManager &config,
                      SpamFilter &filter,
                      const File *stream_file,
                      Message &message,
                      int message_number,
                      bool &is_spam);

protected:
  Command_spam(const string &name,
               const string &short_help,
               const string &long_help,
               bool read_only,
               bool needs_database,
               bool is_spam,
               bool is_train)
    : AbstractMessageCommand(name, short_help, long_help, read_only, needs_database),
      m_isSpam(is_spam),
      m_isTrain(is_train)
  {
  }

private:
  bool m_isSpam;
  bool m_isTrain;
};

#endif // _Command_spam_h
