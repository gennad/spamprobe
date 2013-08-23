///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: SpamFilter.cc 272 2007-01-06 19:37:27Z brian $
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

#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include "LockFile.h"
#include "DatabaseConfig.h"
#include "FilterConfig.h"
#include "TokenSelector.h"
#include "ConfigManager.h"
#include "SpamFilter.h"

static const double MIN_PROB = 0.000001;
static const double MAX_PROB = 0.999999;
static const double NEARLY_ZERO = 0.0000001;
static const int MAX_COUNT_FOR_SORT = 20;
static const int COUNT_SORT_DIVISOR = 3;
static const double PROB_SORT_MULTIPLE = 100000.0;
static const double EXT_ARRAY_THRESHOLD = 0.4999;
static const string LOCK_FILENAME("lock");

// train-mode constants
static const int MIN_TRAINING_SET = 1500;
static const FilterConfig::ScoreMode TRAIN_SCORE_MODE = FilterConfig::SCORE_NORMAL;
static const int MAX_SINGLE_MESSAGE_CLASSIFY = 25;

typedef vector<Token *>::size_type vecsize_t;
typedef vector<Token *>::iterator veciter_t;

SpamFilter::SpamFilter(ConfigManager *config)
  : m_config(config),
    m_db(config->databaseConfig())
{
}

SpamFilter::~SpamFilter()
{
  close();
}

void SpamFilter::lock(const File &raw_db_file,
                      bool read_only)
{
  File db_file(raw_db_file.getPath());
  File lock_file(db_file.parent(), LOCK_FILENAME);
  m_lock.set(new LockFile(lock_file));
  m_lock->lock(read_only ? LockFD::SHARED_LOCK : LockFD::EXCLUSIVE_LOCK);
  if (is_debug) {
    cerr << "LOCKED FILE " << lock_file.getPath() << endl;
  }
}

void SpamFilter::open(bool read_only)
{
  lock(File(m_config->databaseConfig()->privateFilename()), read_only);
  if (!m_db.open(read_only)) {
    throw runtime_error("unable to open words database");
  }
}

void SpamFilter::close(bool abandon_writes)
{
  if (m_db.isOpen()) {
    if (!abandon_writes) {
      m_db.flush();
    }
    m_db.close();
  }
  m_lock.clear();
}

double SpamFilter::computeRatio(double count,
                                double total_count)
{
  if (count == 0) {
    return NEARLY_ZERO;
  } else {
    double score = count / total_count;
    return max(MIN_PROB, min(MAX_PROB, score));
  }
}

double SpamFilter::scoreTerm(int good_count,
                             int spam_count,
                             int good_message_count,
                             int spam_message_count)
{
  good_count *= m_config->filterConfig()->getGoodBias();

  int count = good_count + spam_count;

  double score;
  if (count >= m_config->filterConfig()->getMinWordCount()) {
    double good_ratio = computeRatio(good_count, good_message_count);
    double spam_ratio = computeRatio(spam_count, spam_message_count);
    score = computeRatio(spam_ratio, good_ratio + spam_ratio);
  } else {
    score = m_config->filterConfig()->getNewWordScore();
  }

  return score;
}

// Returns "within document frequency" for sort.  Uses a limit on max
// value for sanity and also uses a ratio of the count to keep small
// differences (3 vs. 2) from having a disproportionate impact on the
// sort.
inline long wdf_for_sort(int count)
{
  return min(MAX_COUNT_FOR_SORT, count / COUNT_SORT_DIVISOR);
}

// Returns probability rounded to 5 decimal places to "band" results for sorting.
// This prevents tiny differences from having excessive impact on results.
inline double rounded_prob(double prob)
{
  return floor(prob * PROB_SORT_MULTIPLE);
}

