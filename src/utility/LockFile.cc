///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: LockFile.cc 272 2007-01-06 19:37:27Z brian $
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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include "LockFile.h"

LockFile::LockFile(const File &file)
  : m_file(file)
{
}

LockFile::~LockFile()
{
  try {
    release();
  } catch (...) {
  }
}

void LockFile::lock(LockType_t lock_type,
		    int lock_mode)
{
  release();

  int open_flag = O_CREAT | ((lock_type == LockFD::SHARED_LOCK) ? O_RDONLY : O_RDWR);
  int fd = ::open(m_file, open_flag, lock_mode);
  if (fd < 0) {
    throw runtime_error(strerror(errno));
  }

  m_lock.set(new LockFD(fd));
  m_lock->lock(lock_type);
}

void LockFile::release()
{
  if (m_lock.get()) {
    m_lock->release();
    close(m_lock->getFD());
    m_lock.clear();
  }
}
