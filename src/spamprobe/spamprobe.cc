///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: spamprobe.cc 272 2007-01-06 19:37:27Z brian $
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

#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include "CommandConfig.h"
#include "Command_auto_train.h"
#include "Command_cleanup.h"
#include "Command_counts.h"
#include "Command_create_config.h"
#include "Command_create_db.h"
#include "Command_dump.h"
#include "Command_edit_term.h"
#include "Command_exec.h"
#include "Command_export.h"
#include "Command_help.h"
#include "Command_import.h"
#include "Command_print.h"
#include "Command_purge.h"
#include "Command_purge_terms.h"
#include "Command_receive.h"
#include "Command_remove.h"
#include "Command_spam.h"
#include "Command_summarize.h"
#include "Command_tokenize.h"
#include "Command_train_test.h"
#include "ConfigManager.h"
#include "CleanupManager.h"
#include "DatabaseConfig.h"
#include "InterruptedException.h"
#include "File.h"
#include "SpamFilter.h"
#include "ParserConfig.h"
#include "WordData.h"
#include "SimpleTokenSelector.h"

static const string DB_FILENAME("sp_words");

static const int MAX_MAX_CACHE_TERMS = 100000;
static const int MAX_MESSAGES_PER_PURGE = 100000;

// must be global so that an atexit() routine can close the filter database
static Ptr<SpamFilter> s_filter;

bool process_extended_options(const vector<string> &option_names,
                              ConfigManager &config_mgr)
{
  bool failed = false;

  for (vector<string>::const_iterator i = option_names.begin(); i != option_names.end(); ++i) {
    string option_name(*i);
    if (option_name.length() > 0) {
      if (option_name == "graham") {
        config_mgr.parserConfig()->setMaxPhraseTerms(1);
        config_mgr.parserConfig()->setRemoveHTML(false);
        config_mgr.parserConfig()->setMinTermLength(1);
        config_mgr.parserConfig()->setMaxTermLength(90);
        config_mgr.filterConfig()->setTermsForScore(15);
        config_mgr.filterConfig()->setMaxWordRepeats(1);
        config_mgr.filterConfig()->setNewWordScore(0.4);
        config_mgr.filterConfig()->setExtendTopTerms(false);
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
        config_mgr.filterConfig()->setScoreMode(FilterConfig::SCORE_ORIGINAL);
      } else if (option_name == "suspicious-tags") {
        config_mgr.parserConfig()->setKeepSuspiciousTags(true);
      } else if (option_name == "honor-status-header") {
        config_mgr.commandConfig()->setStatusFieldName("status");
      } else if (option_name == "honor-xstatus-header") {
        config_mgr.commandConfig()->setStatusFieldName("x-status");
      } else if (option_name == "orig-score") {
        config_mgr.filterConfig()->setScoreMode(FilterConfig::SCORE_ORIGINAL);
      } else if (option_name == "tokenized") {
        config_mgr.commandConfig()->setStreamIsPretokenized(true);
      } else if (option_name == "ignore-body") {
        config_mgr.parserConfig()->setIgnoreBody(true);
      } else if (option_name == "whitelist") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setIgnoreBody(true);
      } else if (option_name != "normal") {
        cerr << "error: unknown option: " << option_name << endl;
        failed = true;
      }
    }
  }

  return failed;
}

