///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: GifParser.h 272 2007-01-06 19:37:27Z brian $
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

#if defined(HAVE_UNGIF)

#ifndef _GifParser_h
#define _GifParser_h

extern "C" {
#include <gif_lib.h>
}
#include "ImageParser.h"
#include "Buffer.h"

class GifParser : public ImageParser
{
public:
  GifParser(Message *message,
            AbstractTokenizer *tokenizer,
            AbstractTokenReceiver *receiver,
            const string &prefix,
            const Buffer<unsigned char> &bytes);
  ~GifParser();

  virtual bool parseImage();

private:
  void openImage();
  void parseImageRecords();

  static int readFromBuffer(GifFileType *file,
                            GifByteType *dst,
                            int length);

private:
  /// Not implemented.
  GifParser(const GifParser &);

  /// Not implemented.
  GifParser& operator=(const GifParser &);

private:
  GifFileType *m_gif;
  int m_nextByteIndex;
};

#endif // _GifParser_h
#endif // HAVE_UNGIF
