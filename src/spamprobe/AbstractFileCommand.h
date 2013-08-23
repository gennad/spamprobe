///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractFileCommand.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _AbstractFileCommand_h
#define _AbstractFileCommand_h

#include "AbstractCommand.h"

class ConfigManager;
class File;
class SpamFilter;

class AbstractFileCommand : public AbstractCommand
{
public:
  AbstractFileCommand(const string &name,
                      const string &short_help,
                      const string &long_help,
                      bool read_only,
                      bool needs_database)
    : AbstractCommand(name, short_help, long_help, read_only),
      m_needsDatabase(needs_database)
  {
  }

  bool argsOK(const ConfigManager &config);

  int execute(const ConfigManager &config,
              SpamFilter &filter);

  bool needsDatabase() const
  {
    return m_needsDatabase;
  }

  virtual void processFile(const ConfigManager &config,
                           SpamFilter &filter,
                           const File *stream_file) = 0;

  static const int AUTO_PURGE_JUNK_COUNT;

private:
  bool m_needsDatabase;
};

#endif // _AbstractFileCommand_h
