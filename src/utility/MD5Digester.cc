///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MD5Digester.cc 272 2007-01-06 19:37:27Z brian $
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

#include "md5.h"
#include "MD5Digester.h"

MD5Digester::MD5Digester()
  : m_running(false),
    m_state(new md5_state_s)
{
}

MD5Digester::~MD5Digester()
{
  if (m_running) {
    md5_byte_t raw_digest[32];
    md5_finish(m_state.get(), raw_digest);
  }
}

void MD5Digester::start()
{
  assert(!m_running);

  m_digest.erase();
  md5_init(m_state.get());
  m_running = true;
}

void MD5Digester::add(const string &value)
{
  assert(m_running);

  if (value.length() > 0) {
    md5_append(m_state.get(), (md5_byte_t *)value.data(), value.length());
  }
}

void MD5Digester::add(const unsigned char *bytes,
                      int num_bytes)
{
  assert(m_running);

  if (num_bytes > 0) {
    md5_append(m_state.get(), (md5_byte_t *)bytes, num_bytes);
  }
}

void MD5Digester::stop()
{
  assert(m_running);

  md5_byte_t raw_digest[32];
  md5_finish(m_state.get(), raw_digest);

  m_digest.erase();
  for (int i = 0; i < 16; ++i) {
    m_digest += hex_digit((raw_digest[i] >> 4) & 0x0f);
    m_digest += hex_digit(raw_digest[i] & 0x0f);
  }

  m_running = false;
}
