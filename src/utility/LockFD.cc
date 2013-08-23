///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: LockFD.cc 272 2007-01-06 19:37:27Z brian $
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
#include "LockFD.h"

LockFD::LockFD(int fd)
  : m_lockFD(fd),
    m_locked(false)
{
}

LockFD::~LockFD()
{
  try {
    release();
  } catch (...) {
  }
}

void LockFD::lock(LockType_t lock_type)
{
  release();

  struct flock buf;
  buf.l_type = (lock_type == SHARED_LOCK) ? F_RDLCK : F_WRLCK;
  buf.l_whence = SEEK_SET;
  buf.l_start = 0;
  buf.l_len = 0;

  int rc = fcntl(m_lockFD, F_SETLKW, &buf);
  if (rc < 0) {
    throw runtime_error(strerror(errno));
  }

  m_locked = true;
}

void LockFD::release()
{
  if (m_locked) {
    struct flock buf;
    buf.l_type = F_UNLCK;
    buf.l_whence = SEEK_SET;
    buf.l_start = 0;
    buf.l_len = 0;

    int rc = fcntl(m_lockFD, F_SETLK, &buf);
    if (rc < 0) {
      throw runtime_error(strerror(errno));
    }

    m_locked = false;
  }
}