bool process_test_cases(const vector<string> &test_cases,
                        ConfigManager &config_mgr)
{
  bool failed = false;

  for (vector<string>::const_iterator i = test_cases.begin(); i != test_cases.end(); ++i) {
    string test_case(*i);
    if (test_case.length() > 0) {
      if (test_case == "graham") {
        config_mgr.parserConfig()->setMaxPhraseTerms(1);
        config_mgr.parserConfig()->setRemoveHTML(false);
        config_mgr.parserConfig()->setMinTermLength(1);
        config_mgr.parserConfig()->setMaxTermLength(90);
        config_mgr.filterConfig()->setTermsForScore(15);
        config_mgr.filterConfig()->setMaxWordRepeats(1);
        config_mgr.filterConfig()->setNewWordScore(0.4);
        config_mgr.filterConfig()->setExtendTopTerms(false);
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
      } else if (test_case == "all-sigs") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
      } else if (test_case == "all-sigs-3") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
      } else if (test_case == "all-sigs-5") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "all-phrases-2-3") {
        config_mgr.parserConfig()->setMinPhraseTerms(2);
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
      } else if (test_case == "all-phrases-3-4") {
        config_mgr.parserConfig()->setMinPhraseTerms(3);
        config_mgr.parserConfig()->setMaxPhraseTerms(4);
      } else if (test_case == "all-phrases-4-5") {
        config_mgr.parserConfig()->setMinPhraseTerms(4);
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "all-phrases-2-5") {
        config_mgr.parserConfig()->setMinPhraseTerms(2);
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "all-phrases-2-3-ext") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setMinPhraseTerms(2);
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
      } else if (test_case == "all-phrases-2-3-water") {
        config_mgr.filterConfig()->setWaterCounts(true);
        config_mgr.parserConfig()->setMinPhraseTerms(2);
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
      } else if (test_case == "all-phrases-3-5") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setMinPhraseTerms(3);
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "all-phrases-5") {
        config_mgr.filterConfig()->setTermsForScore(5);
        config_mgr.filterConfig()->setMaxWordRepeats(5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.parserConfig()->setMinPhraseTerms(5);
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "old-graham") {
        config_mgr.parserConfig()->setMaxPhraseTerms(1);
        config_mgr.parserConfig()->setRemoveHTML(false);
        config_mgr.parserConfig()->setMinTermLength(1);
        config_mgr.parserConfig()->setMaxTermLength(90);
        config_mgr.filterConfig()->setTermsForScore(15);
        config_mgr.filterConfig()->setMaxWordRepeats(1);
        config_mgr.filterConfig()->setNewWordScore(0.2);
        config_mgr.filterConfig()->setExtendTopTerms(false);
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
      } else if (test_case == "nox") {
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
      } else if (test_case == "all") {
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
      } else if (test_case == "wide-open") {
        config_mgr.parserConfig()->setRemoveHTML(false);
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
      } else if (test_case == "alt1") {
        config_mgr.filterConfig()->setScoreMode(FilterConfig::SCORE_ALT1);
      } else if (test_case == "low-score") {
        config_mgr.filterConfig()->setNewWordScore(0.2);
        config_mgr.filterConfig()->setMinWordCount(5);
      } else if (test_case == "high-score") {
        config_mgr.filterConfig()->setNewWordScore(0.88);
        config_mgr.filterConfig()->setMinWordCount(5);
      } else if (test_case == "nox-high-score") {
        config_mgr.filterConfig()->setNewWordScore(0.88);
        config_mgr.filterConfig()->setMinWordCount(5);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
      } else if (test_case == "no-phrase") {
        config_mgr.parserConfig()->setMaxPhraseTerms(1);
      } else if (test_case == "normal-ext") {
        config_mgr.filterConfig()->setExtendTopTerms(true);
      } else if (test_case == "no-min") {
        config_mgr.filterConfig()->setMinWordCount(1);
      } else if (test_case == "1-days") {
        WordData::setTodayDate(1);
      } else if (test_case == "2-days") {
        WordData::setTodayDate(2);
      } else if (test_case == "3-days") {
        WordData::setTodayDate(3);
      } else if (test_case == "10-days") {
        WordData::setTodayDate(10);
      } else if (test_case == "normal-3") {
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
      } else if (test_case == "normal-5") {
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
      } else if (test_case == "phrases-2") {
        config_mgr.parserConfig()->setMaxPhraseTerms(2);
        config_mgr.parserConfig()->setMinPhraseTerms(2);
      } else if (test_case == "phrases-3") {
        config_mgr.parserConfig()->setMaxPhraseTerms(3);
        config_mgr.parserConfig()->setMinPhraseTerms(3);
      } else if (test_case == "phrases-4") {
        config_mgr.parserConfig()->setMaxPhraseTerms(4);
        config_mgr.parserConfig()->setMinPhraseTerms(4);
      } else if (test_case == "phrases-5") {
        config_mgr.parserConfig()->setMaxPhraseTerms(5);
        config_mgr.parserConfig()->setMinPhraseTerms(5);
      } else if (test_case == "no-prefixes") {
        config_mgr.parserConfig()->headers()->setBlankPrefixesMode();
      } else if (test_case == "12-char-phrase") {
        config_mgr.parserConfig()->setMinPhraseChars(12);
      } else if (test_case == "20-char-phrase") {
        config_mgr.parserConfig()->setMaxPhraseTerms(20);
        config_mgr.parserConfig()->setMaxPhraseChars(20);
      } else if (test_case == "html") {
        config_mgr.parserConfig()->setRemoveHTML(false);
      } else if (test_case == "15/1") {
        config_mgr.filterConfig()->setTermsForScore(15);
        config_mgr.filterConfig()->setMaxWordRepeats(1);
      } else if (test_case == "15/1-no-phrase") {
        config_mgr.filterConfig()->setTermsForScore(15);
        config_mgr.filterConfig()->setMaxWordRepeats(1);
        config_mgr.parserConfig()->setMaxPhraseTerms(1);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
      } else if (test_case == "suspicious") {
        config_mgr.parserConfig()->setKeepSuspiciousTags(true);
      } else if (test_case == "nox-suspicious") {
        config_mgr.parserConfig()->setKeepSuspiciousTags(true);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
      } else if (test_case == "min-0.25") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.25);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "min-0.33-10") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.33);
        config_mgr.filterConfig()->setMinArraySize(10);
      } else if (test_case == "min-0.33-8") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.33);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "min-0.40") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.40);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "min-0.45") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.45);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "min-0.49") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.49);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "min-0.495") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.495);
        config_mgr.filterConfig()->setMinArraySize(8);
      } else if (test_case == "trial") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.40);
        config_mgr.filterConfig()->setMinArraySize(10);
      } else if (test_case == "trial-2") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.25);
        config_mgr.filterConfig()->setMinArraySize(12);
        config_mgr.filterConfig()->setTermsForScore(30);
        config_mgr.filterConfig()->setMaxWordRepeats(2);
        config_mgr.filterConfig()->setWaterCounts(true);
        config_mgr.filterConfig()->setNewWordScore(0.5);
        config_mgr.filterConfig()->setExtendTopTerms(true);
      } else if (test_case == "trial-3") {
        config_mgr.filterConfig()->setExtendTopTerms(true);
        config_mgr.filterConfig()->setWaterCounts(true);
      } else if (test_case == "susp-high-score") {
        config_mgr.filterConfig()->setNewWordScore(0.88);
        config_mgr.parserConfig()->setKeepSuspiciousTags(true);
      } else if (test_case == "use-sa") {
        config_mgr.parserConfig()->headers()->addHeaderPrefix("x-spam-status", "sa", "sa");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("x-spam-status");
      } else if (test_case == "water-counts") {
        config_mgr.filterConfig()->setWaterCounts(true);
        config_mgr.filterConfig()->setNewWordScore(0.4);
      } else if (test_case == "original") {
        config_mgr.filterConfig()->setScoreMode(FilterConfig::SCORE_ORIGINAL);
      } else if (test_case == "0.5") {
        config_mgr.filterConfig()->setDefaultThreshold(0.5);
      } else if (test_case == "0.6") {
        config_mgr.filterConfig()->setDefaultThreshold(0.6);
      } else if (test_case == "0.7") {
        config_mgr.filterConfig()->setDefaultThreshold(0.7);
      } else if (test_case == "0.8") {
        config_mgr.filterConfig()->setDefaultThreshold(0.8);
      } else if (test_case == "0.9") {
        config_mgr.filterConfig()->setDefaultThreshold(0.9);
      } else if (test_case == "min-10") {
        config_mgr.filterConfig()->setMinWordCount(10);
      } else if (test_case == "normal-headers-only") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->setIgnoreBody(true);
        config_mgr.parserConfig()->headers()->setDefaultHeadersMode();
      } else if (test_case == "nox-headers-only") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->setIgnoreBody(true);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
      } else if (test_case == "all-headers-only") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->setIgnoreBody(true);
        config_mgr.parserConfig()->headers()->setAllHeadersMode();
      } else if (test_case == "headers-only") {
        config_mgr.parserConfig()->headers()->setNoHeadersMode();
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("from");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("to");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("cc");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("bcc");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("subject");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("reply-to");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("received");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("sender");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("x-mailer");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("errors-to");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("x-beenthere");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("list-id");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("user-agent");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("references");
        config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix("message-id");
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->setIgnoreBody(true);
      } else if (test_case == "multi") {
        config_mgr.filterConfig()->clearTokenSelectors();
        config_mgr.filterConfig()->addTokenSelector(new SimpleTokenSelector(Token::FLAG_ANY, "H"));
        config_mgr.filterConfig()->addTokenSelector(new SimpleTokenSelector(Token::FLAG_ANY - Token::FLAG_PHRASE, ""));
        config_mgr.filterConfig()->addTokenSelector(new SimpleTokenSelector(Token::FLAG_ANY - Token::FLAG_DERIVED, ""));
        config_mgr.filterConfig()->addTokenSelector(new TokenSelector());
      } else if (test_case == "max-250-terms") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
        config_mgr.parserConfig()->setMaxTermsPerMessage(250);
      } else if (test_case == "max-350-terms") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
        config_mgr.parserConfig()->setMaxTermsPerMessage(350);
      } else if (test_case == "max-500-terms") {
        config_mgr.filterConfig()->setMinDistanceForScore(0.30);
        config_mgr.filterConfig()->setMinArraySize(10);
        config_mgr.parserConfig()->headers()->setNonXHeadersMode();
        config_mgr.parserConfig()->setMaxTermsPerMessage(500);
      } else if (test_case != "normal") {
        cerr << "error: unknown test case: " << test_case << endl;
        failed = true;
      }
    }
  }

  return failed;
}

