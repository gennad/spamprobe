///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: FilterConfig.cc 272 2007-01-06 19:37:27Z brian $
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

#include "TokenSelector.h"
#include "FilterConfig.h"

FilterConfig::FilterConfig()
  : m_termsForScore(27),
    m_defaultMinWordCount(5),
    m_maxWordRepeats(2),
    m_extendTopTerms(false),
    m_newWordScore(0.3),
    m_minDistanceForScore(0.0),
    m_minArraySize(0),
    m_waterCounts(false),
    m_defaultThreshold(0.6)
{
  setScoreMode(SCORE_NORMAL);
  m_tokenSelectors.push_back(make_ref(new TokenSelector()));
}

FilterConfig::~FilterConfig()
{
}

void FilterConfig::setScoreMode(ScoreMode mode)
{
  switch (mode) {
  case SCORE_ALT1:
    m_goodBias = 1;
    m_minWordCount = 1;
    m_spamThreshold = 0.6;
    break;

  case SCORE_ORIGINAL:
    m_goodBias = 2;
    m_minWordCount = m_defaultMinWordCount;
    m_spamThreshold = 0.9;
    break;

  default:
    assert(mode == SCORE_NORMAL);
    m_goodBias = 2;
    m_minWordCount = m_defaultMinWordCount;
    m_spamThreshold = m_defaultThreshold;
    break;
  }

  m_scoreMode = mode;
}

void FilterConfig::clearTokenSelectors()
{
  m_tokenSelectors.clear();
}
