///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HashDataFile.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HashDataFile_h
#define _HashDataFile_h

#ifdef USE_MMAP

#include "FrequencyDBImpl.h"
#include "WordArray.h"
#include "CleanupManager.h"

class HashDataFile
{
public:
  typedef WordArray::key_t ulong_t;

  HashDataFile(int num_headers,
               ulong_t size_in_bytes);
  ~HashDataFile();

  void initialize(int num_headers,
                  ulong_t size_in_bytes);

  ulong_t firstIndex() const
  {
    return m_numHeaders;
  }

  ulong_t indexLimit() const
  {
    return m_indexLimit;
  }

  ulong_t size() const
  {
    return m_size;
  }

  const string &filename() const
  {
    return m_filename;
  }

  bool open(const string &filename,
            bool read_only,
            int create_mode);

  void close();

  bool isOpen();

  void writeRecord(int header_number,
                   ulong_t key,
                   const WordData &word_data);

  void readRecord(int header_number,
                  ulong_t &key,
                  WordData &word_data);

  bool write(ulong_t key,
             const WordData &word_data);

  bool read(ulong_t key,
            WordData &word_data);

  bool isReadOnly() const
  {
    m_isReadOnly;
  }

  int createMode() const
  {
    return m_createMode;
  }

  bool isNewFile() const
  {
    return m_isNewFile;
  }

  void setAutoClean(OWNED CleanupManager *cleaner)
  {
    m_autoCleaner.set(cleaner);
  }

  void copyHeadersToFile(HashDataFile &file);

  void copyContentsToFile(HashDataFile &file);

private:
  /// Not implemented.
  HashDataFile(const HashDataFile &);

  /// Not implemented.
  HashDataFile& operator=(const HashDataFile &);

  void populateEmptyFile(int fd);

  bool mmapFile(int fd, bool read_only);

  ulong_t computeIndexForKey(ulong_t key)
  {
    return m_numHeaders + (key % m_divisor);
  }

private:
  string m_filename;
  bool m_isReadOnly;
  int m_createMode;
  bool m_isNewFile;

  char *m_base;
  WordArray m_array;
  ulong_t m_numHeaders;
  ulong_t m_size;
  ulong_t m_divisor;
  ulong_t m_indexLimit;

  Ptr<CleanupManager> m_autoCleaner;
};

#endif // USE_MMAP

#endif // _HashDataFile_h
