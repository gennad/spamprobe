///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_receive.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_receive.h"

const Ref<AbstractCommand> Command_receive::createReceiveCommand()
{
  return Ref<AbstractCommand>(new Command_receive("receive",
                                                  "receive       Scores message, prints score, and updates database accordingly.",
                                                  "receive\n"
                                                  "    Tells SpamProbe to read its standard input (or a file specified\n"
                                                  "    after the receive command) and score it using the current\n"
                                                  "    databases.  Once the message has been scored the message is\n"
                                                  "    classified as either spam or non-spam and its word counts are\n"
                                                  "    written to the appropriate database.  The message's score is\n"
                                                  "    written to stdout along with a single word.  For example:\n"
                                                  "\n"
                                                  "      SPAM 0.9999999 595f0150587edd7b395691964069d7af\n"
                                                  "\n"
                                                  "    or\n"
                                                  "\n"
                                                  "      GOOD 0.0200000 595f0150587edd7b395691964069d7af\n"
                                                  "\n"
                                                  "    The string of numbers and letters after the score is the message's\n"
                                                  "    digest, a 32 character number which uniquely identifies the\n"
                                                  "    message.  The digest is used by SpamProbe to recognize messages\n"
                                                  "    that it has processed previously so that it can keep its word\n"
                                                  "    counts consistent if the message is reclassified.\n"
                                                  "\n"
                                                  "    Using the -T option additionally lists the terms used to produce\n"
                                                  "    the score along with their counts (number of times they were found\n"
                                                  "    in the message).\n",
                                                  false,
                                                  true,
                                                  false,
                                                  true));
}

const Ref<AbstractCommand> Command_receive::createScoreCommand()
{
  return Ref<AbstractCommand>(new Command_receive("score",
                                                  "score         Scores message and prints score.",
                                                  "score\n"
                                                  "    Similar to receive except that the database is not modified in\n"
                                                  "    any way.  Because it does not update the database multiple\n"
                                                  "    processes can run this command at the same time on a single\n"
                                                  "    database.  You might use this instead of receive or train\n"
                                                  "    if you want to score incoming email and then use train-spam\n"
                                                  "    or train-good on the email later to update the database\n"
                                                  "    in a cron task.  Generally train is a better command to\n"
                                                  "    use for incoming email.\n",
                                                  true,
                                                  true,
                                                  false,
                                                  false));
}

const Ref<AbstractCommand> Command_receive::createTrainCommand()
{
  return Ref<AbstractCommand>(new Command_receive("train",
                                                  "train         Scores message, prints score, and updates database if necessary.",
                                                  "train [filename...]\n"
                                                  "    Functionally identical to receive except that the database is only\n"
                                                  "    modified if the message was difficult to classify.  In practice\n"
                                                  "    this can reduce the number of database updates to as little as 10%\n"
                                                  "    of messages received.  Generally this is the best command to use\n"
                                                  "    for incoming mail.\n",
                                                  false,
                                                  true,
                                                  true,
                                                  true));
}

void Command_receive::processMessage(const ConfigManager &config,
                                     SpamFilter &filter,
                                     const File *stream_file,
                                     Message &message,
                                     int message_number,
                                     bool &is_spam)
{
  SpamFilter::Score score = filter.scoreMessage(message);
  is_spam = score.isSpam();
  printMessageScore(config, filter, is_spam, score.getValue(), message);
  if (isReadOnly()) {
    // no updates needed
  } else if (m_isTrain) {
    is_spam = filter.trainOnMessage(message, score, is_spam, true);
  } else {
    is_spam = filter.classifyMessage(message, is_spam);
  }
}
