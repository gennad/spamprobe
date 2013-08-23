///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: File.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _File_h
#define _File_h

#include <vector>
#include "util.h"

class File
{
public:
  explicit File();
  explicit File(const string &path);
  explicit File(const File &parent,
                const string &filename);
  File(const File &);
  File& operator=(const File &);
  ~File();

  operator const char *() const
  {
    return m_path.c_str();
  }

  void addComponent(const string &name);

  void setPath(const string &path);

  string getPath() const
  {
    return m_path;
  }

  string getName() const;
  bool exists() const;
  bool isFile() const;
  bool isDirectory() const;
  bool isSelfOrParent() const;
  bool isHidden() const;
  bool hasChild(const string &path) const;
  File parent() const;

  unsigned long getSize() const;

  void makeDirectory(int mode) const;
  void rename(const string &new_path);
  void remove();

  bool hasSuffix(const string &suffix) const;
  void setSuffix(const string &suffix);

  void children(vector<File> &files);

  static File getHomeDir();

  static const string DOT;
  static const string DOT_DOT;

private:
  string m_path;
};

bool operator<(const File &a, const File &b);

#endif // _File_h
