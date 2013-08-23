///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: SpamFilter.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _SpamFilter_h
#define _SpamFilter_h

#include "File.h"
#include "FrequencyDB.h"
#include "FilterConfig.h"

class ConfigManager;
class LockFile;
class TokenSelector;

class SpamFilter
{
public:
  SpamFilter(ConfigManager *config);
  ~SpamFilter();

  class Score
  {
  public:
    Score()
      :m_score(0.5),
       m_mode(FilterConfig::SCORE_NORMAL),
       m_isSpam(false)
    {
    }

    Score(double _score,
          FilterConfig::ScoreMode _mode,
          bool is_spam)
      : m_score(_score),
        m_mode(_mode),
        m_isSpam(is_spam)
    {
    }

    double getValue() const
    {
      return m_score;
    }

    FilterConfig::ScoreMode getMode()
    {
      return m_mode;
    }

    bool isSpam() const
    {
      return m_isSpam;
    }

    bool isGood() const
    {
      return !m_isSpam;
    }

    bool isConfident(bool should_be_spam)
    {
      switch (m_mode) {
      case FilterConfig::SCORE_ALT1:
        return should_be_spam ? (m_score >= 0.85) : (m_score <= 0.15);

      default:
        assert(m_mode == FilterConfig::SCORE_ORIGINAL || m_mode == FilterConfig::SCORE_NORMAL);
        return should_be_spam ? (m_score >= 0.995) : (m_score <= 0.005);
      }
    }

  private:
    double m_score;
    FilterConfig::ScoreMode m_mode;
    bool m_isSpam;
  };

  void open(bool read_only);

  void close(bool abandon_writes = false);

  void flush()
  {
    m_db.flush();
  }

  FrequencyDB *getDB()
  {
    return &m_db;
  }

  bool isNewMessage(const Message &msg)
  {
    bool is_spam;
    return !m_db.containsMessage(msg, is_spam);
  }

  void ensureGoodMessage(const Message &msg,
                         bool force_update);

  void ensureSpamMessage(const Message &msg,
                         bool force_update);

  void removeMessage(const Message &msg)
  {
    m_db.removeMessage(msg);
  }

  Score scoreMessage(Message &msg);

  Score scoreMessage(Message &msg,
                     FilterConfig::ScoreMode mode);

  Score scoreMessageIfWrongMode(Score current_score,
                                Message &msg,
                                FilterConfig::ScoreMode mode)
  {
    return (current_score.getMode() == mode) ? current_score : scoreMessage(msg, mode);
  }

  bool isSpam(Message &msg)
  {
    return scoreMessage(msg).isSpam();
  }

  double scoreTerm(int good_count,
                   int spam_count,
                   int good_message_count,
                   int spam_message_count);

  bool classifyMessage(Message &msg,
                       bool is_spam);

  bool trainOnMessage(Message &msg,
                      const SpamFilter::Score &score,
                      bool is_spam,
                      bool update_timestamps);

private:
  void dumpSortedTokens(vector<Token *> &tokens);
  void sortTokens(vector<Token *> &tokens);
  void removeTokensBelowMinDistance(vector<Token *> &tokens);
  void scoreTokens(const Message &msg);
  double computeRatio(double count,
                      double total_count);
  void computeScoreProducts(Message &msg,
                            TokenSelector *selector,
                            double &spamness,
                            double &goodness,
                            double &num_terms);
  void scoreToken(Token *tok,
                  int good_message_count,
                  int spam_message_count);
  void getSortedTokens(const Message &msg,
                       TokenSelector *selector,
                       int max_tokens,
                       vector<Token *> &tokens);
  double scoreMessage(Message &msg,
                      TokenSelector *selector);

  double normalScoreMessage(Message &msg, TokenSelector *selector);
  double alt1ScoreMessage(Message &msg, TokenSelector *selector);
  double originalScoreMessage(Message &msg, TokenSelector *selector);
  void lock(const File &db_file,
            bool read_only);

private:
  /// Not implemented.
  SpamFilter(const SpamFilter &);

  /// Not implemented.
  SpamFilter& operator=(const SpamFilter &);

private:
  ConfigManager *m_config;
  FrequencyDB m_db;
  Ptr<LockFile> m_lock;
};

#endif // _SpamFilter_h
