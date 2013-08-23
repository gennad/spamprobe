///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: util.cc 272 2007-01-06 19:37:27Z brian $
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

#include "util.h"

const string EMPTY_STRING;

static const char HEX_CHARS[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

bool is_debug = false;
bool is_verbose = false;

string to_7bits(const string &str)
{
    string answer(str);
    for (string::size_type pos = 0, limit = str.length(); pos < limit; ++pos) {
      unsigned char ch = str[pos];
      if (ch & 0x80) {
        answer[pos] = (char)(ch & 0x7f);
      }
    }
    return answer;
}

string trim_copy(const string &str,
                 string::size_type offset)
{
    string answer;
    const char *s = str.c_str() + offset;
    while (*s && is_space(*s)) {
        ++s;
    }
    while (*s) {
        answer += *s;
        ++s;
    }
    return answer;
}

string trim(const string &str)
{
    const char *start = str.c_str();
    while (*start && is_space(*start)) {
        ++start;
    }

    string answer;
    if (*start) {
        const char *last = start;
        for (const char *s = start; *s; ++s) {
            if (!is_space(*s)) {
                last = s;
            }
        }
        answer.assign(start, last - start + 1);
    }

    return answer;
}

string to_lower(const string &str)
{
    string answer;
    answer.reserve(str.length() + 1);
    for (const char *s = str.c_str(); *s; ++s) {
        answer += to_lower(*s);
    }
    return answer;
}

string to_upper(const string &str)
{
    string answer;
    for (const char *s = str.c_str(); *s; ++s) {
        answer += to_upper(*s);
    }
    return answer;
}

bool is_digits(const string &str)
{
    for (const char *s = str.c_str(); *s; ++s) {
        if (!is_digit(*s)) {
            return false;
        }
    }
    return true;
}

bool is_lower(const string &str)
{
    for (const char *s = str.c_str(); *s; ++s) {
        if (is_alpha(*s) && !is_lower(*s)) {
            return false;
        }
    }
    return true;
}

void encode_string(const string &src,
                   string &dst)
{
    const unsigned char *s = (const unsigned char *)src.data();
    const unsigned char *end = s + src.size();
    for ( ; s != end; ++s) {
        if (*s == '\\') {
            dst += "\\\\";
        } else if (isprint(*s) && (*s != '\"')) {
            dst += *s;
        } else {
            dst += '\\';
            dst += ('0' + ((*s & 0xd0) >> 6));
            dst += ('0' + ((*s & 0x38) >> 3));
            dst += ('0' + (*s & 0x07));
        }
    }
}

void decode_string(const string &src,
                   string &dst)
{
    const char *s = src.c_str();
    while (*s) {
        if (*s != '\\') {
            dst += *s;
            ++s;
        } else {
            ++s;
            if (*s == '\\') {
                dst += *s;
                ++s;
            } else {
                assert(is_digit(s[0]) && is_digit(s[1]) && is_digit(s[2]));
                unsigned char ch = ((s[0] - '0') << 6) | ((s[1] - '0') << 3) | (s[2] - '0');
                dst += ch;
                s += 3;
            }
        }
    }
}

char hex_digit(unsigned ch)
{
    assert(ch <= 15);
    return HEX_CHARS[ch & 0x0f];
}

int hex_to_int(char ch)
{
    if (is_digit(ch)) {
        return ch - '0';
    } else {
        return 10 + to_lower(ch) - 'a';
    }
}

int hex_to_int(const string &str)
{
    int answer = 0;
    for (const char *s = str.c_str(); *s; ++s) {
        answer = (answer << 4) | hex_to_int(*s);
    }
    return answer;
}

const char *num_to_string(int num)
{
    static char buffer[32];
    sprintf(buffer, "%d", num);
    return buffer;
}

const char *num_to_string(double num)
{
    static char buffer[128];
    sprintf(buffer, "%f", num);
    return buffer;
}

const char *num_to_string3(int num)
{
    static char buffer[32];
    sprintf(buffer, "%03d", num);
    return buffer;
}
