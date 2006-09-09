/***************************************************************************
                         Requantizer encoder
                         
    copyright            : (C) 2002/2003 by mean
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
#ifndef __ADM_encoder_copy__
#define __ADM_encoder_copy__


class EncoderRequant:public Encoder
{

protected:
  uint32_t _frameStart;
  uint32_t _total;
  uint32_t _lastIPFrameSent;
  uint8_t   code(uint32_t frame,uint8_t *out, uint32_t *outlen);
  uint8_t   *_buffer;
public:
          EncoderRequant (COMPRES_PARAMS * codecconfig);
          ~EncoderRequant ();		// can be called twice if needed ..
  virtual uint8_t isDualPass (void);
  virtual uint8_t configure (AVDMGenericVideoStream * instream);
  virtual uint8_t encode (uint32_t frame, ADMBitstream *out);
  virtual uint8_t setLogFile (const char *p, uint32_t fr);
  virtual uint8_t stop (void);
  virtual uint8_t startPass2 (void);
  virtual uint8_t startPass1 (void);
  virtual const char *getCodecName (void);
  virtual const char *getFCCHandler (void);
  virtual const char *getDisplayName (void);
  virtual uint8_t hasExtraHeaderData (uint32_t * l, uint8_t ** data);
          uint32_t getNbFrame(void) {return _total;} //<Must be called only after configure call !
};


#endif
