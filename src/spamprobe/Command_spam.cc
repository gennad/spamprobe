///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_spam.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_spam.h"

const Ref<AbstractMessageCommand> Command_spam::createGoodCommand()
{
  return Ref<AbstractMessageCommand>(new Command_spam("good",
                                                      "good          Classifies messages as good (non-spam).",
                                                      "good [filename...]\n"
                                                      "    Scans each file (or stdin if no file is specified) and reclassifies\n"
                                                      "    every email in the file as non-spam.  The databases are updated\n"
                                                      "    appropriately.  Messages previously classified as good (recognized\n"
                                                      "    using their MD5 digest or message ids) are ignored.  Messages\n"
                                                      "    previously classified as spam are reclassified as good.\n",
                                                      false,
                                                      true,
                                                      false,
                                                      false));
}

const Ref<AbstractMessageCommand> Command_spam::createSpamCommand()
{
  return Ref<AbstractMessageCommand>(new Command_spam("spam",
                                                      "spam          Classifies messages as spam.",
                                                      "spam [filename...]\n"
                                                      "    Scans each file (or stdin if no file is specified) and reclassifies\n"
                                                      "    every email in the file as spam.  The databases are updated\n"
                                                      "    appropriately.  Messages previously classified as spam (recognized\n"
                                                      "    using their MD5 digest of message ids) are ignored.  Messages\n"
                                                      "    previously classified as good are reclassified as spam.\n",
                                                      false,
                                                      true,
                                                      true,
                                                      false));
}

const Ref<AbstractMessageCommand> Command_spam::createTrainGoodCommand()
{
  return Ref<AbstractMessageCommand>(new Command_spam("train-good",
                                                      "train-good    Classifies messages as good (non-spam).",
                                                      "train-good [filename...]\n"
                                                      "    Functionally identical to 'good' command except that it only\n"
                                                      "    updates the database for messages that are either incorrectly\n"
                                                      "    classified (i.e. classified as spam) or are difficult to\n"
                                                      "    classify.  In practice this can reduce amount of database updates\n"
                                                      "    to as little as 10% of messages.\n",
                                                      false,
                                                      true,
                                                      false,
                                                      true));
}

const Ref<AbstractMessageCommand> Command_spam::createTrainSpamCommand()
{
  return Ref<AbstractMessageCommand>(new Command_spam("train-spam",
                                                      "train-spam    Classifies messages as spam.",
                                                      "train-spam [filename...]\n"
                                                      "    Functionally identical to 'spam' command except that it only\n"
                                                      "    updates the database for messages that are either incorrectly\n"
                                                      "    classified (i.e. classified as good) or are difficult to\n"
                                                      "    classify.  In practice this can reduce amount of database updates\n"
                                                      "    to as little as 10% of messages.\n",
                                                      false,
                                                      true,
                                                      true,
                                                      true));
}

void Command_spam::processMessage(const ConfigManager &config,
                                  SpamFilter &filter,
                                  const File *stream_file,
                                  Message &message,
                                  int message_number,
                                  bool &is_spam)
{
  if (m_isTrain) {
    is_spam = filter.trainOnMessage(message, filter.scoreMessage(message), m_isSpam, false);
  } else {
    is_spam = filter.classifyMessage(message, m_isSpam);
  }
  logMessageProcessing(true, message, stream_file);
}
