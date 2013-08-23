///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id$
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

#ifndef _TempPtr_h
#define _TempPtr_h

#include <cassert>

/**
 * Manages a temporary pointer to an object.  Nulls the pointer in its destructor.
 * Note that this is not intended for resource management but simply to ensure
 * that a pointer is cleared once it is no longer needed (to avoid it being reused
 * accidentally to refer to an already deleted object).
 */
template <typename T>
class TempPtr
{
public:
  TempPtr(T *&ptr, T *value)
    : m_ptr(ptr)
  {
    assert(m_ptr == 0);
    m_ptr = value;
  }

  ~TempPtr()
  {
    m_ptr = 0;
  }

private:
  /// Not implemented.
  TempPtr(const TempPtr &);

  /// Not implemented.
  TempPtr& operator=(const TempPtr &);

private:
  T *&m_ptr;
};

#endif // _TempPtr_h
