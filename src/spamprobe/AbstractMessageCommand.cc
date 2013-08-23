///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractMessageCommand.cc 272 2007-01-06 19:37:27Z brian $
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
#include "CleanupManager.h"
#include "ParserConfig.h"
#include "IstreamCharReader.h"
#include "TraditionalMailMessageParser.h"
#include "AbstractMailMessageReader.h"
#include "LineReader.h"
#include "MD5Digester.h"
#include "MailMessageDigester.h"
#include "File.h"
#include "ConfigManager.h"
#include "CommandConfig.h"
#include "SpamFilter.h"
#include "AutoPurger.h"
#include "MailMessageReaderFactory.h"
#include "AbstractMessageCommand.h"

bool AbstractMessageCommand::shouldProcessMessage(const ConfigManager &config,
                                                  Message &message)
{
  const string &status_field_name = config.commandConfig()->statusFieldName();
  if (status_field_name.length() == 0) {
    return true;
  }

  string value;
  if (message.getHeader(status_field_name, value).length() == 0) {
    return true;
  }

  if (value.find('D') == string::npos) {
    return true;
  }

  if (is_debug) {
    cerr << "SKIPPING MESSAGE WITH STATUS: " << value << endl;
  }
  return false;
}

void AbstractMessageCommand::processMailReader(const ConfigManager &config,
                                               SpamFilter &filter,
                                               AutoPurger &purger,
                                               const File *stream_file,
                                               Ptr<AbstractMailMessageReader> &mail_reader)
{
  const CommandConfig *cmd_config = config.commandConfig();
  const ParserConfig *prs_config = config.parserConfig();

  TraditionalMailMessageParser parser(config.parserConfig());
  MailMessageDigester digester;

  int message_number = 0;
  Ptr<MailMessage> mail_message;
  mail_message.set(mail_reader->readMessage());
  while (mail_message.isNotNull()) {
      Ptr<Message> msg(parser.parseMailMessage(mail_message.get()));
      msg->setSource(mail_message.release());
      digester.assignDigestToMessage(msg.get(), msg->source(), prs_config->spamprobeFieldName());
      bool should_process = shouldProcessMessage(config, *msg);
      if (should_process) {
        if (is_debug) {
          cerr << "*** NEW MESSAGE ***" << endl;
        }
        ++message_number;
        processMessage(config, filter, stream_file, *msg, message_number, m_isMessageSpam);
        purger.processedMessage();
      } else {
        logMessageProcessing(false, *msg, stream_file);
      }
      mail_message.set(mail_reader->readMessage());
  }
}

static bool read_tokens(LineReader &in,
                        Message &msg)
{
  MD5Digester digester;
  msg.clear();
  digester.start();
  while (in.forward() && in.currentLine().length() > 0) {
    digester.add(in.currentLine());
    msg.addToken(in.currentLine(), Token::FLAG_NORMAL);
  }
  digester.stop();
  msg.setDigest(digester.asString());
  return msg.getTokenCount() > 0;
}

void AbstractMessageCommand::processTokenStream(const ConfigManager &config,
                                                SpamFilter &filter,
                                                AutoPurger &purger,
                                                const File *stream_file,
                                                istream &stream)
{
  Message msg;

  int message_num = 0;
  IstreamCharReader char_reader(&stream);
  LineReader line_reader(&char_reader);

  while (read_tokens(line_reader, msg)) {
    if (is_debug) {
      cerr << "*** NEW MESSAGE ***" << endl;
    }

    ++message_num;
    processMessage(config, filter, stream_file, msg, message_num, m_isMessageSpam);
    purger.processedMessage();
  }
}

void AbstractMessageCommand::processFile(const ConfigManager &config,
                                         SpamFilter &filter,
                                         const File *stream_file)
{
  AutoPurger purger(config, filter);
  if (isReadOnly()) {
    purger.disable();
  }

  m_isMessageSpam = false;
  if (config.commandConfig()->streamIsPretokenized()) {
    if (stream_file) {
      ifstream stream(stream_file->getPath().c_str());
      processTokenStream(config, filter, purger, stream_file, stream);
    } else {
      processTokenStream(config, filter, purger, stream_file, cin);
    }
  } else {
    MailMessageReaderFactory factory;
    factory.setIgnoreFrom(config.commandConfig()->ignoreFrom());
    factory.setIgnoreContentLength(config.commandConfig()->ignoreContentLength());
    Ptr<AbstractMailMessageReader> reader;
    if (stream_file) {
      reader.set(factory.createReader(*stream_file));
    } else {
      reader.set(factory.createStdinReader());
    }
    processMailReader(config, filter, purger, stream_file, reader);
  }
  purger.finish();
}

bool AbstractMessageCommand::isMessageSpam() const
{
  return m_isMessageSpam;
}
