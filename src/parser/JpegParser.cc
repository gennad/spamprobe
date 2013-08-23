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

#include <stdexcept>
#include "AbstractTokenizer.h"
#include "MD5Digester.h"
#include "Message.h"
#include "StringReader.h"
#include "JpegParser.h"

JpegParser::JpegParser(Message *message,
		       AbstractTokenizer *tokenizer,
		       AbstractTokenReceiver *receiver,
		       const string &prefix,
		       const Buffer<unsigned char> &bytes)
  : ImageParser(message, tokenizer, receiver, prefix, bytes),
    m_initialized(false)
{
}

JpegParser::~JpegParser()
{
  if (m_initialized) {
    jpeg_destroy_decompress(&m_compressionInfo);
    m_initialized = false;
  }
}

bool JpegParser::parseImage()
{
  try {
    initializeSource();
    digestImage();
    tokenizeImage();
  } catch (runtime_error &ex) {
    return false;
  }
}

void JpegParser::tokenizeMarker(const string &base_token,
				jpeg_saved_marker_ptr marker)
{
  string prefix("_marker_");
  prefix += num_to_string(marker->marker);
  prefix += "_";
  string token;
  for (int i = 0; i < marker->data_length; ++i) {
    char ch = (char)marker->data[i];
    if (isprint(ch)) {
      token += ch;
    } else {
      token += '{';
      token += num_to_string(ch);
      token += '}';
    }
  }
  StringReader reader(token);
  m_tokenizer->tokenize(m_receiver, &reader, m_prefix + base_token + prefix);
}

void JpegParser::tokenizeImage()
{
  if (setjmp(m_error_mgr.return_buffer)) {
    throw runtime_error("jpeg error");
  }

  jpeg_save_markers(&m_compressionInfo, JPEG_COM, 1024);
  for (int i = 0; i <= 15; ++i) {
    jpeg_save_markers(&m_compressionInfo, JPEG_APP0 + i, 1024);
  }

  jpeg_read_header(&m_compressionInfo, TRUE);

  string base_token("image_0");
  for (jpeg_saved_marker_ptr marker = m_compressionInfo.marker_list; marker; marker = marker->next) {
    tokenizeMarker(base_token, marker);
  }
  sendToken(base_token + "_height_" + num_to_string((int)m_compressionInfo.image_width));
  sendToken(base_token + "_width_" + num_to_string((int)m_compressionInfo.image_height));
  sendToken(base_token + "_color_space_" + num_to_string(m_compressionInfo.jpeg_color_space));
  sendToken(base_token + "_num_components_" + num_to_string(m_compressionInfo.num_components));
}

static void mem_init_source(j_decompress_ptr cinfo)
{
}

static boolean mem_fill_input_buffer(j_decompress_ptr cinfo)
{
  if (cinfo->src->bytes_in_buffer > 0) {
    return TRUE;
  }
  ERREXIT(cinfo, JERR_FILE_READ);
}

static void mem_skip_input_data (j_decompress_ptr cinfo,
				 long num_bytes)
{
  while (num_bytes > 0 && cinfo->src->bytes_in_buffer > 0) {
    ++cinfo->src->next_input_byte;
    --cinfo->src->bytes_in_buffer;
    --num_bytes;
  }
}

static void mem_term_source(j_decompress_ptr cinfo)
{
}

void JpegParser::longjmp_error_exit(j_common_ptr cinfo)
{
  struct error_mgr *error_mgr = (struct error_mgr *)cinfo->err;
  longjmp(error_mgr->return_buffer, 1);
}

void JpegParser::initializeSource()
{
  m_compressionInfo.err = jpeg_std_error(&m_error_mgr.error_mgr);
  m_error_mgr.error_mgr.error_exit = longjmp_error_exit;
  if (setjmp(m_error_mgr.return_buffer)) {
    throw runtime_error("jpeg error");
  }
  jpeg_create_decompress(&m_compressionInfo);
  m_initialized = true;
  m_source.next_input_byte = m_bytes.get();
  m_source.bytes_in_buffer = m_bytes.length();
  m_source.init_source = mem_init_source;
  m_source.fill_input_buffer = mem_fill_input_buffer;
  m_source.skip_input_data = mem_skip_input_data;
  m_source.resync_to_restart = jpeg_resync_to_restart;
  m_source.term_source = mem_term_source;
  m_compressionInfo.src = &m_source;
}

#endif // HAVE_JPEG
