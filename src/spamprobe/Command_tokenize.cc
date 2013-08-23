///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_tokenize.cc 272 2007-01-06 19:37:27Z brian $
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

#include <iomanip>
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_tokenize.h"

Command_tokenize::Command_tokenize()
  : AbstractMessageCommand("tokenize",
                          "tokenize      Prints all tokens for messages.",
                          "tokenize [filename...]\n"
                           "    Prints the tokens found in the file one word per line in human\n"
                           "    readable format with spam probability, good count, spam count,\n"
                           "    message count, and word in columns separated by whitespace.  Terms\n"
                           "    are listed in the order in which they were encountered in the\n"
                           "    message.  The standard unix sort command can be used to sort the\n"
                           "    terms as desired.  For example to list all words from 'most good'\n"
                           "    to 'least good' use this command:\n"
                           "\n"
                           "        spamprobe tokenize filename | sort -k 1nr -k 3nr\n"
                           "\n"
                           "    To list all words from 'most spammy' to 'least spammy' use this\n"
                           "    command:\n"
                           "\n"
                           "        spamprobe tokenize filename | sort -k 1n -k 2nr\n",
                          false,
                          true)
{
}

void Command_tokenize::processMessage(const ConfigManager &config,
                                      SpamFilter &filter,
                                      const File *stream_file,
                                      Message &message,
                                      int message_number,
                                      bool &is_spam)
{
  int good_message_count, spam_message_count;
  filter.getDB()->getMessageCounts(good_message_count, spam_message_count);

  int good_count, spam_count;
  for (int token_num = 0; token_num < message.getTokenCount(); ++token_num) {
    Token *token = message.getToken(token_num);
    filter.getDB()->getWordCounts(token->getWord(), good_count, spam_count);
    double score = filter.scoreTerm(good_count, spam_count, good_message_count, spam_message_count);
    cout << scoreToString(score)
         << dec
         << setw(8) << good_count
         << setw(8) << spam_count
         << setw(8) << token->getCount()
         << "  " << token->getWord()
         << "\n";
  }
}
