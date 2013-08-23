///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: WordArray.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _WordArray_h
#define _WordArray_h

class WordData;

class WordArray
{
public:
  WordArray(char *buffer = 0,
            int num_words = 0);
  ~WordArray();

  enum {
    ENTRY_SIZE = 12,
    KEY_SIZE = 4,
    COUNT_SIZE = 3,
    FLAGS_SIZE = 2,
  };

  typedef unsigned long key_t;

  void reset(char *buffer,
             int num_words);

  key_t readKey(int index);

  void readWord(int index,
                key_t &key,
                WordData &word);

  void writeWord(int index,
                 key_t key,
                 const WordData &word);


  static void fillNullBuffer(unsigned char *buffer, int num_entries);

private:
  /// Not implemented.
  WordArray(const WordArray &);

  /// Not implemented.
  WordArray& operator=(const WordArray &);

private:
  static void copyToArray(unsigned char *&data,
                          int length,
                          int number);

  static void copyFromArray(const unsigned char *&data,
                            int length,
                            int &number);

private:
  unsigned char *m_buffer;
  int m_numWords;
};

#endif // _WordArray_h
