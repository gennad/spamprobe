///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: File.cc 272 2007-01-06 19:37:27Z brian $
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

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <stdexcept>
#include "File.h"

const char SEPARATOR_CHAR('/');
const string SEPARATOR("/");
const string SUFFIX_START(".");

const string File::DOT(".");
const string File::DOT_DOT("..");

File::File()
{
}

File::File(const string &path)
{
  setPath(path);
}

File::File(const File &other)
  : m_path(other.m_path)
{
}

File::File(const File &other,
           const string &name)
  : m_path(other.m_path)
{
  addComponent(name);
}

File::~File()
{
}

File &File::operator=(const File &other)
{
  m_path = other.m_path;
  return *this;
}

void File::addComponent(const string &name)
{
  if (m_path != SEPARATOR) {
    m_path += SEPARATOR;
  }
  string::size_type last = name.length() - 1;
  if (last >= 1 && name[last] == SEPARATOR_CHAR) {
    m_path.append(name, 0, last);
  } else {
    m_path += name;
  }
}

void File::setPath(const string &path)
{
  string::size_type last = path.length() - 1;
  if (last >= 1 && path[last] == SEPARATOR_CHAR) {
    m_path.assign(path, 0, last);
  } else {
    m_path = path;
  }
}

bool File::exists() const
{
  struct stat buf;
  return stat(m_path.c_str(), &buf) == 0;
}

bool File::isFile() const
{
  struct stat buf;
  return stat(m_path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode);
}

bool File::isDirectory() const
{
  struct stat buf;
  return stat(m_path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

bool File::isHidden() const
{
  return starts_with(getName(), DOT);
}

bool File::isSelfOrParent() const
{
  string name = getName();
  return name == DOT || name == DOT_DOT;
}

bool File::hasChild(const string &path) const
{
  return File(*this, path).exists();
}

unsigned long File::getSize() const
{
  struct stat buf;
  return stat(m_path.c_str(), &buf) == 0 ? buf.st_size : 0;
}

File File::parent() const
{
  string::size_type pos = m_path.rfind(SEPARATOR);
  if (pos == string::npos) {
    return isDirectory() ? File(DOT_DOT) : File(DOT);
  } else {
    return File(m_path.substr(0, pos));
  }
}

string File::getName() const
{
  string::size_type pos = m_path.rfind(SEPARATOR);
  if (pos == string::npos) {
    return m_path;
  } else {
    return m_path.substr(pos + 1);
  }
}

void File::makeDirectory(int mode) const
{
  if (mkdir(m_path.c_str(), (mode_t)mode) != 0) {
    throw runtime_error(strerror(errno));
  }
}

void File::rename(const string &new_path)
{
  if (::rename(m_path.c_str(), new_path.c_str()) != 0) {
    throw runtime_error(strerror(errno));
  }
  m_path = new_path;
}

void File::remove()
{
  if (unlink(m_path.c_str()) != 0) {
    throw runtime_error(strerror(errno));
  }
}

File File::getHomeDir()
{
  char *home = getenv("HOME");
  if (home == NULL) {
    return File();
  } else {
    return File(home);
  }
}

bool File::hasSuffix(const string &suffix) const
{
  string::size_type pos = m_path.rfind(SUFFIX_START);
  return (pos != string::npos) && (strcmp(m_path.c_str() + pos + 1, suffix.c_str()) == 0);
}

void File::setSuffix(const string &suffix)
{
  string::size_type pos = m_path.rfind(SUFFIX_START);
  if (pos != string::npos && m_path.find(SEPARATOR, pos + 1) == string::npos) {
    m_path.erase(pos, string::npos);
  }
  m_path += SUFFIX_START;
  m_path += suffix;
}

bool operator<(const File &a, const File &b)
{
  return a.getPath() < b.getPath();
}

#ifdef HAVE_DIRENT_H
struct DirHelper
{
  DirHelper(const char *path) : dir(opendir(path)) {}
  ~DirHelper() { if (dir) closedir(dir); }

  DIR *dir;
};
#endif

void File::children(vector<File> &files)
{
#ifdef HAVE_DIRENT_H
  DirHelper d(m_path.c_str());
  if (d.dir) {
    for (struct dirent *ent = readdir(d.dir); ent ; ent = readdir(d.dir)) {
      files.push_back(File(*this, ent->d_name));
    }
  }
#endif
}
