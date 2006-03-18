/***************************************************************************
                          ADM_divx4.h  -  description
                             -------------------

	Mpeg4 ****decoder******** using divx 5.0.5

    begin                : Wed Sep 25 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef USE_DIVX

#include "ADM_lavcodec.h"
#include "ADM_library/default.h"
#include "ADM_library/fourcc.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"

#include "ADM_codecs/ADM_divx4.h"
#include "ADM_gui/GUI_MPP.h"

#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_CODEC
#include "ADM_toolkit/ADM_debug.h"
#include "ADM_divxInc.h"
//________________________________________________
void
decoderDIVX::setParam (void)
{

  return;			// no param for ffmpeg
}
//-------------------------------
decoderDIVX::decoderDIVX (uint32_t w, uint32_t h):decoders (w, h)
{
  DEC_INIT decinit;
  // All default are good for us
  memset (&decinit, 0, sizeof (decinit));

  decinit.codec_version = 500;

  ADM_assert (DEC_OK == decore (&_handle, DEC_OPT_INIT, &decinit, NULL));

  DivXBitmapInfoHeader header;


  memset (&header, 0, sizeof (header));

  header.biSize = sizeof (DivXBitmapInfoHeader);
  //      header.biBitCount=24;
  header.biWidth = _w;
  header.biHeight = _h;
  header.biCompression = fourCC::get ((uint8_t *) "YV12");
  ADM_assert (DEC_OK == decore (_handle, DEC_OPT_SETOUT, &header, NULL));

  printf ("\n Divx 5.0.5 decoder initialized\n");


}

//_____________________________________________________

decoderDIVX::~decoderDIVX ()
{
  ADM_assert (DEC_OK == decore (_handle, DEC_OPT_RELEASE, NULL, NULL));
  _handle = NULL;
  printf ("Divx 5.0.5 destroyed\n");
}


uint8_t
  decoderDIVX::uncompress (uint8_t * in, uint8_t * out, uint32_t len,
			   uint32_t * flagz)
{
  DEC_FRAME decframe;
  DEC_FRAME_INFO decframeinfo;

  memset (&decframe, 0, sizeof (decframe));
  memset (&decframeinfo, 0, sizeof (decframeinfo));

  decframe.bmp = out;
  decframe.bitstream = in;
  decframe.length = len;
  decframe.stride = 0;
  decframe.render_flag = 1;

  if (DEC_OK != decore (_handle, DEC_OPT_FRAME, &decframe, &decframeinfo))
    {
      printf ("\n **ERROR decoding frame with divx !**\n");
      return 0;
    }
  if (flagz)
    {
      aprintf ("type : %d \n", decframeinfo.prediction_type);
      *flagz = 0;
    }
  return 1;
}

#endif
