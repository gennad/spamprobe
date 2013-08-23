///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: HtmlTokenizer.h 272 2007-01-06 19:37:27Z brian $
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

#ifndef _HtmlTokenizer_h
#define _HtmlTokenizer_h

#include "AbstractCharReader.h"
#include "AbstractTokenizer.h"
#include "AbstractTokenReceiver.h"

class AbstractCharReader;

class HtmlTokenizer : public AbstractTokenizer, AbstractCharReader
{
public:
  HtmlTokenizer(AbstractTokenizer *textTokenizer,
                AbstractTokenizer *htmlTokenizer,
                int maxTagLength);

  virtual ~HtmlTokenizer();

  virtual void tokenize(AbstractTokenReceiver *receiver,
                        AbstractCharReader *reader,
                        const string &prefix);

  virtual bool forward();
  virtual bool hasChar();
  virtual bool atEnd();
  virtual bool skip(int nchars);

protected:
  virtual void processTagUrls(const string &tag);
  virtual void processTagBody(const string &tag);
  virtual bool isCommentTag(const string &tag);
  virtual bool isInvisibleTag(const string &tag);
  virtual bool isOpenCommentTag(const string &tag);
  virtual bool isIncompleteCommentTag(const string &tag);
  virtual bool processedTag(string &tag);
  char parseEntityInteger(const string &entity);
  bool processEntity(const string &entity);
  virtual bool processedEntity();
  const string &decodeUrl(const string &url,
                          string &buffer);

private:
  // configuration options initialized during construction
  // lifetime of these objects must exceed this object's lifetime
  AbstractTokenizer *m_textTokenizer;
  AbstractTokenizer *m_tagTokenizer;
  int m_maxTagLength;

  // temporary variables used during tokenizing
  string m_prefix;
  AbstractCharReader *m_reader;
  AbstractTokenReceiver *m_receiver;
};

#endif