void SpamFilter::scoreToken(Token *tok,
                            int good_message_count,
                            int spam_message_count)
{
  assert(tok->getCount() > 0);

  int good_count, spam_count;
  m_db.getWordCounts(tok->getWord(), good_count, spam_count);
  tok->setDBGoodCount(good_count);
  tok->setDBSpamCount(spam_count);
  if (tok->getDBTotalCount() < 0 && is_debug) {
    cerr << "WARNING: token " << tok->getWord() << " has negative count" << endl;
  }

  double score = scoreTerm(tok->getDBGoodCount(), tok->getDBSpamCount(), good_message_count, spam_message_count);
  assert(score >= MIN_PROB);
  assert(score <= MAX_PROB);
  tok->setScore(score);

  long sort_count = ((long)rounded_prob(abs(score - 0.5))) << 6;
  sort_count |= (wdf_for_sort(tok->getCount()) & 0x1f) << 1;
  if (score < 0.5) {
    sort_count |= 1;
  }
  assert(sort_count >= 0);
  tok->setSortCount(sort_count);

  double tie_break_ratio;
  if (score > 0.5) {
    tie_break_ratio = computeRatio(spam_count, spam_message_count);
  } else {
    tie_break_ratio = computeRatio(good_count, good_message_count);
  }
  tok->setTieBreakCount((long)(tie_break_ratio / MIN_PROB));
}

void SpamFilter::scoreTokens(const Message &msg)
{
  int good_message_count, spam_message_count;
  m_db.getMessageCounts(good_message_count, spam_message_count);

  for (int i = 0; i < msg.getTokenCount(); ++i) {
    Token *tok = msg.getToken(i);
    assert(tok);
    scoreToken(tok, good_message_count, spam_message_count);
  }
}

static int token_sort_descending(Token * const &t1, Token * const &t2)
{
  int answer;
  if (t2->getSortCount() != t1->getSortCount()) {
    answer = t2->getSortCount() < t1->getSortCount();
  } else if (t2->getTieBreakCount() != t1->getTieBreakCount()) {
    answer = t2->getTieBreakCount() < t1->getTieBreakCount();
  } else {
    answer = t1->getWord() < t2->getWord();
  }
  return answer;
}

// Remove tokens below min distance from mean but be sure that we have
// at least the number of terms required by the min_array_size config option.
void SpamFilter::removeTokensBelowMinDistance(vector<Token *> &tokens)
{
  double min_distance = m_config->filterConfig()->getMinDistanceForScore();
  if (min_distance > 0.0) {
    const int min_size = m_config->filterConfig()->getMinArraySize();
    int array_size = 0;
    for (veciter_t i = tokens.begin(), limit = tokens.end(); i != limit; ++i, ++array_size) {
      const Token *tok = *i;
      if ((tok->getDistanceFromMean() < min_distance) && (array_size >= min_size)) {
        tokens.erase(i, tokens.end());
        break;
      }
    }
  }
}

void SpamFilter::sortTokens(vector<Token *> &tokens)
{
  bool need_whole_array = m_config->filterConfig()->getExtendTopTerms() || tokens.size() <= (vecsize_t)m_config->filterConfig()->getTermsForScore();
  veciter_t sort_end;
  if (need_whole_array) {
    sort_end = tokens.end();
  } else {
    sort_end = tokens.begin() + (vecsize_t)m_config->filterConfig()->getTermsForScore();
  }

  assert((tokens.end() - tokens.begin()) >= (sort_end - tokens.begin()));
  partial_sort(tokens.begin(), sort_end, tokens.end(), token_sort_descending);

  if (!need_whole_array) {
    tokens.erase(sort_end, tokens.end());
  }
}

void SpamFilter::dumpSortedTokens(vector<Token *> &tokens)
{
  if (is_debug) {
    for (vector<Token *>::const_iterator i = tokens.begin(); i != tokens.end(); ++i) {
      Token *tok = *i;
      cerr << "SORTED " << tok->getWord()
           << " count " << tok->getCount()
           << " score " << tok->getScore()
           << " dist " << tok->getDistanceFromMean()
           << " good " << tok->getDBGoodCount()
           << " spam " << tok->getDBSpamCount()
           << " tiebreak " << tok->getTieBreakCount()
           << endl;
    }
  }
}