static bool parse_int_arg(const char *arg,
                          const char *option,
                          int &value,
                          int min_val,
                          int max_val)
{
  value = atoi(arg);
  if (value >= min_val && value <= max_val) {
    return true;
  }

  cerr << "error: -" << option << " requires integer argument between "
       << min_val << " and " << max_val << endl;
  return false;
}

static bool parse_double_arg(const char *arg,
                             const char *option,
                             double &value,
                             double min_val,
                             double max_val)
{
  value = atof(arg);
  if (value >= min_val && value <= max_val) {
    return true;
  }

  cerr << "error: -" << option << " requires number argument between "
       << min_val << " and " << max_val << endl;
  return false;
}

static bool set_headers(ConfigManager &config_mgr,
                        const char *optarg)
{
  bool successful = true;
  if (strcmp(optarg, "all") == 0) {
    config_mgr.parserConfig()->headers()->setAllHeadersMode();
  } else if (strcmp(optarg, "nox") == 0) {
    config_mgr.parserConfig()->headers()->setNonXHeadersMode();
  } else if (strcmp(optarg, "normal") == 0) {
    config_mgr.parserConfig()->headers()->setDefaultHeadersMode();
  } else if (strcmp(optarg, "none") == 0) {
    config_mgr.parserConfig()->headers()->setNoHeadersMode();
  } else if (optarg[0] == '+') {
    config_mgr.parserConfig()->headers()->addSimpleHeaderPrefix(optarg + 1);
  } else if (optarg[0] == '-') {
    config_mgr.parserConfig()->headers()->removeHeaderPrefix(optarg + 1);
  } else {
    cerr << "error: -H option requires all, nox, none, normal, +header, or -header" << endl;
    successful = false;
  }
  return successful;
}

