///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDB.cc 272 2007-01-06 19:37:27Z brian $
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
#include <fcntl.h>
#include "DatabaseConfig.h"
#include "File.h"
#include "InterruptedException.h"
#include "WordData.h"
#include "DatabaseConfig.h"
#include "FrequencyDBImpl.h"
#include "FrequencyDBImpl_null.h"
#include "FrequencyDB.h"

/// defined here to avoid having an otherwise empty WordData.cc file - yeah I know
unsigned long WordData::s_today = 0;

static const string DIGEST_PREFIX("__MD5__");
const string FrequencyDB::COUNT_WORD("__COUNT__");

const int FrequencyDBImpl::SHARED_DB_MODE = 0666;
const int FrequencyDBImpl::PRIVATE_DB_MODE = 0600;

class InterruptTest
{
public:
  InterruptTest(FrequencyDB *db)
    : m_db(db)
  {
    m_db->setBusy(true);
  }

  ~InterruptTest()
  {
    m_db->setBusy(false);
    m_db->throwOnInterrupt();
  }

private:
  FrequencyDB *m_db;
};

FrequencyDB::FrequencyDB(const DatabaseConfig *config)
  : m_config(config),
    m_isInterrupted(false),
    m_isBusy(false),
    m_db(new FrequencyDBImpl_null)
{
}

FrequencyDB::~FrequencyDB()
{
  close();
}

bool FrequencyDB::requestInterrupt()
{
  if (m_isInterrupted || !m_isBusy) {
    return true;
  }
  m_isInterrupted = true;
  cerr << "INTERRUPT SCHEDULED: TRY AGAIN AFTER SEVERAL SECONDS IF SHUTDOWN FAILS" << endl;
  return false;
}

void FrequencyDB::throwOnInterrupt()
{
  if (m_isInterrupted) {
    m_isInterrupted = false;
    throw InterruptedException();
  }
}

bool FrequencyDB::open(bool read_only)
{
  close();

  WordData::setTodayDate();

  m_db.set(m_config->createDatabaseImpl(read_only));
  if (m_db.isNull()) {
    m_db.set(new FrequencyDBImpl_null);
    return false;
  } else {
    return true;
  }
}

void FrequencyDB::close()
{
  // don't want to cause a loop in the int_test destructor
  m_isInterrupted = false;

  InterruptTest int_test(this);
  m_db.set(new FrequencyDBImpl_null);
}

void FrequencyDB::flush()
{
  assert(m_db.get());

  InterruptTest int_test(this);
  m_db->flush();
}

void FrequencyDB::beginTransaction()
{
  assert(m_db.get());

  InterruptTest int_test(this);
  m_db->beginTransaction();
}

void FrequencyDB::endTransaction(bool commit)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  m_db->endTransaction(commit);
}

void FrequencyDB::setWordCounts(const string &word,
                                int good_count,
                                int spam_count)
{
  assert(m_db.get());
  assert(good_count >= 0 && spam_count >= 0);

  WordData counts(good_count, spam_count);
  counts.adjustDate();
  InterruptTest int_test(this);
  m_db->writeWord(word, counts);
}

void FrequencyDB::touchWord(const string &word)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  WordData counts;
  if (m_db->readWord(word, counts)) {
    counts.adjustDate();
    m_db->writeWord(word, counts);
  }
}

void FrequencyDB::addWord(const string &word,
                          int good_count,
                          int spam_count)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  WordData counts;
  m_db->readWord(word, counts);

  counts.adjustGoodCount(good_count);
  counts.adjustSpamCount(spam_count);
  counts.adjustDate();

  m_db->writeWord(word, counts);
}

void FrequencyDB::addWord(const string &word,
                          int good_count,
                          int spam_count,
                          unsigned long flags)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  WordData counts;
  bool exists = m_db->readWord(word, counts);

  counts.adjustGoodCount(good_count);
  counts.adjustSpamCount(spam_count);
  if (exists) {
    counts.adjustDate();
  } else {
    counts.setFlags(flags);
  }

  m_db->writeWord(word, counts);
}

bool FrequencyDB::readWord(const string &word,
                           WordData &counts)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  return m_db->readWord(word, counts);
}

void FrequencyDB::removeWord(const string &word,
                             int good_count,
                             int spam_count)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  addWord(word, -good_count, -spam_count);
}

void FrequencyDB::getWordCounts(const string &word,
                                int &good_count,
                                int &spam_count)
{
  WordData counts;
  readWord(word, counts);
  good_count = counts.goodCount();
  spam_count = counts.spamCount();
}

