//
// C++ Interface: audioeng_libtoolame
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBTOOLME
#define LIBTOOLME
 
 //_____________________________________________
class AVDMProcessAudio_LibToolame : public AVDMBufferedAudioStream
{
protected:
	uint32_t grab(uint8_t *obuffer);   

public:
	uint8_t	init( uint32_t mode,
				 uint32_t bitrate);	
	virtual ~AVDMProcessAudio_LibToolame();				
	AVDMProcessAudio_LibToolame(AVDMGenericAudioStream *instream	);
};
#endif
