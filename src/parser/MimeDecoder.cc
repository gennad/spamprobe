///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MimeDecoder.cc 272 2007-01-06 19:37:27Z brian $
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

#include "AbstractCharReader.h"
#include "MultiLineString.h"
#include "MultiLineSubString.h"
#include "MessageHeader.h"
#include "MessageHeaderList.h"
#include "RegularExpression.h"
#include "MimeDecoder.h"

static unsigned char char_value(unsigned char ch)
{
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }

  if (ch >= 'a' && ch <= 'f') {
    return 10 + ch - 'a';
  }

  if (ch >= 'A' && ch <= 'F') {
    return 10 + ch - 'A';
  }

  return 0;
}

static const int BASE64_CHARS[256] = {
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
    52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
    -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
    -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
};
static const int MAX_CHAR_INDEX = sizeof(BASE64_CHARS) / sizeof(BASE64_CHARS[0]);

inline bool next_char64(AbstractCharReader *reader,
                        char &ch,
                        unsigned char &value)
{
    while (!reader->atEnd()) {
        ch = reader->currentChar();
        if (ch == '=') {
            value = 0;
            reader->forward();
            return true;
        }

        unsigned int index = (unsigned)(ch & 0xff);
        if (index >= MAX_CHAR_INDEX) {
          assert(!"should not be possible but just to be safe");
          break;
        }

        if (BASE64_CHARS[index] >= 0) {
            value = BASE64_CHARS[index];
            reader->forward();
            return true;
        }

        reader->forward();
    }

    ch = ' ';
    value = 0;
    return false;
}

CRef<AbstractMultiLineString> MimeDecoder::decodeHeaderString(const CRef<AbstractMultiLineString> &header_value)
{
  static const string token_expr("[^] \t()<>@,;:\"/[?.=]+");
  static const string encoded_word_expr = string("=\\?(") + token_expr + ")\\?(" + token_expr + ")\\?([^\\?]+)\\?=";

  string text;
  header_value->join(text);

  if (is_debug) {
    cerr << "decodeHeader: before: " << text << endl;
  }

  bool changed = false;
  RegularExpression::MatchData whole_match;
  string whole_string, charset, method, encoded_word, decoded_text;
  RegularExpression encoded_word_regex(encoded_word_expr, 4, false, true);
  string::size_type offset = 0;
  while ((offset < text.length()) && encoded_word_regex.match(text.c_str() + offset)) {
    string::size_type replace_start = offset;
    encoded_word_regex.getMatch(0, whole_match);
    encoded_word_regex.getMatch(2, method);
    encoded_word_regex.getMatch(3, encoded_word);
    method = trim(to_lower(method));

    bool valid_method = true;
    if (method == "q") {
      Ref<MultiLineString> word(new MultiLineString());
      word->addLine(encoded_word);
      CRef<AbstractMultiLineString> decoded_lines(unquoteString(word, true));
      decoded_lines->join(decoded_text);
      offset += decoded_text.length();
    } else if (method == "b") {
      Ref<MultiLineString> word(new MultiLineString());
      word->addLine(encoded_word);
      CRef<AbstractMultiLineString> decoded_lines(decodeString(word));
      decoded_lines->join(decoded_text);
      offset += decoded_text.length();
    } else {
      valid_method = false;
      offset += whole_match.end_pos;
    }

    if (valid_method) {
      text.replace(replace_start + whole_match.start_pos, whole_match.end_pos - whole_match.start_pos, decoded_text);
      changed = true;
    }
  }

  if (is_debug) {
    if (changed) {
      cerr << "decodeHeader: after: " << text << endl;
    } else {
      cerr << "decodeHeader: no change" << endl;
    }
  }

  if (changed) {
    return MultiLineString::fromText(text);
  } else {
    return header_value;
  }
}


bool MimeDecoder::isQuotedPrintable(const CRef<AbstractMultiLineString> &contentTransferEncoding)
{
    if (contentTransferEncoding.isNull() || contentTransferEncoding->lineCount() < 1) {
        return false;
    }
    return starts_with(to_lower(contentTransferEncoding->line(0)), "quoted-printable");
}

bool MimeDecoder::isBase64(const CRef<AbstractMultiLineString> &contentTransferEncoding)
{
    if (contentTransferEncoding.isNull() || contentTransferEncoding->lineCount() < 1) {
        return false;
    }
    return starts_with(to_lower(contentTransferEncoding->line(0)), "base64");
}

CRef<AbstractMultiLineString> MimeDecoder::decodeText(const MessageHeaderList *headers,
                                                             const CRef<AbstractMultiLineString> &body)
{
    if (!headers->isTextType()) {
        return body;
    }

    CRef<AbstractMultiLineString> transfer_encoding(headers->header("content-transfer-encoding"));
    if (isQuotedPrintable(transfer_encoding)) {
        if (is_debug) {
            cerr << "UNQUOTING BODY" << endl;
        }
        return unquoteString(body);
    }

    if (isBase64(transfer_encoding)) {
        if (is_debug) {
            cerr << "DECODING BODY" << endl;
        }
        return decodeString(body);
    }

    return body;
}

