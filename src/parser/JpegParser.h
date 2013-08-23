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

#if defined(HAVE_JPEG)

#ifndef _JpegParser_h
#define _JpegParser_h

extern "C" {
#include <setjmp.h>
#include <jpeglib.h>
#include "jerror.h"
}
#include "ImageParser.h"
#include "Buffer.h"

class AbstractTokenizer;
class AbstractTokenReceiver;
class Message;

class JpegParser : public ImageParser
{
public:
  JpegParser(Message *message,
	    AbstractTokenizer *tokenizer,
	    AbstractTokenReceiver *receiver,
	    const string &prefix,
	    const Buffer<unsigned char> &bytes);
  ~JpegParser();

  virtual bool parseImage();

private:
  /// Not implemented.
  JpegParser(const JpegParser &);

  /// Not implemented.
  JpegParser& operator=(const JpegParser &);

  static void longjmp_error_exit(j_common_ptr cinfo);

  void initializeSource();
  void tokenizeImage();
  void tokenizeMarker(const string &base_token,
		      jpeg_saved_marker_ptr marker);

private:
  struct jpeg_decompress_struct m_compressionInfo;
  bool m_initialized;
  struct jpeg_source_mgr m_source;
  struct error_mgr {
    struct jpeg_error_mgr error_mgr;
    jmp_buf return_buffer;
  } m_error_mgr;
};

#endif // _JpegParser_h
#endif // HAVE_JPEG
