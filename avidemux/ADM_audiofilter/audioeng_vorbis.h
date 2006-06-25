
/***************************************************************************
                          audioeng_vorbish  -  description
                             -------------------
			     Interface to livorbis
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
#ifndef ADMaudioVorbis
#define ADMaudioVorbis
 
 //_____________________________________________
class AVDMProcessAudio_Vorbis : public AVDMBufferedAudioStream
{
protected:
         uint32_t 	_size;
	 uint32_t	_incoming_frame;
         uint32_t 	grab(uint8_t *obuffer);
	 void  		*_handle;
	 float		dropBuffer[SIZE_INTERNAL];
	 uint64_t	_oldpos;
	 uint32_t	_extraLen;
	 uint8_t	*_extraData;
	 
public:
			uint8_t	init( uint32_t bitrate, uint8_t mode);		
		virtual ~AVDMProcessAudio_Vorbis();				
			AVDMProcessAudio_Vorbis(AVDMGenericAudioStream *instream);	
     		virtual uint8_t	getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);		
		virtual uint8_t	extraData(uint32_t *l,uint8_t **d);
};

#endif