void SpamFilter::computeScoreProducts(Message &msg,
                                      TokenSelector *selector,
                                      double &spamness,
                                      double &goodness,
                                      double &num_terms)
{
  vector<Token *> tokens;
  tokens.reserve(msg.getTokenCount());
  selector->selectTokens(msg, tokens);
  sortTokens(tokens);
  removeTokensBelowMinDistance(tokens);
  dumpSortedTokens(tokens);

  msg.clearTopTokens();

  int max_word_repeats = m_config->filterConfig()->getMaxWordRepeats();
  int max_terms = m_config->filterConfig()->getTermsForScore();

  if (m_config->filterConfig()->getWaterCounts()) {
    max_terms = max(15, (int)tokens.size() / 25);
    max_word_repeats = max(1, max_terms / 15);
  }

  goodness = 1.0;
  spamness = 1.0;
  num_terms = 0;
  for (vector<Token *>::const_iterator i = tokens.begin(); i != tokens.end(); ++i) {
    Token *tok = *i;

    if ((num_terms >= max_terms) &&
        (!m_config->filterConfig()->getExtendTopTerms() || (tok->getDistanceFromMean() < EXT_ARRAY_THRESHOLD))) {
      break;
    }

    int count = tok->getCount();
    double score = tok->getScore();

    msg.addTopToken(tok);

    assert(count > 0);

    int times = min(max_word_repeats, count);
    if (is_debug) {
      cerr << "** TOKEN " << tok->getWord() << ": score " << score << ": times " << times << endl;
    }

    num_terms += times;
    while (times-- > 0) {
      spamness = spamness * score;
      goodness = goodness * (1.0 - score);
    }
  }
}

//
// Paul Graham's original formula as outlined in A Plan For Spam.
// Yields excellent results but nearly all messages score as 0 or 1
//
double SpamFilter::originalScoreMessage(Message &msg,
                                        TokenSelector *selector)
{
  double spamness, goodness, num_terms;
  computeScoreProducts(msg, selector, spamness, goodness, num_terms);

  double score = spamness / (spamness + goodness);
  if (is_nan(score)) {
    score = 0.5;
  }

  if (is_debug) {
    cerr << "** SPAMNESS " << spamness << ": GOODNESS " << goodness << ": SCORE "
         << score << endl;
  }

  return score;
}

//
// Robinson's simple algorithm.  Refer to his article for details:
// http://radio.weblogs.com/0101454/stories/2002/09/16/spamDetection.html
//
double SpamFilter::alt1ScoreMessage(Message &msg,
                                    TokenSelector *selector)
{
  double spamness, goodness, num_terms;
  computeScoreProducts(msg, selector, spamness, goodness, num_terms);

  double p = 1.0 - pow(goodness, 1.0 / num_terms);
  double q = 1.0 - pow(spamness, 1.0 / num_terms);
  double s = (p - q) / (p + q);
  s = (s + 1.0) / 2.0;

  if (is_debug) {
    cerr << "SCORE P " << p << ": Q " << q << ": SCORE " << s << endl;
  }

  return s;
}

//
// Paul's formula modified to use the nth root of the products.  Gives
// same results as Paul's formula but spreads out scores more evenly.
// Gives a better distribution than Robinson's simple formula.
//
double SpamFilter::normalScoreMessage(Message &msg,
                                      TokenSelector *selector)
{
  double spamness, goodness, num_terms;
  computeScoreProducts(msg, selector, spamness, goodness, num_terms);

  spamness = pow(spamness, 1.0 / num_terms);
  goodness = pow(goodness, 1.0 / num_terms);

  double score = spamness / (spamness + goodness);
  if (is_nan(score)) {
    score = 0.5;
  }

  if (is_debug) {
    cerr << "** SPAMNESS " << spamness << ": GOODNESS " << goodness << ": SCORE "
         << score << endl;
  }

  return score;
}

SpamFilter::Score SpamFilter::scoreMessage(Message &msg)
{
  scoreTokens(msg);

  double score_value = 0;
  for (int i = 0, limit = m_config->filterConfig()->numTokenSelectors(); i < limit; ++i) {
    score_value += scoreMessage(msg, m_config->filterConfig()->tokenSelector(i));
  }
  score_value = score_value / ((double)m_config->filterConfig()->numTokenSelectors());

  return Score(score_value, m_config->filterConfig()->getScoreMode(), score_value >= m_config->filterConfig()->getSpamThreshold());
}

