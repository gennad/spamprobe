///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: util.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _util_h
#define _util_h

#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif

#include <cmath>
#include <memory>
#include <string>
#include <cctype>
#include <cassert>
#include <iostream>
#include <cstdio>
#include "Ptr.h"
#include "Ref.h"

using namespace std;

#define OWNED

extern bool is_debug;
extern bool is_verbose;

inline unsigned to_8bits(char ch)
{
    return((unsigned)ch) & 0xff;
}

extern string to_7bits(const string &str);
extern string to_lower(const string &str);
extern string to_upper(const string &str);

extern bool is_lower(const string &str);

extern string trim_copy(const string &str,
                        string::size_type start_offset);

extern string trim(const string &str);

inline bool is_nan(double value)
{
#ifdef isnan
    return isnan(value);
#else
    return !(value == value);
#endif
}

inline bool starts_with(const string &str,
                        const char *prefix)
{
    return strncmp(str.c_str(), prefix, strlen(prefix)) == 0;
}

inline bool starts_with(const string &str,
                        const string &prefix)
{
    return strncmp(str.c_str(), prefix.c_str(), prefix.length()) == 0;
}

inline bool ends_with(const string &str,
                      const string &suffix)
{
    return str.length() >= suffix.length() && strcmp(str.c_str() + str.length() - suffix.length(), suffix.c_str()) == 0;
}

extern bool is_digits(const string &str);

// work around for unsafe ctype functions on solaris
inline bool is_lower(char ch)
{
    return islower(to_8bits(ch));
}

// work around for unsafe ctype functions on solaris
inline bool is_upper(char ch)
{
    return isupper(to_8bits(ch));
}

inline bool is_space(char ch)
{
    return isspace(to_8bits(ch));
}

inline bool is_alpha(char ch)
{
    return isalpha(to_8bits(ch));
}

inline bool is_alnum(char ch)
{
    return isalnum(to_8bits(ch));
}

inline bool is_digit(char ch)
{
    return isdigit(to_8bits(ch));
}

inline char to_lower(char ch)
{
    return tolower(to_8bits(ch));
}

inline char to_upper(char ch)
{
    return toupper(to_8bits(ch));
}

inline bool is_cntrl(char ch)
{
    return iscntrl(to_8bits(ch));
}

inline bool is_xdigit(char ch)
{
    return isxdigit(to_8bits(ch));
}

char hex_digit(unsigned ch);
int hex_to_int(char ch);
int hex_to_int(const string &s);

inline char safe_char(unsigned ch)
{
    ch = (ch & 0xff);
    if (iscntrl(ch) && !isspace(ch)) {
        ch = ' ';
    }
    return static_cast<char>(ch);
}

//
// Simple escape sequence encoding and decoding of string data.
//
void encode_string(const string &src, string &dst);
void decode_string(const string &src, string &dst);

const char *num_to_string(int num);
const char *num_to_string(double num);
const char *num_to_string3(int num);

extern const string EMPTY_STRING;

#endif // _util_h
