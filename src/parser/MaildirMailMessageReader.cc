///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MaildirMailMessageReader.cc 272 2007-01-06 19:37:27Z brian $
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

#include <algorithm>
#include "MailMessage.h"
#include "FileMailMessageReader.h"
#include "MaildirMailMessageReader.h"

MaildirMailMessageReader::MaildirMailMessageReader(const File &maildir)
  : m_maildir(maildir)
{
  File(m_maildir, "cur").children(m_children);
  File(m_maildir, "new").children(m_children);
  sort(m_children.begin(), m_children.end());
  m_current = m_children.begin();
  m_end = m_children.end();
}

MaildirMailMessageReader::~MaildirMailMessageReader()
{
}

bool MaildirMailMessageReader::isMaildir(const File &maildir)
{
  return File(maildir, "cur").isDirectory() && File(maildir, "new").isDirectory();
}

OWNED MailMessage *MaildirMailMessageReader::readMessage()
{
  while (m_reader.isNotNull() || m_current != m_end) {
    if (m_reader.isNotNull()) {
      Ptr<MailMessage> msg(m_reader->readMessage());
      if (msg.isNotNull()) {
        return msg.release();
      }
      m_reader.clear();
    }

    while (m_reader.isNull() && (m_current != m_end)) {
      const File &child = *m_current;
      ++m_current;

      if (child.isFile() && !child.isHidden()) {
        Ptr<FileMailMessageReader> tmp(new FileMailMessageReader);
        tmp->open(child.getPath(), true, true);
        m_reader.set(tmp.release());
      }
    }
  }
  return 0;
}
