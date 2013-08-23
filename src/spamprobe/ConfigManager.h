///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: ConfigManager.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _ConfigManager_h
#define _ConfigManager_h

#include <iostream>
#include "util.h"

class CommandConfig;
class DatabaseConfig;
class FilterConfig;
class HdlStatement;
class HdlPrinter;
class ParserConfig;
class File;

class ConfigManager
{
public:
  ConfigManager();
  ~ConfigManager();

  const File &configFile() const
  {
    return *m_configFile;
  }

  void loadFromFile(const File &config_file);
  void writeToFile(const File &config_file) const;

  DatabaseConfig *databaseConfig() const
  {
    return m_databaseConfig.get();
  }

  FilterConfig *filterConfig() const
  {
    return m_filterConfig.get();
  }

  ParserConfig *parserConfig() const
  {
    return m_parserConfig.get();
  }

  CommandConfig *commandConfig() const
  {
    return m_commandConfig.get();
  }

private:
  void loadDatabaseConfig(const CRef<HdlStatement> &database_stmt);
  void loadFilterConfig(const CRef<HdlStatement> &filter_stmt);
  void loadParserConfig(const CRef<HdlStatement> &parser_stmt);
  void loadHeaderConfig(const CRef<HdlStatement> &header_stmt);
  void writeConfig(HdlPrinter &printer, ostream &out) const;
  void writeDatabaseConfig(HdlPrinter &printer, ostream &out) const;
  void writeFilterConfig(HdlPrinter &printer, ostream &out) const;
  void writeParserConfig(HdlPrinter &printer, ostream &out) const;

private:
  /// Not implemented.
  ConfigManager(const ConfigManager &);

  /// Not implemented.
  ConfigManager& operator=(const ConfigManager &);

private:
  Ptr<File> m_configFile;
  Ptr<CommandConfig> m_commandConfig;
  Ptr<DatabaseConfig> m_databaseConfig;
  Ptr<FilterConfig> m_filterConfig;
  Ptr<ParserConfig> m_parserConfig;
};

#endif // _ConfigManager_h
