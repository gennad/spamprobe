///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MessageHeaderList.cc 272 2007-01-06 19:37:27Z brian $
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

#include "RegularExpression.h"
#include "MultiLineSubString.h"
#include "MultiLineJoinedString.h"
#include "MessageHeader.h"
#include "MessageHeaderList.h"

static const string CONTENT_TYPE("content-type");

CRef<AbstractMultiLineString> MessageHeaderList::header(const string &name) const
{
    MultiLineStringList values;
    for (int i = 0; i < m_headers.size(); ++i) {
        if (m_headers[i]->hasName(name)) {
            values.push_back(m_headers[i]->lines());
        }
    }

    if (values.size() == 0) {
        return CRef<AbstractMultiLineString>();
    }

    if (values.size() == 1) {
        return values[0];
    }

    return CRef<AbstractMultiLineString>(new MultiLineJoinedString(values));
}

const string &MessageHeaderList::getContentTypeString(string &buffer) const
{
    getHeaderString(CONTENT_TYPE, EMPTY_STRING, buffer);
    buffer = to_lower(buffer);
    return buffer;
}

const string &MessageHeaderList::getBoundaryString(string &buffer) const
{
    string header;
    getHeaderString(CONTENT_TYPE, EMPTY_STRING, header);
    return findParam(header, "boundary", EMPTY_STRING, buffer);
}

const string &MessageHeaderList::getCharsetString(string &buffer) const
{
    string header;
    getHeaderString(CONTENT_TYPE, EMPTY_STRING, header);
    findParam(header, "charset", EMPTY_STRING, buffer);
    buffer = to_lower(buffer);
    return buffer;
}

const string &MessageHeaderList::getHeaderString(const string &header_name,
                                                 const string &default_value,
                                                 string &buffer) const
{
    CRef<AbstractMultiLineString> header_strings(header(header_name));
    if (header_strings.isNull()) {
        buffer = default_value;
    } else {
        header_strings->join(buffer);
    }
    return buffer;
}

bool MessageHeaderList::hasBoundaryString() const
{
    string boundary_string;
    getBoundaryString(boundary_string);
    return boundary_string.length() > 0;
}

bool MessageHeaderList::isTextType() const
{
    string content_type;
    getContentTypeString(content_type);
    return content_type.length() == 0 || starts_with(content_type, "text");
}

bool MessageHeaderList::hasType(const string &type_name) const
{
    string content_type;
    getContentTypeString(content_type);
    return starts_with(content_type, type_name);
}

bool MessageHeaderList::isMessageType() const
{
  return hasType("message/rfc822");
}

const string &MessageHeaderList::findParam(const string &header,
                                           const string &param,
                                           const string &defaultValue,
                                           string &buffer) const
{
    RegularExpression param_regex(string("[^a-z]") + param + "[ \t]*=[ \t]*\"([^\"]+)\"", 2, true);
    if (param_regex.match(header)) {
        return param_regex.getMatch(1, buffer);
    }

    param_regex.setExpression(string("[^a-z]") + param + "[ \t]*=[ \t]*([^ \t;\"]+)", 2, true);
    if (param_regex.match(header)) {
        return param_regex.getMatch(1, buffer);
    }

    buffer = defaultValue;
    return buffer;
}

