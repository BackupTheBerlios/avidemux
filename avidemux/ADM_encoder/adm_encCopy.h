/***************************************************************************
                         Fake encoder used for copy mode

        We have to reorder !
        TODO FIXME

    begin                : Sun Jul 14 2002
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


class EncoderCopy:public Encoder
{

protected:
        uint32_t _frameStart;
        uint32_t _total;
        uint32_t _lastIPFrameSent;
public:
    EncoderCopy (COMPRES_PARAMS *codecconfig );
        ~EncoderCopy() ; // can be called twice if needed ..
  virtual uint8_t isDualPass (void);
  virtual uint8_t configure (AVDMGenericVideoStream * instream);
  virtual uint8_t encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags);
  virtual uint8_t encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags,uint32_t *displayFrame);
  virtual uint8_t setLogFile (const char *p, uint32_t fr);
  virtual uint8_t stop (void);
  virtual uint8_t startPass2 (void);
  virtual uint8_t startPass1 (void);
  virtual const char *getCodecName(void ) ;
  virtual const char *getFCCHandler(void ) ;
  virtual const char *getDisplayName(void ) ;
  virtual uint8_t     getLastQz( void);
  virtual uint8_t hasExtraHeaderData( uint32_t *l,uint8_t **data);
};


#endif
