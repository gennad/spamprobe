///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: Command_import.cc 272 2007-01-06 19:37:27Z brian $
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

#include <fstream>
#include "LineReader.h"
#include "IstreamCharReader.h"
#include "WordData.h"
#include "RegularExpression.h"
#include "SpamFilter.h"
#include "FrequencyDB.h"
#include "CommandConfig.h"
#include "ConfigManager.h"
#include "SpamFilter.h"
#include "Command_import.h"

Command_import::Command_import()
: AbstractFileCommand("import",
                      "import        Imports terms into database.",
                      "import [filename...]\n"
                      "    Reads the specified files which must contain export data written by\n"
                      "    the export command.  The terms and counts from this file are added\n"
                      "    to the database.  This can be used to convert a database from a\n"
                      "    prior version.\n",
                      false,
                      true)
{
}

void Command_import::processStream(const ConfigManager &config,
                                   SpamFilter &filter,
                                   const File *stream_file,
                                   istream &in_stream)
{
  RegularExpression parts_expr("^(-?[0-9]+),(-?[0-9]+)(,([0-9]+))?,\"?([^\"]+)\"?$", 5);
  assert(parts_expr.isValid());

  string match_buffer;
  int record_count = 0;
  IstreamCharReader char_reader(&in_stream);
  LineReader line_reader(&char_reader);
  while (line_reader.forward()) {
    if (is_debug) {
      cerr << "IMPORT " << line_reader.currentLine() << endl;
    }
    if (parts_expr.match(line_reader.currentLine())) {
      if (parts_expr.matchCount() >= 6) {
        int good_count = atoi(parts_expr.getMatch(1, match_buffer).c_str());
        int spam_count = atoi(parts_expr.getMatch(2, match_buffer).c_str());
        string flags_str = parts_expr.getMatch(4, match_buffer);
        parts_expr.getMatch(5, match_buffer);

        string decoded;
        decode_string(match_buffer, decoded);

        if (is_debug) {
          cerr << "gc " << good_count
               << " sc " << spam_count
               << " flags " << flags_str
               << " term " << match_buffer
               << endl;
        }

        if (flags_str.length() > 0) {
          unsigned long flags = atoi(flags_str.c_str());
          filter.getDB()->addWord(decoded, good_count, spam_count, flags);
        } else {
          filter.getDB()->addWord(decoded, good_count, spam_count);
        }
        if (++record_count > 10000) {
          filter.flush();
          record_count = 0;
        }
      }
    }
  }
}

void Command_import::processFile(const ConfigManager &config,
                                 SpamFilter &filter,
                                 const File *stream_file)
{
  if (stream_file) {
    ifstream in_stream(stream_file->getPath().c_str());
    processStream(config, filter, stream_file, in_stream);
  } else {
    processStream(config, filter, stream_file, cin);
  }
}