CRef<AbstractMultiLineString> MimeDecoder::unquoteString(const CRef<AbstractMultiLineString> &messageBody,
                                                                bool convert_underscores)
{
    Ref<MultiLineString> answer(new MultiLineString());
    string buffer;
    bool no_changes = true;

    buffer.reserve(10240);
    for (int k = 0; k < messageBody->lineCount(); ++k) {
        bool add_new_line = true;
        const string &line(messageBody->line(k));
        const char *line_chars = line.c_str();
        const int line_length = line.length();
        const int last_index = line_length - 1;
        const int hex_limit = line_length - 2;
        for (string::size_type i = 0; i < line_length; ++i) {
            char ch = line_chars[i];
            if (ch == '=') {
                no_changes = false;
                if (i == last_index) {
                    // soft line break - ignore newline
                    add_new_line = false;
                } else if ((i < hex_limit) && is_xdigit(line_chars[i+1]) && is_xdigit(line_chars[i+2])) {
                    char ch = (char)((char_value(line_chars[i+1]) << 4) | char_value(line_chars[i+2]));
                    buffer += safe_char(ch);
                    i += 2;
                }
            } else if (convert_underscores && ch == '_') {
                buffer += ' ';
                no_changes = false;
            } else {
                buffer += ch;
            }
        }
        if (add_new_line) {
            answer->addLine(buffer);
            buffer.erase();
        }
    }
    answer->addLine(buffer);

    if (no_changes) {
        return messageBody;
    } else {
        return answer;
    }
}

CRef<AbstractMultiLineString> MimeDecoder::decodeString(const CRef<AbstractMultiLineString> &messageBody)
{
    string buffer;
    Ref<AbstractCharReader> char_reader(messageBody->createCharReader());
    decodeString(char_reader.ptr(), buffer);
    if (is_debug) {
        cerr << "DECODED TO " << buffer << endl;
    }
    return MultiLineString::fromText(buffer);
}

void MimeDecoder::decodeString(AbstractCharReader *reader,
                               string &decoded_string)
{
    char c1, c2, c3, c4;
    unsigned char u1, u2, u3, u4;
    reader->forward();
    while (next_char64(reader, c1, u1) && next_char64(reader, c2, u2) &&
           next_char64(reader, c3, u3) && next_char64(reader, c4, u4))
    {
      if (c3 != '=' && c4 != '=') {
        // common case of 3 octets
        decoded_string += safe_char((u1 << 2) | (u2 >> 4));           // 6 + 2
        decoded_string += safe_char(((u2 & 0x0f) << 4) | (u3 >> 2));  // 4 + 4
        decoded_string += safe_char(((u3 & 0x03) << 6) | u4);         // 2 + 6
      } else if (c3 == '=') {
        // padded case with 1 octet
        decoded_string += safe_char((u1 << 2) | (u2 >> 4));           // 6 + 2
      } else {
        // padded case with 2 octets
        decoded_string += safe_char((u1 << 2) | (u2 >> 4));           // 6 + 2
        decoded_string += safe_char(((u2 & 0x0f) << 4) | (u3 >> 2));  // 4 + 4
      }
    }
}

void MimeDecoder::decodeData(const CRef<AbstractMultiLineString> &messageBody,
                             Buffer<unsigned char> &decoded_data)
{
    Ref<AbstractCharReader> char_reader(messageBody->createCharReader());
    decodeData(char_reader.ptr(), decoded_data);
}

void MimeDecoder::decodeData(AbstractCharReader *reader,
                             Buffer<unsigned char> &decoded_data)
{
    char c1, c2, c3, c4;
    unsigned char u1, u2, u3, u4;
    reader->forward();
    while (next_char64(reader, c1, u1) && next_char64(reader, c2, u2) &&
           next_char64(reader, c3, u3) && next_char64(reader, c4, u4))
    {
      if (c3 != '=' && c4 != '=') {
        // common case of 3 octets
        decoded_data.append((u1 << 2) | (u2 >> 4));           // 6 + 2
        decoded_data.append(((u2 & 0x0f) << 4) | (u3 >> 2));  // 4 + 4
        decoded_data.append(((u3 & 0x03) << 6) | u4);         // 2 + 6
      } else if (c3 == '=') {
        // padded case with 1 octet
        decoded_data.append((u1 << 2) | (u2 >> 4));           // 6 + 2
      } else {
        // padded case with 2 octets
        decoded_data.append((u1 << 2) | (u2 >> 4));           // 6 + 2
        decoded_data.append(((u2 & 0x0f) << 4) | (u3 >> 2));  // 4 + 4
      }
    }
}
