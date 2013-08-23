///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FilterConfig.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _FilterConfig_h
#define _FilterConfig_h

#include <vector>
#include "TokenSelector.h"

class TokenSelector;

class FilterConfig
{
public:
  enum ScoreMode {
    SCORE_NORMAL,
    SCORE_ORIGINAL,
    SCORE_ALT1,
  };

  FilterConfig();
  ~FilterConfig();

  void setScoreMode(ScoreMode mode);

  ScoreMode getScoreMode() const
  {
    return m_scoreMode;
  }

  void setGoodBias(int value)
  {
    m_goodBias = value;
  }

  int getGoodBias() const
  {
    return m_goodBias;
  }

  void setTermsForScore(int value)
  {
    m_termsForScore = value;
  }

  int getTermsForScore() const
  {
    return m_termsForScore;
  }

  void setMaxWordRepeats(int value)
  {
    m_maxWordRepeats = value;
  }

  int getMaxWordRepeats() const
  {
    return m_maxWordRepeats;
  }

  void setMinWordCount(int value)
  {
    m_defaultMinWordCount = value;
    m_minWordCount = value;
  }

  int getMinWordCount() const
  {
    return m_minWordCount;
  }

  void setNewWordScore(double value)
  {
    m_newWordScore = value;
  }

  double getNewWordScore() const
  {
    return m_newWordScore;
  }

  void setExtendTopTerms(bool value)
  {
    m_extendTopTerms = value;
  }

  bool getExtendTopTerms() const
  {
    return m_extendTopTerms;
  }

  double getSpamThreshold() const
  {
    return m_spamThreshold;
  }

  void setSpamThreshold(double value)
  {
    m_defaultThreshold = value;
    m_spamThreshold = value;
  }

  void setDefaultThreshold(double value)
  {
    m_defaultThreshold = value;
    setScoreMode(m_scoreMode);
  }

  double getDefaultThreshold() const
  {
    return m_defaultThreshold;
  }

  void setMinDistanceForScore(double value)
  {
    m_minDistanceForScore = value;
  }

  double getMinDistanceForScore() const
  {
    return m_minDistanceForScore;
  }

  void setMinArraySize(int value)
  {
    m_minArraySize = value;
  }

  int getMinArraySize() const
  {
    return m_minArraySize;
  }

  void setWaterCounts(bool value)
  {
    m_waterCounts = value;
  }

  bool getWaterCounts() const
  {
    return m_waterCounts;
  }

  int numTokenSelectors() const
  {
    return (int)m_tokenSelectors.size();
  }

  TokenSelector *tokenSelector(int index)
  {
    return m_tokenSelectors[index].ptr();
  }

  void addTokenSelector(OWNED TokenSelector *selector)
  {
    m_tokenSelectors.push_back(make_ref(selector));
  }

  void clearTokenSelectors();

private:
  /// Not implemented.
  FilterConfig(const FilterConfig &);

  /// Not implemented.
  FilterConfig& operator=(const FilterConfig &);

private:
  typedef vector<Ref<TokenSelector> > TokenSelectorVector;

  // settings related to scoring mode
  int m_goodBias;
  int m_minWordCount;
  int m_defaultMinWordCount;
  double m_spamThreshold;
  ScoreMode m_scoreMode;
  TokenSelectorVector m_tokenSelectors;

  // other settings
  int m_termsForScore;
  int m_maxWordRepeats;
  bool m_extendTopTerms;
  double m_newWordScore;
  double m_minDistanceForScore;
  int m_minArraySize;
  bool m_waterCounts;
  double m_defaultThreshold;
};

#endif // _FilterConfig_h
