///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FrequencyDB.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FrequencyDB_h
#define _FrequencyDB_h

#include "Message.h"

class CleanupManager;
class DatabaseConfig;
class FrequencyDBImpl;
class WordData;

class FrequencyDB
{
public:
  FrequencyDB(const DatabaseConfig *config);
  ~FrequencyDB();

  bool open(bool read_only);
  void close();
  void flush();

  bool isOpen()
  {
    return m_db.get() != 0;
  }

  void beginTransaction();
  void endTransaction(bool commit);

  void setWordCounts(const string &word,
                     int good_count,
                     int spam_count);

  void addWord(const string &word,
               int good_count,
               int spam_count);

  void addWord(const string &word,
               int good_count,
               int spam_count,
               unsigned long flags);

  bool readWord(const string &word,
		WordData &counts);

  void removeWord(const string &word,
                  int good_count,
                  int spam_count);

  void touchWord(const string &word);

  void getWordCounts(const string &word,
                     int &good_count,
                     int &spam_count);

  void getMessageCounts(int &good_message_count,
                        int &spam_message_count);

  int getTotalMessageCount();

  bool containsMessage(const Message &msg,
                       bool &is_spam);

  int getMessageCount(const Message &msg,
                      bool &is_spam);

  void addMessage(const Message &msg,
                  bool is_spam,
                  bool force_update);

  /**
     Decrement the counts on all terms of this message that are in the database.
     The message must already be in the database or else nothing is done.
  */
  void removeMessage(const Message &msg);

  /**
     Update the timestamp of all terms in this message that are already
     in the database.  Any terms not already in the database are ignored.
  */
  void touchMessage(const Message &msg);

  bool firstWord(string &word,
                 WordData &counts);

  bool nextWord(string &word,
                WordData &counts);

  string getDatabaseType();

  void sweepOutOldTerms(const CleanupManager &cleanman);

  int getMessageCount(bool is_spam)
  {
    int good_count, spam_count;
    getMessageCounts(good_count, spam_count);
    return is_spam ? spam_count : good_count;
  }

  int getMessageCount()
  {
    int good_count, spam_count;
    getMessageCounts(good_count, spam_count);
    return good_count + spam_count;
  }

  bool requestInterrupt();

  bool isInterrupted()
  {
    return m_isInterrupted;
  }

  void throwOnInterrupt();

  void setBusy(bool value)
  {
    m_isBusy = value;
  }

  static const string COUNT_WORD;

private:
  /// Not implemented.
  FrequencyDB(const FrequencyDB &);

  /// Not implemented.
  FrequencyDB& operator=(const FrequencyDB &);

private:
  void adjustWordCounts(const string &word,
                        int delta,
                        bool is_spam);
  OWNED FrequencyDBImpl *createDB(string &filename);

private:
  const DatabaseConfig *m_config;
  Ptr<FrequencyDBImpl> m_db;
  bool m_isInterrupted;
  bool m_isBusy;
};

#endif // _FrequencyDB_h
