///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: WordArray.cc 272 2007-01-06 19:37:27Z brian $
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

#include "WordData.h"
#include "WordArray.h"

WordArray::WordArray(char *buffer,
                     int num_words)
  : m_buffer((unsigned char *)buffer),
    m_numWords(num_words)
{
}

WordArray::~WordArray()
{
}

void WordArray::reset(char *buffer,
                      int num_words)
{
  m_buffer = (unsigned char *)buffer;
  m_numWords = num_words;
}

WordArray::key_t WordArray:: readKey(int index)
{
  assert(index >= 0);
  assert(index < m_numWords);

  int key_int = 0;
  const unsigned char *data = m_buffer + ENTRY_SIZE * index;
  copyFromArray(data, KEY_SIZE, key_int);
  return (key_t)key_int;
}

void WordArray::readWord(int index,
                         WordArray::key_t &key,
                         WordData &word)
{
  assert(index >= 0);
  assert(index < m_numWords);

  const unsigned char *data = m_buffer + ENTRY_SIZE * index;
  int good_count, spam_count, flags, key_int;
  copyFromArray(data, KEY_SIZE, key_int);
  copyFromArray(data, COUNT_SIZE, good_count);
  copyFromArray(data, COUNT_SIZE, spam_count);
  copyFromArray(data, FLAGS_SIZE, flags);
  word.reset(good_count, spam_count, flags);
  key = key_int;
}

void WordArray::writeWord(int index,
                          WordArray::key_t key,
                          const WordData &word)
{
  assert(index >= 0);
  assert(index < m_numWords);

  unsigned char *data = m_buffer + ENTRY_SIZE * index;
  copyToArray(data, KEY_SIZE, key);
  copyToArray(data, COUNT_SIZE, word.goodCount());
  copyToArray(data, COUNT_SIZE, word.spamCount());
  copyToArray(data, FLAGS_SIZE, word.flags());
}

void WordArray::copyToArray(unsigned char *&data,
                            int length,
                            int number)
{
  for (int i = 0; i < length; ++i) {
    *data = (unsigned char)(number & 0xff);
    number = number >> 8;
    ++data;
  }
}

void WordArray::copyFromArray(const unsigned char *&data,
                              int length,
                              int &number)
{
  number = 0;
  data += length;
  for (int i = 0; i < length; ++i) {
    --data;
    number = number << 8;
    number = number | (unsigned int)*data;
  }
  data += length;
}

void WordArray::fillNullBuffer(unsigned char *buffer,
                               int num_entries)
{
  memset(buffer, 0, num_entries * ENTRY_SIZE);
}
