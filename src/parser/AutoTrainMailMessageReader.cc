///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AutoTrainMailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MailMessage.h"
#include "AutoTrainMailMessageReader.h"

AutoTrainMailMessageReader::AutoTrainMailMessageReader()
  : m_spamCount(0), m_totalCount(0), m_messageWasSpam(false)
{
  m_readerFactory.setIgnoreFrom(false);
  m_readerFactory.setIgnoreContentLength(false);
}

AutoTrainMailMessageReader::~AutoTrainMailMessageReader()
{
}

void AutoTrainMailMessageReader::addMailboxFile(bool is_spam,
                                                const string &path)
{
  long num_messages = countMessagesInFile(path);
  if (num_messages == 0) {
    cerr << "IGNORING EMPTY FILE " << path << endl;
    return;
  }

  if (is_spam) {
    m_spamPaths.push_back(path);
    m_spamCount += num_messages;
  } else {
    m_goodPaths.push_back(path);
  }
  m_totalCount += num_messages;
}

long AutoTrainMailMessageReader::countMessagesInFile(const string &path)
{
  if (is_verbose) {
    cerr << "counting messages in file " << path << endl;
  }

  Ptr<AbstractMailMessageReader> reader;
  openFile(path, reader);

  long count = 0;
  Ptr<MailMessage> message(reader->readMessage());
  while (message.isNotNull()) {
    ++count;
    message.set(reader->readMessage());
  }

  if (is_verbose) {
    cerr << "file " << path << " contains " << count << " messages." << endl;
  }

  return count;
}

void AutoTrainMailMessageReader::openFile(const string &path,
                                          Ptr<AbstractMailMessageReader> &reader)
{
  reader.set(m_readerFactory.createReader(File(path)));
}

bool AutoTrainMailMessageReader::readMessage(deque<string> &paths,
                                             string &path,
                                             Ptr<AbstractMailMessageReader> &reader,
                                             Ptr<MailMessage> &message)
{
  m_messageFile.setPath(path);
  message.clear();

  if (reader.isNotNull()) {
    message.set(reader->readMessage());
  }

  while (message.isNull() && paths.size() > 0) {
    path = paths.front();
    paths.pop_front();
    openFile(path, reader);
    message.set(reader->readMessage());
    m_messageFile.setPath(path);
  }

  return message.isNotNull();
}

bool AutoTrainMailMessageReader::readMessage(bool is_spam,
                                             Ptr<MailMessage> &message)
{
  m_messageWasSpam = is_spam;

  bool read_message = false;
  if (is_spam) {
    read_message = readMessage(m_spamPaths, m_spamPath, m_spamReader, message);
  } else {
    read_message = readMessage(m_goodPaths, m_goodPath, m_goodReader, message);
  }

  return read_message;
}

OWNED MailMessage *AutoTrainMailMessageReader::readMessage()
{
  Ptr<MailMessage> message;
  bool use_spam = shouldUseSpam();
  if (!readMessage(use_spam, message)) {
    readMessage(!use_spam, message);
  }
  return message.release();
}

bool AutoTrainMailMessageReader::shouldUseSpam()
{
  return (m_totalCount >= 0) && ((random() % m_totalCount) < m_spamCount);
}
