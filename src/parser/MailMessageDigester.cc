///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MailMessageDigester.cc 272 2007-01-06 19:37:27Z brian $
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

#include "MD5Digester.h"
#include "MessageHeaderList.h"
#include "RegularExpression.h"
#include "MailMessageDigester.h"

void MailMessageDigester::assignDigestToMessage(Message *parsed_message,
                                                MailMessage *mail_message,
                                                const string &existing_digest_header)
{
    string digest;
    if (!getExistingDigest(mail_message->head(), existing_digest_header, digest)) {
        digestMessage(mail_message, digest);
    }
    parsed_message->setDigest(digest);
}

const string &MailMessageDigester::digestMessage(MailMessage *message,
                                                 string &buffer)
{
    MD5Digester md5;
    md5.start();
    digestLines(md5, message->bodyText());
    digestHeaderLines(md5, message->head(), "date");
    digestHeaderLines(md5, message->head(), "from");
    digestHeaderLines(md5, message->head(), "to");
    digestHeaderLines(md5, message->head(), "subject");
    md5.stop();
    buffer = md5.asString();
    return buffer;
}

void MailMessageDigester::digestLines(MD5Digester &md5,
                                      const CRef<AbstractMultiLineString> lines)
{
    for (int i = 0; i < lines->lineCount(); ++i) {
        const string &line(lines->line(i));
        if (line.length() > 0) {
            md5.add(line);
            if (is_debug) {
                cerr << "DIGEST: " << line << endl;
            }
        }
    }
}

void MailMessageDigester::digestHeaderLines(MD5Digester &md5,
                                            const MessageHeaderList *message_headers,
                                            const string &header_name)
{
  CRef<AbstractMultiLineString> header_lines(message_headers->header(header_name));
  if (header_lines.isNotNull()) {
    digestLines(md5, header_lines);
  }
}

bool MailMessageDigester::getExistingDigest(const MessageHeaderList *message_headers,
                                            const string &header_name,
                                            string &buffer)
{
  static RegularExpression digest_expr("^[a-z]+ +[0-9]\\.[0-9]+ +([0-9a-z]+)", 2, true);

  if (header_name.length() == 0) {
    return false;
  }

  CRef<AbstractMultiLineString> header_lines(message_headers->header(header_name));
  if (header_lines.isNull()) {
    return false;
  }

  assert(header_lines->lineCount() >= 1);
  if (!digest_expr.match(header_lines->line(0))) {
    return false;
  }

  return digest_expr.getMatch(1, buffer).length() == 32;
}