static void print_version()
{
  cout << "SpamProbe v" << VERSION;
  if (s_filter.get()) {
    try {
      s_filter->open(true);
    } catch (...) {
    }
    if (s_filter->getDB()) {
      cout << " using " << s_filter->getDB()->getDatabaseType();
    }
  }
  cout << " database." << endl
       << endl
       << "Copyright 2002-2006 Burton Computer Corporation" << endl
       << "This program is distributed in the hope that it will be useful," << endl
       << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl
       << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl
       << "Q Public License for more details." << endl;
}

static void print_usage()
{
  cerr << "usage: spamprobe [options] command [args]\n"
       << " Try \"spamprobe help\" to list all commands and options.\n"
       << "  or \"spamprobe help command\" for detailed help on a single command.\n"
       << endl;
}

//
// Cleanup function for use by signal handlers.
//
void quick_close(int signum)
{
  signal(signum,  SIG_DFL);
  try {
    cerr << "caught signal " << signum << ": quitting" << endl;
  } catch (...) {
    // not allowed to throw any exceptions in signal handlers
  }
}

//
// For user interrupts give the any outstanding database operation time
// to complete before exiting.
//
bool interrupt_or_exit(int signum)
{
  if (s_filter.get() && s_filter->getDB()) {
    return s_filter->getDB()->requestInterrupt();
  } else {
    return true;
  }
}

