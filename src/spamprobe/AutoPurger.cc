///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AutoPurger.cc 272 2007-01-06 19:37:27Z brian $
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

#include "CleanupManager.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "FrequencyDB.h"
#include "SpamFilter.h"
#include "AutoPurger.h"

AutoPurger::AutoPurger(const ConfigManager &config,
                       SpamFilter &filter,
                       int junk_count)
  : m_cleaner(new CleanupManager(junk_count, -1)), // -1 forces today's to be removed too
    m_db(filter.getDB()),
    m_messageCount(0),
    m_messagesPerPurge(config.commandConfig()->messagesPerPurge())
{
}

AutoPurger::~AutoPurger()
{
}

void AutoPurger::disable()
{
  m_messagesPerPurge = 0;
}

void AutoPurger::processedMessage()
{
  if (m_messagesPerPurge > 0) {
    ++m_messageCount;
    if (m_messageCount % m_messagesPerPurge == 0) {
      m_db->sweepOutOldTerms(*m_cleaner);
    }
  }
}

void AutoPurger::finish()
{
  if (m_messagesPerPurge > 0) {
    if (m_messageCount % m_messagesPerPurge != 0) {
      m_db->sweepOutOldTerms(*m_cleaner);
    }
  }
}
