/***************************************************************************
                          adm_encmjpeg.h  -  description
                             -------------------
    begin                : Tue Jul 23 2002
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

#ifndef __ADM_encoder_mjpeg__
#define __ADM_encoder_mjpeg__

#ifdef USE_FFMPEG
typedef struct MJPEGConfig
{
		
		int		qual;
		int		swapped;
}MJPEGConfig;

class EncoderMjpeg: public Encoder
{
 	
	protected:
				
 							ffmpegEncoderFFMjpeg		*_codec;
							uint32_t						_q;
							uint8_t 						_swapped;
	public:
							EncoderMjpeg	( MJPEGConfig  *conf)  ;
							virtual uint8_t isDualPass( void ) { return 0;}; 			// mjpeg is always monopass
							virtual uint8_t configure(AVDMGenericVideoStream *instream) ;
							virtual uint8_t encode( uint32_t frame,uint32_t *len,uint8_t *out,uint32_t *flags);
							virtual uint8_t setLogFile( const char *p,uint32_t fr); // for dual pass only
							virtual uint8_t stop( void);
							virtual uint8_t startPass2( void ) 		 { assert(0);}; // for dual pass only
							virtual uint8_t startPass1( void ) 		 { assert(0);}; // for dual pass only
						  virtual const char *getCodecName(void ) {return "MJPG";}				
						  virtual const char *getFCCHandler(void ) {return "MJPG";}
							virtual const char *getDisplayName(void ) {return "Mjpeg";}						  
}   ;


#endif
#endif
