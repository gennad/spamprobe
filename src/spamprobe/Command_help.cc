///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_help.cc 272 2007-01-06 19:37:27Z brian $
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
#include "Command_help.h"

Command_help::Command_help()
: AbstractCommand("help",
                  "help          Prints information about supported commands and options.",
                  "help [command...]\n"
                  "    With no arguments prints a summary of help for all commands and options.\n"
                  "    With arguments prints helpful information about each named command.\n",
                  true)
{
}

void Command_help::printCommandLineOptions(const CommandConfig *cmd_config)
{
  cout << "Command Line Options:\n"
       << " -a char\n"
       << "    Sets character used to replace non-ascii chars (default z).\n"
       << " -c\n"
       << "    Causes database directory to be created if not already present.\n"
       << " -C count\n"
       << "    Sets minimum count for a word to use computed probability.\n"
       << "    Default value is 5.\n"
       << " -d dirname\n"
       << "    Sets base directory in which spamprobe config file and database are stored.\n"
       << " -D dirname\n"
       << "    Sets base directory in which shared (read only) database is stored.\n"
       << " -f filename\n"
       << "    Sets configuration file used to set default options.\n"
       << " -g\n"
       << "    Sets name of mail header used to store SpamProbe score (default X-SpamProbe).\n"
       << "    This option is used to allow SpamProbe to find the current message's MD5 digest.\n"
       << " -h\n"
       << "    Causes all HTML tags to be parsed for tokens.\n"
       << " -H namelist\n"
       << "    Comma separated list of headers to be processed.  Can use all or none to\n"
       << "    cause all or none of the headers in the email to be processed.  Header names\n"
       << "    are not case sensitive,\n"
       << " -l number\n"
       << "    Sets the spam threshold value.  The number must be between 0.0 and 1.0.\n"
       << "    All messages with scores >= the threshold will be considered spam.  Default\n"
       << "    value is 0.6.\n"
       << " -m\n"
       << "    Causes SpamProbe to treat the message as an mbox file even if the command\n"
       << "    being run would normally treat it as a single message (receive and train).\n"
       << " -M\n"
       << "    Causes SpamProbe to assume that each file contains only one message.\n"
       << " -o option\n"
       << "    Enables the specified extended option.  Currently supported options are:\n"
       << "     graham           Use options similar to those from A Plan For Spam\n"
       << "     suspicious-tags  Force certain tags to be parsed for tokens\n"
       << "     honor-status-header  Ignore messages flagged as deleted in Status header\n"
       << "     honor-xstatus-header Ignore messages flagged as deleted in X-Status header\n"
       << "     orig-score           Use original scoring algorithm (deprecated)\n"
       << "     tokenized            Assume file consists of tokens one per line\n"
       << " -p num_words\n"
       << "    Sets maximum number of words per phrase (default 2)\n"
       << " -P num_messages\n"
       << "    Causes terms with count <= 2 to be purged from database evern num_messages\n"
       << "    messages.  Useful when training on large files to remove unique terms and avoid\n"
       << "    excessive database growth.\n"
       << " -r max_repeats\n"
       << "    Sets maximum number of times words can appear in top tokens for scoring.\n"
       << "    Default is 2.\n"
       << " -R\n"
       << "    Causes SpamProbe to return exit status 0 if message was good or 1 if spam.\n"
       << "    Also activates -M option.\n"
       << " -s max_terms\n"
       << "    Sets size of in-memory cache.  Larger values use more memory but boost performance.\n"
       << " -T\n"
       << "    Causes SpamProbe to output terms used to compute score.\n"
       << " -v\n"
       << "    Activates verbose output for some commands.\n"
       << " -V\n"
       << "    Causes SpamProbe to print version information and exit.\n"
       << " -w num_terms\n"
       << "    Sets number of terms used to compute score (default 27).\n"
       << " -x\n"
       << "    Causes SpamProbe to extend top terms array to hold all highly significant terms\n"
       << " -X\n"
       << "    Same as -w5 -r5 -x\n"
       << " -Y\n"
       << "    Causes SpamProbe to ignore Content-Length header when parsing mbox files.\n"
       << " -7\n"
       << "    Causes SpamProbe to ignore non-ascii characters.\n"
       << " -8\n"
       << "    Causes SpamProbe to retain non-ascii characters.\n"
       << endl;
}

void Command_help::printAllCommandsSummary(const CommandConfig *cmd_config)
{
  cout << "Supported Commands:\n";
  for (int i = 0, len = cmd_config->numCommands(); i < len; ++i) {
    Ref<AbstractCommand> cmd = cmd_config->command(i);
    cout << cmd->shortHelp() << endl;
  }
}

void Command_help::printDetailedHelp(const CommandConfig *cmd_config)
{
  for (int i = 0, len = cmd_config->numArgs(); i < len; ++i) {
    string name = cmd_config->arg(i);
    Ref<AbstractCommand> cmd = cmd_config->findCommand(name);
    if (cmd.isNull()) {
      cout << "NO SUCH COMMAND: " << name << endl;
    } else {
      printHelpForCommand(cmd);
    }
  }
}

void Command_help::printHelpForCommand(const Ref<AbstractCommand> &command)
{
  cout << command->longHelp() << endl;
}

int Command_help::execute(const ConfigManager &config,
                          SpamFilter &filter)
{
  const CommandConfig *cmd_config = config.commandConfig();
  if (cmd_config->numArgs() == 0) {
    printCommandLineOptions(cmd_config);
    printAllCommandsSummary(cmd_config);
  } else {
    printDetailedHelp(cmd_config);
  }
  return 0;
}
