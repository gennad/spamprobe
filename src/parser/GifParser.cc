///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: GifParser.cc 272 2007-01-06 19:37:27Z brian $
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

#include <stdexcept>
#include "AbstractTokenizer.h"
#include "MD5Digester.h"
#include "Message.h"
#include "StringReader.h"
#include "GifParser.h"

static int throw_on_error(const char *function_name,
                          int rc)
{
  if (rc != GIF_ERROR) {
    return rc;
  }

  static string message(function_name);
  message += ": error: ";
  message += num_to_string(rc);
  throw runtime_error(message);
}

int GifParser::readFromBuffer(GifFileType *file,
                              GifByteType *dst,
                              int length)
{
  GifParser *parser = (GifParser *)file->UserData;
  int remaining = parser->m_bytes.length() - parser->m_nextByteIndex;
  if (remaining == 0) {
    return 0;
  }

  if (length > remaining) {
    length = remaining;
  }
  memcpy(dst, parser->m_bytes.get() + parser->m_nextByteIndex, length);
  parser->m_nextByteIndex += length;
  return length;
}

GifParser::GifParser(Message *message,
                     AbstractTokenizer *tokenizer,
                     AbstractTokenReceiver *receiver,
                     const string &prefix,
                     const Buffer<unsigned char> &bytes)
  : ImageParser(message, tokenizer, receiver, prefix, bytes),
    m_gif(0)
{
}

GifParser::~GifParser()
{
  if (m_gif) {
    DGifCloseFile(m_gif);
  }
}

bool GifParser::parseImage()
{
  try {
    openImage();
    digestImage();
    parseImageRecords();
  } catch (runtime_error &ex) {
    return false;
  }
}

void GifParser::openImage()
{
  m_nextByteIndex = 0;
  m_gif = DGifOpen(this, readFromBuffer);
  if (!m_gif) {
    throw runtime_error("open gif failed");
  }
}

void GifParser::parseImageRecords()
{
  GifRecordType rec_type;

  int max_loops = 1500;
  int image_num = 0;
  while ((max_loops-- > 0) && (throw_on_error("DGifGetRecordType", DGifGetRecordType(m_gif, &rec_type)) != TERMINATE_RECORD_TYPE)) {
    string base_token("image_");
    base_token += num_to_string(image_num);
    switch (rec_type) {
    case IMAGE_DESC_RECORD_TYPE:
      {
        throw_on_error("DGifGetImageDesc", DGifGetImageDesc(m_gif));
        sendToken(base_token + "_height_" + num_to_string(m_gif->Image.Height));
        sendToken(base_token + "_width_" + num_to_string(m_gif->Image.Width));
        sendToken(base_token + "_left_" + num_to_string(m_gif->Image.Left));
        sendToken(base_token + "_top_" + num_to_string(m_gif->Image.Top));
        sendToken(base_token + (m_gif->Image.Interlace ? "_interlaced" : "_noninterlaced"));
        sendToken(base_token + (m_gif->Image.ColorMap ? "_color_map" : "_no_color_map"));
        if (m_gif->Image.ColorMap) {
          sendToken(base_token + "_bpp_" + num_to_string(m_gif->Image.ColorMap->BitsPerPixel));
          int len = 1 << m_gif->Image.ColorMap->BitsPerPixel;
          for (int i = 0; i < len; i+= 4) {
            for (int j = 0; j < 4 && j < len; ++j) {
              sendToken(base_token + "_color_red_" + num_to_string(m_gif->Image.ColorMap->Colors[i + j].Red));
              sendToken(base_token + "_color_green_" + num_to_string(m_gif->Image.ColorMap->Colors[i + j].Green));
              sendToken(base_token + "_color_blue_" + num_to_string(m_gif->Image.ColorMap->Colors[i + j].Blue));
            }
          }
        }

        // skip the image
        int code_size;
        GifByteType *code_block;
        throw_on_error("DGifGetCode", DGifGetCode(m_gif, &code_size, &code_block));
        while (code_block) {
          throw_on_error("DGifGetCodeNext", DGifGetCodeNext(m_gif, &code_block));
        }

        ++image_num;
      }
      break;

    case EXTENSION_RECORD_TYPE:
      {
        int ext_code;
        GifByteType *extension;
        throw_on_error("DGifGetExtension", DGifGetExtension(m_gif, &ext_code, &extension));
        sendToken(base_token + "_extcode_" + num_to_string(ext_code));
        while (extension) {
          const int slen = (int)extension[0];
          string prefix(base_token);
          prefix += "_extchars_";
          string token;
          for (int i = 1; i <= slen; ++i) {
            if (isprint(extension[i])) {
              token += extension[i];
            } else {
              token += '{';
              token += num_to_string(extension[i]);
              token += '}';
            }
          }
          sendToken(prefix + token);
          StringReader reader(token);
          m_tokenizer->tokenize(m_receiver, &reader, m_prefix + prefix);
          throw_on_error("DGifGetExtensionNext", DGifGetExtensionNext(m_gif, &extension));
        }
      }
      break;

    default:
      break;
    }
  }
}

#endif // HAVE_UNGIF
