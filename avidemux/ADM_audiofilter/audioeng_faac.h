
/***************************************************************************
                          audioeng_faac.h  -  description
                             -------------------
			     Interface to libfaac
    begin                : Sun 11 sept
    copyright            : (C) 2002-4 by mean
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
#ifndef ADMaudioAAC
#define ADMaudioAAC
 
 //_____________________________________________
class AVDMProcessAudio_Faac : public AVDMBufferedAudioStream
{
protected:
         uint32_t 	_size;
	 uint32_t	_incoming_frame;
         uint32_t 	grab(uint8_t *obuffer);
	 void  		*_handle;
	 uint8_t	dropBuffer[SIZE_INTERNAL];
public:
			uint8_t	init( uint32_t bitrate);		
		virtual ~AVDMProcessAudio_Faac();				
			AVDMProcessAudio_Faac(AVDMGenericAudioStream *instream);	
     		virtual uint8_t	getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
		virtual uint8_t packetPerFrame( void) {return 1;}
};

#endif
