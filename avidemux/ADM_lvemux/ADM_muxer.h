//
// C++ Interface: ADM_muxer
//
// Description: 
//
//	Front end to lvemux low level muxer
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
class MpegMuxer
{
	void *packStream;
	int  audioType;
public:
		MpegMuxer(void );
		~MpegMuxer(  );
	uint8_t open( char *filename, uint32_t vbitrate, uint32_t fps1000, WAVHeader *audioheader);
	uint8_t writeAudioPacket(uint32_t len, uint8_t *buf);
	uint8_t writeVideoPacket(uint32_t len, uint8_t *buf);
	uint8_t close( void );

};
