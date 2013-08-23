///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Buffer.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Buffer_h
#define _Buffer_h

#include "Array.h"

//
// Similar to Array but handles variable length.
//
template<class T>
class Buffer
{
public:
  explicit Buffer(int capacity)
    : m_length(0),
      m_capacity(capacity),
      m_ptr(new T[capacity])
  {
    assert(capacity > 0);
  }

  T &operator[](int index) const
  {
    assert(index >= 0);
    assert(index < m_length);
    return m_ptr[index];
  }

  T *get() const
  {
    return m_ptr.get();
  }

  void clear()
  {
    m_length = 0;
  }

  void append(const T &value)
  {
    assert(m_length <= m_capacity);
    if (m_length >= m_capacity) {
      resize(m_capacity + ((m_capacity < 128) ? 32 : m_capacity / 2));
    }
    assert(m_length < m_capacity);
    m_ptr[m_length++] = value;
  }

  int length() const
  {
    return m_length;
  }

  int capacity() const
  {
    return m_capacity;
  }

  void reset(int new_capacity)
  {
    assert(new_capacity > 0);
    Array<T> tmp(new T[new_capacity]);
    m_length = 0;
    m_capacity = new_capacity;
    m_ptr.set(tmp.release());
  }

  void resize(int new_capacity)
  {
    assert(new_capacity > 0);
    Array<T> tmp(new T[new_capacity]);
    int new_length = (m_length < new_capacity) ? m_length : new_capacity;
    if (m_ptr.isNotNull() && new_length > 0) {
      copy(m_ptr.get(), m_ptr.get() + new_length, tmp.get());
    }
    m_capacity = new_capacity;
    m_length = new_length;
    m_ptr.set(tmp.release());
    assert(m_capacity >= m_length);
  }

private:
  /// Not implemented.
  Buffer(const Buffer &);

  /// Not implemented.
  Buffer& operator=(const Buffer &);

  Array<T> m_ptr;
  int m_length;
  int m_capacity;
};

#endif // _Buffer_h
