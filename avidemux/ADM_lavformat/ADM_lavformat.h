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
typedef enum
{
	MUXER_DVD,
	MUXER_VCD,
	MUXER_SVCD,
	MUXER_DUMMY
}ADM_MUXER_TYPE;
class lavMuxer
{
protected:
		uint32_t _frameNo;
		uint32_t _fps1000;
		uint32_t _audioByterate;
		uint32_t _total;
		uint32_t _running;
		ADM_MUXER_TYPE _type;

public:
		lavMuxer(void );
		~lavMuxer(  );
	uint8_t needAudio( void );
			// Inbitrate is in bps,  0 means let the muxer takes default 
			// value
	uint8_t open( char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader);
	uint8_t writeAudioPacket(uint32_t len, uint8_t *buf);
	uint8_t writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe );
	uint8_t forceRestamp(void);
	uint8_t close( void );
	uint8_t audioEmpty( void);

};
//EOF

