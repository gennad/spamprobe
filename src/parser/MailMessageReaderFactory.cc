///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MailMessageReaderFactory.cc 272 2007-01-06 19:37:27Z brian $
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

#include <stdexcept>
#include "File.h"
#include "AbstractMailMessageReader.h"
#include "FileMailMessageReader.h"
#include "MaildirMailMessageReader.h"
#include "StreamMailMessageReader.h"
#include "MailMessageReaderFactory.h"

MailMessageReaderFactory::MailMessageReaderFactory()
  : m_ignoreFrom(false),
    m_ignoreContentLength(true)
{
}

// Any Maildir directory is valid.
// Just about any file is acceptable except for dot files inside
// of a Maildir directory.  The latter restriction prevents us from
// processing password files, IMAPD cache files, etc.
bool MailMessageReaderFactory::isAcceptableFile(const File &file)
{
  if (file.isSelfOrParent()) {
    return false;
  } else if (file.isFile()) {
    return !(file.isHidden() && MaildirMailMessageReader::isMaildir(file.parent()));
  } else {
    return MaildirMailMessageReader::isMaildir(file);
  }
}

OWNED AbstractMailMessageReader *MailMessageReaderFactory::createReader(const File &file)
{
  if (file.isDirectory()) {
    if (!MaildirMailMessageReader::isMaildir(file)) {
      throw runtime_error(string("not a maildir: ") + file.getPath());
    }
    return new MaildirMailMessageReader(file);
  } else {
    Ptr<FileMailMessageReader> reader(new FileMailMessageReader);
    reader->open(file.getPath(), m_ignoreFrom, m_ignoreContentLength);
    return reader.release();
  }
}

OWNED AbstractMailMessageReader *MailMessageReaderFactory::createStdinReader()
{
  Ptr<StreamMailMessageReader> reader(new StreamMailMessageReader);
  reader->setStream(&cin, false, m_ignoreFrom, m_ignoreContentLength);
  return reader.release();
}
