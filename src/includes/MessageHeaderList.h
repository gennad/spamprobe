///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: MessageHeaderList.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _MessageHeaderList_h
#define _MessageHeaderList_h

#include <vector>
#include "MessageHeader.h"

class MessageHeaderList
{
public:
  MessageHeaderList()
  {
  }

  ~MessageHeaderList()
  {
  }

  const MessageHeader *header(int index) const
  {
    assert(index >= 0);
    assert(index < headerCount());
    return m_headers[index].ptr();
  }

  int headerCount() const
  {
    return m_headers.size();
  }

  void addHeader(OWNED MessageHeader *header)
  {
    m_headers.push_back(make_ref(header));
  }

  CRef<AbstractMultiLineString> header(const string &name) const;

  const string &getContentTypeString(string &buffer) const;
  const string &getBoundaryString(string &buffer) const;
  const string &getCharsetString(string &buffer) const;
  bool isTextType() const;
  bool hasType(const string &type_name) const;
  bool isMessageType() const;
  bool hasBoundaryString() const;

  const string &getHeaderString(const string &header_name,
                                const string &default_value,
                                string &buffer) const;

private:
  const string &findParam(const string &header,
                          const string &param,
                          const string &defaultValue,
                          string &buffer) const;

private:
  typedef vector<Ref<MessageHeader> > HeaderVector;

  HeaderVector m_headers;
};

#endif
