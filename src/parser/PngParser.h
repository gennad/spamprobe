///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id$
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

#if defined(HAVE_PNG)

#ifndef _PngParser_h
#define _PngParser_h

extern "C" {
#include <png.h>
}
#include "ImageParser.h"
#include "Buffer.h"

class AbstractTokenizer;
class AbstractTokenReceiver;
class Message;

class PngParser : public ImageParser
{
public:
  PngParser(Message *message,
	    AbstractTokenizer *tokenizer,
	    AbstractTokenReceiver *receiver,
	    const string &prefix,
	    const Buffer<unsigned char> &bytes);
  ~PngParser();

  virtual bool parseImage();

private:
  void initializeImage();
  void tokenizeImage();
  static void receiveHeader(png_structp png_ptr, png_infop info);

private:
  /// Not implemented.
  PngParser(const PngParser &);

  /// Not implemented.
  PngParser& operator=(const PngParser &);

private:
  png_structp m_pngPtr;
  png_infop m_infoPtr;
  bool m_initialized;
};

#endif // _PngParser_h
#endif // HAVE_PNG
