//
// C++ Interface: ADM_lavformat
//
// Description: 
//
//	iface to libavformat	
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

class lavMuxer
{
protected:
		uint32_t _frameNo;
		uint32_t _fps1000;
		uint32_t _audioByterate;
		uint32_t _total;
		uint32_t _running;

public:
		lavMuxer(void );
		~lavMuxer(  );
	uint8_t needAudio( void );
	uint8_t open( char *filename, uint32_t vbitrate, aviInfo *info, WAVHeader *audioheader);
	uint8_t writeAudioPacket(uint32_t len, uint8_t *buf);
	uint8_t writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe );
	uint8_t forceRestamp(void);
	uint8_t close( void );
	uint8_t audioEmpty( void);

};
//EOF

