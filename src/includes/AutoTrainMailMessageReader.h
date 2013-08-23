///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AutoTrainMailMessageReader.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _AutoTrainMessageReader_h
#define _AutoTrainMessageReader_h

#include <deque>
#include <fstream>
#include "File.h"
#include "AbstractMailMessageReader.h"
#include "MailMessageReaderFactory.h"

class AutoTrainMailMessageReader : public AbstractMailMessageReader
{
public:
  AutoTrainMailMessageReader();
  ~AutoTrainMailMessageReader();

  void addMailboxFile(bool is_spam, const string &path);

  OWNED MailMessage *readMessage();

  bool messageWasSpam()
  {
    return m_messageWasSpam;
  }

  const File &messageFile()
  {
    return m_messageFile;
  }

private:
  /// Not implemented.
  AutoTrainMailMessageReader(const AutoTrainMailMessageReader &);

  /// Not implemented.
  AutoTrainMailMessageReader& operator=(const AutoTrainMailMessageReader &);

private:
  bool shouldUseSpam();
  long countMessagesInFile(const string &path);
  void openFile(const string &path,
                Ptr<AbstractMailMessageReader> &reader);
  bool readMessage(deque<string> &paths,
                   string &path,
                   Ptr<AbstractMailMessageReader> &reader,
                   Ptr<MailMessage> &message);
  bool readMessage(bool is_spam,
                   Ptr<MailMessage> &message);

private:
  long m_spamCount;
  long m_totalCount;
  deque<string> m_spamPaths;
  deque<string> m_goodPaths;
  MailMessageReaderFactory m_readerFactory;

  bool m_messageWasSpam;
  File m_messageFile;
  string m_spamPath;
  string m_goodPath;
  Ptr<AbstractMailMessageReader> m_spamReader;
  Ptr<AbstractMailMessageReader> m_goodReader;
};

#endif // _AutoTrainMailMessageReader_h
