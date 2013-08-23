///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractCommand.cc 272 2007-01-06 19:37:27Z brian $
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
#include "ConfigManager.h"
#include "CommandConfig.h"
#include "DatabaseConfig.h"
#include "File.h"
#include "Message.h"
#include "SpamFilter.h"
#include "AbstractCommand.h"

const char *AbstractCommand::scoreToString(double score)
{
  static char buffer[32];
  sprintf(buffer, "%9.7f", score);
  return buffer;
}

bool AbstractCommand::argsOK(const ConfigManager &config)
{
  return true;
}

void AbstractCommand::openDatabase(const ConfigManager &config,
                                   SpamFilter &filter)
{
  if (config.commandConfig()->shouldCreateDbDir() && !isReadOnly()) {
    createDbDir(config);
  }
  filter.open(isReadOnly());
}

void AbstractCommand::printTerms(FrequencyDB *db,
                                 ostream &out,
                                 Message &msg,
                                 const string &indent)
{
  int good_count, spam_count;
  out << indent << "Spam Prob   Count    Good    Spam  Word\n";
  for (int i = 0; i < msg.getTopTokenCount(); ++i) {
    Token *tok = msg.getTopToken(i);
    db->getWordCounts(tok->getWord(), good_count, spam_count);
    out << indent
        << scoreToString(tok->getScore())
        << setw(8) << tok->getCount()
        << setw(8) << good_count
        << setw(8) << spam_count
        << "  " << tok->getWord()
        << "\n";
  }
}

void AbstractCommand::printMessageScore(const ConfigManager &config,
                                        SpamFilter &filter,
                                        bool is_spam,
                                        double score,
                                        Message &msg,
                                        const File *source)
{
  if (source) {
    cout << source->getPath()
         << ' ';
  }
  cout << (is_spam ? "SPAM " : "GOOD ")
       << scoreToString(score)
       << ' '
       << msg.getDigest();
  cout << endl;
  if (config.commandConfig()->shouldShowTerms() && msg.getTopTokenCount() > 0) {
    printTerms(filter.getDB(), cout, msg, "    ");
  }
}

bool AbstractCommand::isMessageSpam() const
{
  return false;
}

void AbstractCommand::createDbDir(const ConfigManager &config)
{
  File db_file(config.databaseConfig()->privateFilename());
  File db_dir = db_file.parent();
  if (!db_dir.exists()) {
    db_dir.makeDirectory(0700);
  }
}

void AbstractCommand::logMessageProcessing(bool processed,
                                           Message &message,
                                           const File *stream_file)
{
  if (!is_verbose) {
    return;
  }

  string subject;

  const string::size_type MAX_SUBJECT_LENGTH = 20;
  message.getHeader("subject", subject);
  if (subject.length() > MAX_SUBJECT_LENGTH) {
    subject.erase(MAX_SUBJECT_LENGTH, string::npos);
    subject += "...";
  }

  cerr << "COMMAND " << name();

  if (stream_file) {
    cerr << " FILE " << stream_file->getPath();
  }

  cerr << " DIGEST " << message.getDigest()
       << " SUBJECT " << subject;

  if (processed) {
    cerr << " PROCESSED";
  } else {
    cerr << " IGNORED";
  }

  cerr << endl;
}
