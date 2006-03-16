//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author:Mean, fixounet@free.fr
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ENC_264
#define _ENC_264

#ifdef USE_X264
#include "ADM_codecs/ADM_x264.h"
typedef struct X264Config
{
    COMPRES_PARAMS  generic;
    ADM_x264Param   specific;         
}X264Config;

class EncoderX264: public Encoder
{
        
  protected:
                                
    X264Encoder            *_codec;
    char                    *_logfile;
    uint32_t                _q;
    uint32_t                _pass1Done;
    uint32_t                _fps1000;
    COMPRES_PARAMS          _param;
    ADM_x264Param           _codecParam;
    uint32_t                _totalframe;
  public:
    EncoderX264    ( COMPRES_PARAMS  *conf)  ;
    ~EncoderX264    ()  ;
    virtual uint8_t isDualPass( void ) ;                       // mjpeg is always monopass
    virtual uint8_t configure(AVDMGenericVideoStream *instream) ;
    virtual uint8_t encode( uint32_t frame,uint32_t *len,uint8_t *out,uint32_t *flags);
    virtual uint8_t encode( uint32_t frame,uint32_t *len,uint8_t *out,uint32_t *flags,uint32_t *displayFrameNo);
   
    virtual uint8_t setLogFile( const char *p,uint32_t fr); // for dual pass only
    virtual uint8_t stop( void);
    virtual uint8_t startPass2( void )  ;
    virtual uint8_t startPass1( void )   ;
    virtual const char *getCodecName(void ) {return "H264";}                              
    virtual const char *getFCCHandler(void ) {return "H264";}
    virtual const char *getDisplayName(void ) {return "X264";} 
    virtual uint8_t    hasExtraHeaderData( uint32_t *l,uint8_t **data);
    virtual uint8_t    getLastQz(void);
}   ;


#endif
#endif
