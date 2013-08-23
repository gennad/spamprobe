///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Ref.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _Ref_h
#define _Ref_h

#include <cassert>

template<typename T> class RefBase
{
public:
  void clear()
  {
    release();
    m_count = 0;
    m_ptr = 0;
  }

  bool equals(const RefBase<T> &other) const
  {
    return m_ptr == other.m_ptr;
  }

  bool isNull() const
  {
    return m_ptr == 0;
  }

  bool isNotNull() const
  {
    return m_ptr != 0;
  }

  int *count() const
  {
    return m_count;
  }

  T *ptr() const
  {
    return m_ptr;
  }

protected:
  RefBase()
    : m_ptr(0), m_count(0)
  {
  }

  RefBase(T *ptr);

  RefBase(const RefBase<T> &other)
    : m_count(other.m_count),
      m_ptr(other.m_ptr)
  {
    if (m_count) {
      ++*m_count;
    }
  }

  template<typename Z> RefBase(const RefBase<Z> &other)
    : m_count(other.count()),
      m_ptr(other.ptr())
  {
    if (m_count) {
      ++*m_count;
    }
  }

  ~RefBase()
  {
    release();
  }

  void assign(const RefBase &other);

private:
  // not implemented
  RefBase<T> &operator=(const RefBase<T> &);

  void acquire(const RefBase<T> &other);
  void release();

  int *m_count;
  T *m_ptr;
};

template <typename T>
RefBase<T>::RefBase(T *ptr)
{
  if (ptr) {
    try {
      m_count = new int(1);
    } catch (...) {
      delete ptr;
      throw;
    }
  } else {
    m_count = 0;
  }
  m_ptr = ptr;
}

template <typename T>
void RefBase<T>::acquire(const RefBase<T> &other)
{
  m_count = other.m_count;
  m_ptr = other.m_ptr;
  if (m_count) {
    ++*m_count;
  }
}

template <typename T>
void RefBase<T>::release()
{
  assert(!m_count || *m_count >= 1);
  assert(!m_count == !m_ptr);
  if (m_count && --*m_count == 0) {
    delete m_count;
    delete m_ptr;
  }
}

template <typename T>
void RefBase<T>::assign(const RefBase &other)
{
  if (m_count != other.m_count) {
    release();
    acquire(other);
  }
}

template <typename T> class Ref;

template <typename T> class CRef : public RefBase<T>
{
public:
  CRef()
  {
  }

  explicit CRef(T *ptr)
    : RefBase<T>(ptr)
  {
  }

  CRef(const CRef<T> &other)
    : RefBase<T>(other)
  {
  }

  template <class Z> CRef(const Ref<Z> &other)
    : RefBase<T>(other)
  {
  }

  template <class Z> CRef(const CRef<Z> &other)
    : RefBase<T>(other)
  {
  }

  CRef<T> &operator=(const CRef<T> &other)
  {
    assign(other);
    return *this;
  }

  const T *operator->() const
  {
    return RefBase<T>::ptr();
  }

  const T &operator*() const
  {
    return *RefBase<T>::ptr();
  }

  const T *ptr() const
  {
    return RefBase<T>::ptr();
  }

  const CRef<T> transfer()
  {
    CRef<T> tmp(*this);
    RefBase<T>::clear();
    return tmp;
  }
};

template <typename T> class Ref : public CRef<T>
{
public:
  Ref()
  {
  }

  explicit Ref(T *ptr)
    : CRef<T>(ptr)
  {
  }

  template<typename Z> explicit Ref<T>(Z *ptr)
    : CRef<T>(ptr)
  {
  }

  Ref(const Ref &other)
    : CRef<T>(other)
  {
  }

  template<typename Z> Ref(const Ref<Z> &other)
    : CRef<T>(other)
  {
  }

  Ref<T> &operator=(const Ref<T> &other)
  {
    assign(other);
    return *this;
  }

  T *operator->() const
  {
    return RefBase<T>::ptr();
  }

  T &operator*() const
  {
    return *RefBase<T>::ptr();
  }

  T *ptr() const
  {
    return RefBase<T>::ptr();
  }

  const Ref<T> transfer()
  {
    Ref<T> tmp(*this);
    RefBase<T>::clear();
    return tmp;
  }
};

template<typename T> Ref<T> make_ref(T *ptr)
{
  return Ref<T>(ptr);
}

template<typename T> bool operator==(const RefBase<T> &a,
                                     const RefBase<T> &b)
{
  return a.equals(b);
}

template<typename T> bool operator!=(const RefBase<T> &a,
                                     const RefBase<T> &b)
{
  return !a.equals(b);
}

#endif // _Ref_h