//
// Signal handler to close the database on receipt of a signal
// to ensure that any locks it might hold are cleared.
//
void close_and_exit(int signum)
{
  if (interrupt_or_exit(signum)) {
    quick_close(signum);
    exit(127);
  }
}

//
// Cleanup function to close the database on receipt of a signal
// to ensure that any locks it might hold are cleared.  After closing
// calls abort to dump core.
//
void close_and_abort(int signum)
{
  quick_close(signum);
  signal(SIGABRT, SIG_DFL);
  abort();
}

//
// Register signal handler to cleanup on abnormal exit from program (i.e.
// ^C or a crash.
//
void install_signal_handler()
{
  // clean shutdown
  signal(SIGINT,  close_and_exit);
  signal(SIGTERM, close_and_exit);
  signal(SIGPIPE, close_and_exit);

  // close and abort
  signal(SIGQUIT, close_and_abort);
  signal(SIGSEGV, close_and_abort);
  signal(SIGBUS,  close_and_abort);
  signal(SIGABRT, close_and_abort);
}

static void load_config_file(const File &basedir,
                             const string &filename,
                             ConfigManager &config_mgr)
{
  File config_file(basedir, filename);
  if (config_file.isFile()) {
    config_mgr.loadFromFile(config_file);
  }
}

static void define_commands(CommandConfig *command_config)
{
  command_config->addCommand(Command_auto_train::createAutoLearnCommand());
  command_config->addCommand(Command_auto_train::createAutoTrainCommand());
  command_config->addCommand(Ref<AbstractCommand>(new Command_cleanup));
  command_config->addCommand(Ref<AbstractCommand>(new Command_counts));
  command_config->addCommand(Ref<AbstractCommand>(new Command_create_config));
  command_config->addCommand(Ref<AbstractCommand>(new Command_create_db));
  command_config->addCommand(Ref<AbstractCommand>(new Command_dump));
  command_config->addCommand(Ref<AbstractCommand>(new Command_edit_term));
  command_config->addCommand(Ref<AbstractCommand>(new Command_exec(false)));
  command_config->addCommand(Ref<AbstractCommand>(new Command_exec(true)));
  command_config->addCommand(Ref<AbstractCommand>(new Command_export));
  command_config->addCommand(Command_summarize::createFindGoodCommand());
  command_config->addCommand(Command_summarize::createFindSpamCommand());
  command_config->addCommand(Command_spam::createGoodCommand());
  command_config->addCommand(Ref<AbstractCommand>(new Command_help));
  command_config->addCommand(Ref<AbstractCommand>(new Command_import));
  command_config->addCommand(Command_train_test::createLearnTestCommand());
  command_config->addCommand(Ref<AbstractCommand>(new Command_print));
  command_config->addCommand(Ref<AbstractCommand>(new Command_purge));
  command_config->addCommand(Ref<AbstractCommand>(new Command_purge_terms));
  command_config->addCommand(Command_receive::createReceiveCommand());
  command_config->addCommand(Ref<AbstractCommand>(new Command_remove));
  command_config->addCommand(Command_receive::createScoreCommand());
  command_config->addCommand(Command_spam::createSpamCommand());
  command_config->addCommand(Command_summarize::createSummarizeCommand());
  command_config->addCommand(Command_receive::createTrainCommand());
  command_config->addCommand(Command_spam::createTrainGoodCommand());
  command_config->addCommand(Command_spam::createTrainSpamCommand());
  command_config->addCommand(Ref<AbstractCommand>(new Command_tokenize));
  command_config->addCommand(Command_train_test::createTrainTestCommand());
}

