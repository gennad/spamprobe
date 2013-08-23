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

#include <stdexcept>
#include "AbstractTokenizer.h"
#include "MD5Digester.h"
#include "Message.h"
#include "StringReader.h"
#include "PngParser.h"

PngParser::PngParser(Message *message,
                     AbstractTokenizer *tokenizer,
                     AbstractTokenReceiver *receiver,
                     const string &prefix,
                     const Buffer<unsigned char> &bytes)
  : ImageParser(message, tokenizer, receiver, prefix, bytes),
    m_pngPtr(0),
    m_infoPtr(0),
    m_initialized(false)
{
}

PngParser::~PngParser()
{
  if (m_initialized) {
    if (m_infoPtr) {
      png_destroy_read_struct(&m_pngPtr, &m_infoPtr, png_infopp_NULL);
    } else {
      png_destroy_read_struct(&m_pngPtr, png_infopp_NULL, png_infopp_NULL);
    }
  }
}

bool PngParser::parseImage()
{
  try {
    digestImage();
    initializeImage();
  } catch (runtime_error &ex) {
    return false;
  }
}

void PngParser::initializeImage()
{
  m_pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!m_pngPtr) {
    throw runtime_error("allocate png ptr failed");
  }

  m_initialized = true;
  m_infoPtr = png_create_info_struct(m_pngPtr);
  if (!m_infoPtr) {
    throw runtime_error("allocate info ptr failed");
  }

  if (setjmp(png_jmpbuf(m_pngPtr))) {
    throw runtime_error("png error");
  }

  png_set_progressive_read_fn(m_pngPtr, this, receiveHeader, NULL, NULL);
  png_process_data(m_pngPtr, m_infoPtr, m_bytes.get(), m_bytes.length());
}

void PngParser::receiveHeader(png_structp png_ptr,
                              png_infop info)
{
  PngParser *parser = (PngParser *)png_get_progressive_ptr(png_ptr);
  parser->tokenizeImage();
}

void PngParser::tokenizeImage()
{
  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;

  png_get_IHDR(m_pngPtr, m_infoPtr, &width, &height, &bit_depth, &color_type,
               &interlace_type, int_p_NULL, int_p_NULL);

  string base_token("image_0");
  sendToken(base_token + "_height_" + num_to_string((int)width));
  sendToken(base_token + "_width_" + num_to_string((int)height));
  sendToken(base_token + "_bit_depth_" + num_to_string((int)bit_depth));
  sendToken(base_token + "_color_type_" + num_to_string(color_type));
  sendToken(base_token + "_interlace_type_" + num_to_string(interlace_type));

  png_start_read_image(m_pngPtr);
}

#endif // HAVE_PNG