void FrequencyDB::getMessageCounts(int &good_message_count,
                                   int &spam_message_count)
{
  getWordCounts(COUNT_WORD, good_message_count, spam_message_count);
}

int FrequencyDB::getTotalMessageCount()
{
  InterruptTest int_test(this);
  int good_message_count, spam_message_count;
  getWordCounts(COUNT_WORD, good_message_count, spam_message_count);
  return good_message_count + spam_message_count;
}

int FrequencyDB::getMessageCount(const Message &msg,
                                 bool &is_spam)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  WordData counts;
  if (!m_db->readWord(DIGEST_PREFIX + msg.getDigest(), counts)) {
    is_spam = false;
    return 0;
  }

  assert(counts.goodCount() >= 0 && counts.spamCount() >= 0);
  assert(!(counts.goodCount() > 0 && counts.spamCount() > 0));

  is_spam = (counts.spamCount() > 0);
  return is_spam ? counts.spamCount() : counts.goodCount();
}

bool FrequencyDB::containsMessage(const Message &msg,
                                  bool &is_spam)
{
  return getMessageCount(msg, is_spam) > 0;
}

void FrequencyDB::adjustWordCounts(const string &word,
                                   int delta,
                                   bool is_spam)
{
  assert(m_db.get());

  if (is_spam) {
    addWord(word, 0, delta);
  } else {
    addWord(word, delta, 0);
  }
}

void FrequencyDB::touchMessage(const Message &msg)
{
  assert(m_db.get());
  assert(msg.getDigest().length() > 0);

  for (int i = 0; i < msg.getTokenCount(); ++i) {
    const Token *tok = msg.getToken(i);
    touchWord(tok->getWord());
  }

  if (is_debug) {
    string msg_id;
    cerr << "Updated terms from message " << msg.getID(msg_id)
         << "/" << msg.getDigest()
         << " in database"
         << endl;
  }
}

void FrequencyDB::addMessage(const Message &msg,
                             bool new_is_spam,
                             bool force_update)
{
  assert(m_db.get());
  assert(msg.getDigest().length() > 0);

  bool already_is_spam = false;
  bool already_exists = containsMessage(msg, already_is_spam);

  assert(!already_exists || (!new_is_spam == !already_is_spam));

  if (already_exists) {
    assert(!already_is_spam == !new_is_spam);

    if (!force_update) {
      // message already counted, do nothing
      return;
    }

    // force_update causes us to pretend it doesn't already exist so
    // we wind up adding its terms again even though they are already counted
  }

  for (int i = 0; i < msg.getTokenCount(); ++i) {
    const Token *tok = msg.getToken(i);
    adjustWordCounts(tok->getWord(), tok->getCount(), new_is_spam);
  }
  adjustWordCounts(DIGEST_PREFIX + msg.getDigest(), 1, new_is_spam);

  if (!already_exists) {
    adjustWordCounts(COUNT_WORD, 1, new_is_spam);
  }

  if (is_debug) {
    string msg_id;
    cerr << "Updated message " << msg.getID(msg_id)
         << "/" << msg.getDigest()
         << " in database as "
         << (new_is_spam ? "spam." : "good.")
         << endl;
  }
}

void FrequencyDB::removeMessage(const Message &msg)
{
  assert(m_db.get());
  assert(msg.getDigest().length() > 0);

  bool is_spam = false;
  int message_count = getMessageCount(msg, is_spam);
  if (message_count == 0) {
    // not in database
    return;
  }

  for (int i = 0; i < msg.getTokenCount(); ++i) {
    const Token *tok = msg.getToken(i);
    adjustWordCounts(tok->getWord(), -message_count * tok->getCount(), is_spam);
  }
  adjustWordCounts(DIGEST_PREFIX + msg.getDigest(), -message_count, is_spam);
  adjustWordCounts(COUNT_WORD, -1, is_spam);

  assert(getMessageCount(msg, is_spam) == 0);

  if (is_debug) {
    string msg_id;
    cerr << "Removed message " << msg.getID(msg_id)
         << "/" << msg.getDigest()
         << " from database." << endl;
  }
}

bool FrequencyDB::firstWord(string &word,
                            WordData &counts)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  return m_db->firstWord(word, counts);
}

bool FrequencyDB::nextWord(string &word,
                           WordData &counts)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  return m_db->nextWord(word, counts);
}

string FrequencyDB::getDatabaseType()
{
  InterruptTest int_test(this);
  return m_db.get() ? m_db->getDatabaseType() : "unknown";
}

void FrequencyDB::sweepOutOldTerms(const CleanupManager &cleanman)
{
  assert(m_db.get());

  InterruptTest int_test(this);
  return m_db->sweepOutOldTerms(cleanman);
}
