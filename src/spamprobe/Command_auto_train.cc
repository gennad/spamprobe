///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_auto_train.cc 272 2007-01-06 19:37:27Z brian $
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

#include <stdexcept>
#include "AutoTrainMailMessageReader.h"
#include "AutoPurger.h"
#include "ParserConfig.h"
#include "IstreamCharReader.h"
#include "TraditionalMailMessageParser.h"
#include "LineReader.h"
#include "MD5Digester.h"
#include "MailMessageDigester.h"
#include "Command_spam.h"
#include "WordData.h"
#include "RegularExpression.h"
#include "SpamFilter.h"
#include "FrequencyDB.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "MailMessageReaderFactory.h"
#include "Command_auto_train.h"

const Ref<AbstractCommand> Command_auto_train::createAutoTrainCommand()
{
  return Ref<AbstractCommand>(new Command_auto_train("auto-train",
                                                     "auto-train    Builds database using train mode.",
                                                     "auto-train GOOD filename... SPAM filename...\n"
                                                     "    Attempts to efficiently build a database from all of the named\n"
                                                     "    files.  You may specify one or more file of each type.  Prior to\n"
                                                     "    each set of file names you must include the word SPAM or GOOD to\n"
                                                     "    indicate what type of mail is contained in the files which follow\n"
                                                     "    on the command line.\n"
                                                     "\n"
                                                     "    The case of the SPAM and GOOD keywords is important.  Any number of\n"
                                                     "    file names can be specified between the keywords.  The command line\n"
                                                     "    format is very flexible.  You can even use a find command in\n"
                                                     "    backticks to process whole directory trees of files. For example:\n"
                                                     "\n"
                                                     "      spamprobe auto-train SPAM spams/* GOOD `find hams -type f`\n"
                                                     "\n"
                                                     "    SpamProbe pre-scans the files to determine how many emails of each\n"
                                                     "    type exist and then trains on hams and spams in a random sequence\n"
                                                     "    that balances the inflow of each type so that the train command can\n"
                                                     "    work most effectively.  For example if you had 400 hams and 400\n"
                                                     "    spams, auto-train will generally process one spam, then one ham,\n"
                                                     "    etc.  If you had 4000 spams and 400 hams then auto-train will\n"
                                                     "    generally process 10 spams, then one ham, etc.\n"
                                                     "\n"
                                                     "    Since this command will likely take a long time to run it is often\n"
                                                     "    desireable to use it with the -v option to see progress information\n"
                                                     "    as the messages are processed.\n"
                                                     "\n"
                                                     "      spamprobe -v auto-train SPAM spams/* GOOD hams/* \n",
                                                     false));
}

const Ref<AbstractCommand> Command_auto_train::createAutoLearnCommand()
{
  return Ref<AbstractCommand>(new Command_auto_train("auto-learn",
                                                     "auto-learn    Builds database using learn mode.",
                                                     "auto-learn GOOD filename... SPAM filename...\n"
                                                     "    Similar to auto-train but adds all messages in all specified files to\n"
                                                     "    the database, even if they are easily determined to be good or spam.\n"
                                                     "    Generally speaking auto-train should be used instead of auto-learn.\n",
                                                     true));
}

void Command_auto_train::logMessage(const ConfigManager &config,
                                    SpamFilter &filter,
                                    const string &type,
                                    bool correct,
                                    Message &message)
{
  string subject;

  const string::size_type MAX_SUBJECT_LENGTH = 20;
  message.getHeader("subject", subject);
  if (subject.length() > MAX_SUBJECT_LENGTH) {
    subject.erase(MAX_SUBJECT_LENGTH, string::npos);
    subject += "...";
  }

  cout << type;

  if (correct) {
    cout << " PASS";
  } else {
    cout << " FAIL";
  }

  cout << " DIGEST " << message.getDigest()
       << " SUBJECT " << subject;

  cout << endl;

  if (config.commandConfig()->shouldShowTerms() && message.getTopTokenCount() > 0) {
    printTerms(filter.getDB(), cout, message, "    ");
  }
}

bool Command_auto_train::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  bool answer = true;
  for (int i = 0; i < num_args; ++i) {
    string arg(config.commandConfig()->arg(i));
    if (arg != "LOG" && arg != "SPAM" && arg != "GOOD" && !MailMessageReaderFactory::isAcceptableFile(File(arg))) {
      cerr << "error: " << arg << " is not a file" << endl;
      answer = false;
    }
  }

  return answer;
}

int Command_auto_train::execute(const ConfigManager &config,
                                SpamFilter &filter)
{
  openDatabase(config, filter);

  Ptr<AutoTrainMailMessageReader> mail_reader(new AutoTrainMailMessageReader());
  bool is_spam_file = false;
  bool should_log = false;
  for (int i = 0, limit = config.commandConfig()->numArgs(); i < limit; ++i) {
    string arg(config.commandConfig()->arg(i));
    if (arg == "LOG") {
      should_log = true;
    } else if (arg == "SPAM") {
      is_spam_file = true;
    } else if (arg == "GOOD") {
      is_spam_file = false;
    } else {
      File file(arg);
      if (!MailMessageReaderFactory::isAcceptableFile(file)) {
        throw runtime_error(string("file does not exist: ") + arg);
      }
      mail_reader->addMailboxFile(is_spam_file, arg);
    }
  }

  Ref<AbstractMessageCommand> good_command;
  Ref<AbstractMessageCommand> spam_command;

  if (m_isLearn) {
    good_command = Command_spam::createGoodCommand();
    spam_command = Command_spam::createSpamCommand();
  } else {
    good_command = Command_spam::createTrainGoodCommand();
    spam_command = Command_spam::createTrainSpamCommand();
  }

  bool is_message_spam = false;
  int message_num = 0;

  ParserConfig *parser_config = config.parserConfig();

  TraditionalMailMessageParser parser(parser_config);
  MailMessageDigester digester;

  AutoPurger purger(config, filter);
  int cumulative_message_count = 0;
  Ptr<MailMessage> mail_message;
  mail_message.set(mail_reader->readMessage());
  while (mail_message.isNotNull()) {
      Ptr<Message> msg(parser.parseMailMessage(mail_message.get()));
      msg->setSource(mail_message.release());
      digester.assignDigestToMessage(msg.get(), msg->source(), parser_config->spamprobeFieldName());

      SpamFilter::Score score;
      if (should_log) {
        score = filter.scoreMessage(*msg);
      }
      bool scored_as_spam = should_log && filter.scoreMessage(*msg).isSpam();
      bool is_spam = mail_reader->messageWasSpam();
      if (should_log) {
        logMessage(config, filter, is_spam ? "SPAM" : "GOOD", score.isSpam() == is_spam, *msg);
      }
      if (is_spam) {
        spam_command->processMessage(config, filter, &mail_reader->messageFile(), *msg, message_num, is_spam);
      } else {
        good_command->processMessage(config, filter, &mail_reader->messageFile(), *msg, message_num, is_spam);
      }
      purger.processedMessage();

      mail_message.set(mail_reader->readMessage());
  }
  purger.finish();
  return 0;
}
