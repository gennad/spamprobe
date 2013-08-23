///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_train_test.cc 272 2007-01-06 19:37:27Z brian $
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
#include "ParserConfig.h"
#include "IstreamCharReader.h"
#include "TraditionalMailMessageParser.h"
#include "AbstractMailMessageReader.h"
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
#include "Command_train_test.h"

const Ref<AbstractCommand> Command_train_test::createTrainTestCommand()
{
  return Ref<AbstractCommand>(new Command_train_test("train-test",
                                                     "train-test    Builds database using specs on stdin.",
                                                     "train-test\n"
                                                     "    Builds database using type and filenames read from stdin.\n"
                                                     "    Specified files are processed using train-good and train-spam.\n"
                                                     "    Each line of stdin must contain the word spam or good followed\n"
                                                     "    by the full path to a file containing one or more messages.\n"
                                                     "    This command is used by deveopers for testing spamprobe and is\n"
                                                     "    not really intended for general use.\n",
                                                     false));
}

const Ref<AbstractCommand> Command_train_test::createLearnTestCommand()
{
  return Ref<AbstractCommand>(new Command_train_test("learn-test",
                                                     "learn-test    Builds database using learn mode.",
                                                     "learn-test\n"
                                                     "    Builds database using type and filenames read from stdin.\n"
                                                     "    Specified files are processed using good and spam commands.\n"
                                                     "    Each line of stdin must contain the word spam or good followed\n"
                                                     "    by the full path to a file containing one or more messages.\n"
                                                     "    This command is used by deveopers for testing spamprobe and is\n"
                                                     "    not really intended for general use.\n",
                                                     true));
}

bool Command_train_test::argsOK(const ConfigManager &config)
{
  const int num_args = config.commandConfig()->numArgs();
  return num_args == 0;
}

int Command_train_test::execute(const ConfigManager &config,
                                SpamFilter &filter)
{
  openDatabase(config, filter);

  Ref<AbstractMessageCommand> good_command;
  Ref<AbstractMessageCommand> spam_command;

  if (m_isLearn) {
    good_command = Command_spam::createGoodCommand();
    spam_command = Command_spam::createSpamCommand();
  } else {
    good_command = Command_spam::createTrainGoodCommand();
    spam_command = Command_spam::createTrainSpamCommand();
  }

  string type_name, file_name, command;
  while (cin) {
    cin >> type_name;
    cin >> file_name;

    if (cin) {
      File message_file(file_name);
      if (!message_file.isFile()) {
        throw runtime_error(file_name + ": does not exist");
      }

      if (type_name == "spam") {
        spam_command->processFile(config, filter, &message_file);
      } else if (type_name == "good") {
        good_command->processFile(config, filter, &message_file);
      } else {
        throw runtime_error(string("invalid message type: ") + type_name);
      }
    }
  }
  return 0;
}
