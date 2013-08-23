///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_summarize.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_summarize.h"

const Ref<AbstractCommand> Command_summarize::createFindGoodCommand()
{
  return Ref<AbstractCommand>(new Command_summarize("find-good",
                                                    "find-good     Prints a brief summary of each good (non-spam) message.",
                                                    "find-good [filename...]\n"
                                                    "    Similar to score except that it prints a short summary and score\n"
                                                    "    for each message that is determined to be good.  This can be useful\n"
                                                    "    when testing.  Using the -T option additionally lists the terms\n"
                                                    "    used to produce the score along with their counts (number of times\n"
                                                    "    they were found in the message).\n",
                                                    false,
                                                    true,
                                                    true,
                                                    false));
}

const Ref<AbstractCommand> Command_summarize::createFindSpamCommand()
{
  return Ref<AbstractCommand>(new Command_summarize("find-spam",
                                                    "find-spam     Prints a brief summary of each spam message.",
                                                    "find-spam [filename...]\n"
                                                    "    Similar to score except that it prints a short summary and score\n"
                                                    "    for each message that is determined to be spam.  This can be useful\n"
                                                    "    when testing.  Using the -T option additionally lists the terms\n"
                                                    "    used to produce the score along with their counts (number of times\n"
                                                    "    they were found in the message).\n",
                                                    false,
                                                    true,
                                                    false,
                                                    true));
}

const Ref<AbstractCommand> Command_summarize::createSummarizeCommand()
{
  return Ref<AbstractCommand>(new Command_summarize("summarize",
                                                    "summarize     Prints a brief summary of each message.",
                                                    "summarize [filename...]\n"
                                                    "    Similar to score except that it prints a short summary and score\n"
                                                    "    for each message.  This can be useful when testing.  Using the -T\n"
                                                    "    option additionally lists the terms used to produce the score along\n"
                                                    "    with their counts (number of times they were found in the message).\n",
                                                    false,
                                                    true,
                                                    true,
                                                    true));
}

void Command_summarize::processMessage(const ConfigManager &config,
                                       SpamFilter &filter,
                                       const File *stream_file,
                                       Message &message,
                                       int message_number,
                                       bool &is_spam)
{
  SpamFilter::Score score = filter.scoreMessage(message);
  is_spam = score.isSpam();
  bool should_show = (m_includeGood && !is_spam) || (m_includeSpam && is_spam);
  if (should_show) {
    string subject, msg_id;
    message.getHeader("subject", subject);
    if (stream_file) {
      cout << "File: " << stream_file->getPath() << "\n";
    }
    cout << "Message Num: " << message_number << "\n"
         << "Message-ID: " << message.getID(msg_id) << "\n"
         << "Digest: " << message.getDigest() << "\n"
         << "Subject: " << subject << "\n"
         << "NumTerms: " << message.getTokenCount() << "\n"
         << "Score: " << scoreToString(score.getValue())
         << "\n";
    if (config.commandConfig()->shouldShowTerms() && message.getTopTokenCount() > 0) {
      printTerms(filter.getDB(), cout, message, "       ");
    }
  }
}