int main(int argc,
         char **argv)
{
  try {
    bool usage_error = false;
    bool force_mbox = false;
    bool single_message_file = false;
    bool wants_version = false;
    bool return_spam_status = false;
    File shared_db_dir;
    File basedir(File::getHomeDir(), ".spamprobe");
    string config_filename("spamprobe.hdl");
    vector<string> test_cases;
    vector<string> extended_options;

    ConfigManager config_mgr;
    define_commands(config_mgr.commandConfig());
    load_config_file(basedir, config_filename, config_mgr);

    CommandConfig *command_config = config_mgr.commandConfig();
    DatabaseConfig *database_config = config_mgr.databaseConfig();
    ParserConfig *parser_config = config_mgr.parserConfig();
    FilterConfig *filter_config = config_mgr.filterConfig();

    optind = 1;
    int opt, opt_value;
    double opt_double_value;
    while ((opt = getopt(argc, argv, "a:cC:d:D:f:hH:g:l:mMo:p:P:r:Rs:t:TvVw:xXY78")) != EOF) {
      switch (opt) {
      case 'a':
        if (strlen(optarg) != 1) {
          cerr << "error: -a option requires a one character argument" << endl;
          usage_error = true;
        } else {
          parser_config->setReplaceNonAsciiChars(optarg[0]);
        }
        break;

      case 'c':
        command_config->setShouldCreateDbDir(true);
        break;

      case 'C':
        if (parse_int_arg(optarg, "C", opt_value, 0, 10000)) {
          filter_config->setMinWordCount(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 'd':
        {
          string path(optarg);
          string db_type;
          int target_size;
          database_config->parseCommandLineArg(path, db_type, target_size);
          database_config->setDatabaseType(db_type);
          database_config->setTargetSizeMB(target_size);
          basedir.setPath(path);
          load_config_file(basedir, config_filename, config_mgr);
          File private_db_file(basedir, DB_FILENAME);
          database_config->setPrivateFilename(private_db_file.getPath());
        }
        break;

      case 'D':
        {
          string path(optarg);
          string db_type;
          int target_size;
          database_config->parseCommandLineArg(path, db_type, target_size);
          shared_db_dir.setPath(path);
          File shared_db_file(shared_db_dir, DB_FILENAME);
          database_config->setSharedFilename(shared_db_file.getPath());
        }
        break;

      case 'f':
        {
          File config_file(optarg);
          if (!config_file.isFile()) {
            cerr << optarg << ": no such file" << endl;
            usage_error = true;
          } else {
            basedir.setPath(config_file.parent().getPath());
            config_filename = config_file.getName();
            load_config_file(basedir, config_filename, config_mgr);
          }
        }
        break;

      case 'g':
        parser_config->setSpamProbeFieldName(to_lower(optarg));
        break;

      case 'h':
        parser_config->setRemoveHTML(false);
        break;

      case 'H':
        if (!set_headers(config_mgr, optarg)) {
          usage_error = true;
        }
        break;

      case 'l':
        if (parse_double_arg(optarg, "l", opt_double_value, 0.0, 1.0)) {
          filter_config->setDefaultThreshold(opt_double_value);
        } else {
          usage_error = true;
        }
        break;

      case 'm':
        force_mbox = true;
        break;

      case 'M':
        single_message_file = true;
        break;

      case 'o':
        extended_options.push_back(optarg);
        break;

      case 'p':
        if (parse_int_arg(optarg, "p", opt_value, 1, 10)) {
          parser_config->setMaxPhraseTerms(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 'P':
        if (parse_int_arg(optarg, "P", opt_value, 1, MAX_MESSAGES_PER_PURGE)) {
          command_config->setMessagesPerPurge(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 'r':
        if (parse_int_arg(optarg, "r", opt_value, 1, 10)) {
          filter_config->setMaxWordRepeats(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 'R':
        single_message_file = true;
        return_spam_status = true;
        break;

      case 's':
        if (parse_int_arg(optarg, "s", opt_value, 0, MAX_MAX_CACHE_TERMS)) {
          if (opt_value == 0) {
            opt_value = MAX_MAX_CACHE_TERMS;
          }
          database_config->setMaxCacheTerms(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 't':
        test_cases.push_back(optarg);
        break;

      case 'T':
        command_config->setShouldShowTerms(true);
        break;

      case 'v':
        if (is_verbose) {
          is_debug = true;
        } else {
          is_verbose = true;
        }
        break;

      case 'V':
        wants_version = true;
        break;

      case 'w':
        if (parse_int_arg(optarg, "w", opt_value, 5, 500)) {
          filter_config->setTermsForScore(opt_value);
        } else {
          usage_error = true;
        }
        break;

      case 'x':
        filter_config->setExtendTopTerms(true);
        break;

      case 'X':
        filter_config->setTermsForScore(5);
        filter_config->setMaxWordRepeats(5);
        filter_config->setExtendTopTerms(true);
        break;

      case 'Y':
        command_config->setIgnoreContentLength(true);
        break;

      case '7':
        parser_config->setReplaceNonAsciiChars(' ');
        break;

      case '8':
        parser_config->setReplaceNonAsciiChars(-1);
        break;

      default:
        usage_error = true;
        break;
      }
    }

    // import locales so that tolower() will work properly
    setlocale(LC_ALL, "");

    if ((argc - optind) < 1) {
      usage_error = true;
    }

    string command;
    Ref<AbstractCommand> cmd;
    if (!usage_error) {
      command = argv[optind++];
      cmd = command_config->findCommand(command);
      if (cmd.isNull()) {
        cerr << "error: invalid command name: " << command << endl;
        usage_error = true;
      } else if (cmd->isReadOnly() && command_config->messagesPerPurge() > 0) {
        command_config->setMessagesPerPurge(0);
      }
    }

    if (cmd.isNotNull()) {
      command_config->addArgs(argv + optind);
      usage_error |= !cmd->argsOK(config_mgr);
    }

    install_signal_handler();
    s_filter.set(new SpamFilter(&config_mgr));

    if (usage_error || wants_version) {
      if (!wants_version) {
        print_usage();
      }
      print_version();
      return wants_version ? 0 : 1;
    }

    assert(cmd.isNotNull());

    if (single_message_file || (cmd->isSingleMessageCommand() && !force_mbox)) {
      command_config->setIgnoreFrom(true);
      command_config->setIgnoreContentLength(true);
    }

    if (process_test_cases(test_cases, config_mgr)) {
      print_usage();
      return 1;
    }

    if (process_extended_options(extended_options, config_mgr)) {
      print_usage();
      return 1;
    }

    cmd->execute(config_mgr, *s_filter);
    s_filter->close();
    s_filter.clear();

    if (return_spam_status) {
      return cmd->isMessageSpam() ? 0 : 1;
    }
  } catch (InterruptedException &ex) {
    cerr << "interrupted by signal" << endl;
  } catch (runtime_error &ex) {
    cerr << "caught runtime exception: " << ex.what() << endl;
    return 1;
  } catch (logic_error &ex) {
    cerr << "caught logic exception: " << ex.what() << endl;
    return 1;
  } catch (...) {
    cerr << "caught unknown exception" << endl;
    return 1;
  }

  return 0;
}
