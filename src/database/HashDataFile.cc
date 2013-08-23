///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HashDataFile.cc 272 2007-01-06 19:37:27Z brian $
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

#ifdef USE_MMAP

#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include "File.h"
#include "WordData.h"
#include "HashDataFile.h"

enum {
  SIZE_MULTIPLE = 1024,
  DEBUG_HASH = 1,
};

// table of primes indexed roughy by megabyte from 1 to 100 megs
static const int primes_by_meg[] = {
  // assumes 12 bytes per entry
     87359,     174761,     262139,     349519,     436889,   // 1 - 5 megs
    524287,     611657,     699037,     786431,     873787,   // 6 - 10 megs
    961189,    1048573,    1135951,    1223329,    1310719,   // 11 - 15 megs
   1398091,    1485479,    1572853,    1660231,    1747619,   // 16 - 20 megs
   1835003,    1922387,    2009759,    2097143,    2184509,   // 21 - 25 megs
   2271901,    2359267,    2446673,    2534057,    2621431,   // 26 - 30 megs
   2708821,    2796181,    2883577,    2970959,    3058343,   // 31 - 35 megs
   3145721,    3233093,    3320477,    3407857,    3495253,   // 36 - 40 megs
   3582629,    3670013,    3757393,    3844777,    3932153,   // 41 - 45 megs
   4019527,    4106897,    4194301,    4281679,    4369061,   // 46 - 50 megs
   4456433,    4543823,    4631203,    4718579,    4805959,   // 51 - 55 megs
   4893341,    4980727,    5068111,    5155489,    5242877,   // 56 - 60 megs
   5330251,    5417597,    5505023,    5592373,    5679769,   // 61 - 65 megs
   5767129,    5854543,    5941921,    6029299,    6116687,   // 66 - 70 megs
   6204071,    6291449,    6378787,    6466193,    6553577,   // 71 - 75 megs
   6640979,    6728347,    6815741,    6903121,    6990493,   // 76 - 80 megs
   7077883,    7165267,    7252649,    7340009,    7427353,   // 81 - 85 megs
   7514791,    7602151,    7689557,    7776931,    7864301,   // 86 - 90 megs
   7951693,    8039081,    8126453,    8213819,    8301217,   // 91 - 95 megs
   8388593,    8475947,    8563351,    8650727,    8738129,   // 96 - 100 megs
};

#define PRIMES_ARRAY_SIZE (sizeof(primes_by_meg) / sizeof(primes_by_meg[0]))

static int compute_divisor_for_max_elements(int max_elements)
{
  for (int i = PRIMES_ARRAY_SIZE - 1; i >= 0; --i) {
    int divisor = primes_by_meg[i];
    if (divisor <= max_elements) {
      return divisor;
    }
  }
  return primes_by_meg[0];
}

HashDataFile::HashDataFile(int num_headers,
                           HashDataFile::ulong_t size_in_bytes)
  : m_base(0)
{
  initialize(num_headers, size_in_bytes);
}

HashDataFile::~HashDataFile()
{
  close();
}

void HashDataFile::initialize(int num_headers,
                              HashDataFile::ulong_t size_in_bytes)
{
  m_numHeaders = num_headers;
  int max_elements = (size_in_bytes / WordArray::ENTRY_SIZE) - m_numHeaders;
  m_divisor = compute_divisor_for_max_elements(max_elements);
  m_indexLimit = m_divisor + m_numHeaders;
  m_size = WordArray::ENTRY_SIZE * m_indexLimit;
  if (is_debug) {
    cerr << "HASH_DIVISOR " << m_divisor
         << " (0x" << hex << m_divisor << ")"
         << " INDEX_LIMIT " << dec << m_indexLimit
         << " SIZE " << m_size
         << endl;
  }
}

void HashDataFile::populateEmptyFile(int fd)
{
  if (is_debug) {
    cerr << "INITIALIZING NEW HASHFILE" << endl;
  }

  const int SIZE_MULTIPLE = 1024;
  unsigned char zeros[SIZE_MULTIPLE * WordArray::ENTRY_SIZE];
  WordArray::fillNullBuffer(zeros, SIZE_MULTIPLE);

  for (int i = 0; i < m_indexLimit; i += SIZE_MULTIPLE) {
    ::write(fd, &zeros, min(m_indexLimit - i, (HashDataFile::ulong_t)SIZE_MULTIPLE) * WordArray::ENTRY_SIZE);
  }
}

