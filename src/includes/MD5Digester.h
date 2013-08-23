///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MD5Digester.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _MD5Digester_h
#define _MD5Digester_h

#include "util.h"

struct md5_state_s;

class MD5Digester
{
public:
  MD5Digester();

  ~MD5Digester();

  void start();

  void add(const string &value);
  void add(const unsigned char *bytes, int num_bytes);

  void stop();

  const bool isRunning() const
  {
    return m_running;
  }

  const string &asString() const
  {
    assert(!m_running);

    return m_digest;
  }

private:
  /// Not implemented.
  MD5Digester(const MD5Digester &);

  /// Not implemented.
  MD5Digester& operator=(const MD5Digester &);

private:
  string m_digest;
  Ptr<md5_state_s> m_state;
  bool m_running;
};

#endif // _MD5Digester_h
