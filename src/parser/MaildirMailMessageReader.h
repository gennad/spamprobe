///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MaildirMailMessageReader.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _MaildirMailMessageReader_h
#define _MaildirMailMessageReader_h

#include "File.h"
#include "AbstractMailMessageReader.h"

class MaildirMailMessageReader : public AbstractMailMessageReader
{
public:
  MaildirMailMessageReader(const File &maildir);
  ~MaildirMailMessageReader();

  virtual OWNED MailMessage *readMessage();

  static bool isMaildir(const File &maildir);

private:
  /// Not implemented.
  MaildirMailMessageReader(const MaildirMailMessageReader &);

  /// Not implemented.
  MaildirMailMessageReader& operator=(const MaildirMailMessageReader &);

  typedef vector<File> FileVec;
  typedef FileVec::const_iterator FileIter;

  File m_maildir;
  vector<File> m_children;
  FileIter m_current;
  FileIter m_end;
  Ptr<AbstractMailMessageReader> m_reader;
};

#endif // _MaildirMailMessageReader_h