bool HashDataFile::open(const string &filename,
                        bool read_only,
                        int create_mode)
{
  close();

  if (is_debug) {
    cerr << "OPEN HASHFILE " << filename << endl;
  }

  m_isReadOnly = read_only;
  m_createMode = create_mode;

  File data_file(filename);
  bool exists = data_file.isFile();

  m_isNewFile = !exists;

  if (read_only && !exists) {
    cerr << "error: hash file "
         << filename
         << " does not exist and cannot be created in read-only mode"
         << endl;
    return false;
  }

  if (exists) {
    unsigned long file_size = data_file.getSize();
    if ((file_size % WordArray::ENTRY_SIZE) != 0) {
      cerr << "error: hash file "
           << filename
           << " size not a multiple of "
           << WordArray::ENTRY_SIZE
           << " bytes"
           << endl;
      return false;
    }
    initialize(m_numHeaders, file_size);
    if (m_size != file_size) {
      cerr << "error: hash file "
           << filename
           << " size invalid "
           << file_size
           << " != "
           << m_size
           << endl;
      return false;
    }
  }

  if (is_debug) {
    cerr << "HASHFILE " << filename << " SIZE " << m_size << endl;
  }

  int flags = (read_only) ? O_RDONLY : O_RDWR;
  if (!exists) {
    flags |= O_CREAT;
  }

  int fd = ::open(filename.c_str(), flags, create_mode);
  if (fd < 0) {
    cerr << "error: unable to open database " << filename << ": " << strerror(errno) << endl;
    return false;
  }

  if (!exists) {
    populateEmptyFile(fd);
  }

  if (!mmapFile(fd, read_only)) {
    cerr << "error: unable to mmap file " << filename << ": " << strerror(errno) << endl;
    close();
    return false;
  }

  m_filename = filename;

  return true;
}

bool HashDataFile::mmapFile(int fd,
                            bool read_only)
{

  if (is_debug) {
    cerr << "MMAPPING HASHFILE" << endl;
  }

  int flags = (read_only) ? PROT_READ : (PROT_READ | PROT_WRITE);
  m_base = (char *)mmap(0, m_size, flags, MAP_SHARED, fd, 0);
  if (m_base == (char *)-1) {
    m_base = 0;
    return false;
  }

  m_array.reset(m_base, m_indexLimit);
  return true;
}

void HashDataFile::close()
{
  if (m_base) {
    ::munmap(m_base, m_size);
    m_array.reset(0, 0);
    m_base = 0;
    m_filename.erase();
  }
}

bool HashDataFile::isOpen()
{
  return m_base != 0;
}

void HashDataFile::writeRecord(int record_number,
                               HashDataFile::ulong_t key,
                               const WordData &word_data)
{
  assert(m_base);
  assert(record_number >= 0);
  assert(record_number < m_indexLimit);

  m_array.writeWord(record_number, key, word_data);
}

void HashDataFile::readRecord(int record_number,
                              HashDataFile::ulong_t &key,
                              WordData &word_data)
{
  assert(m_base);
  assert(record_number >= 0);
  assert(record_number < m_indexLimit);

  m_array.readWord(record_number, key, word_data);
}

bool HashDataFile::write(ulong_t key,
                         const WordData &word_data)
{
  assert(m_base);

  int index = computeIndexForKey(key);
  for (int i = 0; i < m_divisor; ++i) {
    ulong_t old_key = m_array.readKey(index);
    if (old_key == key || old_key == 0) {
      m_array.writeWord(index, key, word_data);
      return true;
    }

    if (m_autoCleaner.isNotNull()) {
      WordData old_data;
      m_array.readWord(index, old_key, old_data);
      if (m_autoCleaner->shouldDelete(old_data)) {
	m_array.writeWord(index, key, word_data);
	if (is_debug) {
	  cerr << "HashDataFile: overwriting old word for new one: "
	       << " index " << index
	       << " new_key " << key
	       << " old_key " << old_key
	       << " total " << old_data.totalCount()
	       << " good " << old_data.goodCount()
	       << " spam " << old_data.spamCount()
	       << " age " << old_data.age()
	       << endl;
	}
	return true;
      }
    }

    // skip this slot since it contains a collision
    index = m_numHeaders + ((index - m_numHeaders + 1) % m_divisor);
  }

  return false;
}

bool HashDataFile::read(ulong_t key,
                        WordData &word_data)
{
  assert(m_base);

  ulong_t old_key = 0;
  int index = computeIndexForKey(key);
  for (int i = 0; i < m_divisor; ++i) {
    m_array.readWord(index, old_key, word_data);
    if (old_key == key) {
      // slot matches our key so return it's value
      return true;
    }

    if (old_key == 0) {
      // slot is empty - this key is not in file
      return false;
    }

    // skip this slot since it contains a collision
    index = m_numHeaders + ((index - m_numHeaders + 1) % m_divisor);
  }

  return false;
}

void HashDataFile::copyHeadersToFile(HashDataFile &file)
{
  ulong_t key = 0;
  WordData word_data;
  for (int i = 0; i < m_numHeaders; ++i) {
    readRecord(i, key, word_data);
    file.writeRecord(i, key, word_data);
  }
}

void HashDataFile::copyContentsToFile(HashDataFile &file)
{
  ulong_t key = 0;
  WordData word_data;
  for (int i = m_numHeaders; i < m_indexLimit; ++i) {
    readRecord(i, key, word_data);
    if (key != 0) {
      file.write(key, word_data);
    }
  }
}

#endif // USE_MMAP