double SpamFilter::scoreMessage(Message &msg,
                                TokenSelector *selector)
{
  double score_value;
  switch (m_config->filterConfig()->getScoreMode()) {
  case FilterConfig::SCORE_ALT1:
    score_value = alt1ScoreMessage(msg, selector);
    break;

  case FilterConfig::SCORE_ORIGINAL:
    score_value = originalScoreMessage(msg, selector);
    break;

  default:
    assert(m_config->filterConfig()->getScoreMode() == FilterConfig::SCORE_NORMAL);
    score_value = normalScoreMessage(msg, selector);
    break;
  }

  return score_value;
}

SpamFilter::Score SpamFilter::scoreMessage(Message &msg,
                                           FilterConfig::ScoreMode mode)
{
  if (m_config->filterConfig()->getScoreMode() == mode) {
    return scoreMessage(msg);
  }

  FilterConfig::ScoreMode old_mode = m_config->filterConfig()->getScoreMode();
  m_config->filterConfig()->setScoreMode(mode);
  Score score = scoreMessage(msg);
  m_config->filterConfig()->setScoreMode(old_mode);
  return score;
}

void SpamFilter::ensureGoodMessage(const Message &msg,
                                   bool force_update)
{
  bool is_spam = false;
  if (m_db.containsMessage(msg, is_spam) && is_spam) {
    // remove from database if previously classified as spam
    m_db.removeMessage(msg);
  }
  m_db.addMessage(msg, false, force_update);
}

void SpamFilter::ensureSpamMessage(const Message &msg,
                                   bool force_update)
{
  bool is_spam = false;
  if (m_db.containsMessage(msg, is_spam) && !is_spam) {
    // remove from database if previously classified as good
    m_db.removeMessage(msg);
  }
  m_db.addMessage(msg, true, force_update);
}

bool SpamFilter::classifyMessage(Message &msg,
                                 bool is_spam)
{
  if (is_spam) {
    ensureSpamMessage(msg, false);
  } else {
    ensureGoodMessage(msg, false);
  }

  Score score = scoreMessage(msg);
  while (!score.isConfident(is_spam)) {
    bool is_spam_in_db = false;
    int message_count = getDB()->getMessageCount(msg, is_spam_in_db);
    assert(!is_spam_in_db == !is_spam);

    if (message_count >= MAX_SINGLE_MESSAGE_CLASSIFY) {
      break;
    }

    if (is_debug) {
      cerr << "ANOTHER ATTEMPT to classify message " << msg.getDigest()
           << " SCORE " << score.getValue() << endl;
    }

    if (is_spam) {
      ensureSpamMessage(msg, true);
    } else {
      ensureGoodMessage(msg, true);
    }

    score = scoreMessage(msg);
  }

  return is_spam;
}


bool SpamFilter::trainOnMessage(Message &msg,
                                const SpamFilter::Score &score,
                                bool is_spam,
                                bool update_timestamps)
{
  int good_count, spam_count;
  getDB()->getMessageCounts(good_count, spam_count);

  bool counts_unbalanced, below_min_count;
  if (is_spam) {
    counts_unbalanced = spam_count < good_count;
    below_min_count   = spam_count < MIN_TRAINING_SET;
  } else {
    counts_unbalanced = good_count < spam_count;
    below_min_count   = good_count < MIN_TRAINING_SET;
  }

  if (below_min_count || counts_unbalanced || (score.isSpam() != is_spam) ||
      !scoreMessageIfWrongMode(score, msg, TRAIN_SCORE_MODE).isConfident(is_spam)) {
    // train on the message if it was wrong or not a definite decision
    classifyMessage(msg, is_spam);
  } else if (update_timestamps) {
    // Otherwise just update term time stamps to keep them from expiring.
    // If we didn't do this terms that appear frequently in messages that
    // aren't classified would expire from the database.
    getDB()->touchMessage(msg);
  }

  return is_spam;
}
