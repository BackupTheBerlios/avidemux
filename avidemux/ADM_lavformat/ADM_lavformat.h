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
        MUXER_NONE=0,
	MUXER_DVD,
	MUXER_VCD,
	MUXER_SVCD,
        MUXER_TS,
	MUXER_DUMMY
}ADM_MUXER_TYPE;


class ADMMpegMuxer
{
protected:
                uint32_t _frameNo;
                uint32_t _fps1000;
                uint32_t _audioByterate;
                uint32_t _total;
                uint32_t _running;
                ADM_MUXER_TYPE _type;
                uint8_t  _restamp;
                ADM_MUXER_TYPE _muxerType;

public:
        virtual uint8_t open( char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, 
                                aviInfo *info, WAVHeader *audioheader)=0;
        virtual uint8_t writeAudioPacket(uint32_t len, uint8_t *buf)=0;
        virtual uint8_t writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe )=0;
        virtual uint8_t forceRestamp(void)=0;
        virtual uint8_t close( void )=0;
        virtual uint8_t audioEmpty( void)=0;
        virtual uint8_t needAudio(void)=0;

                ADMMpegMuxer(void) {};
                virtual ~ADMMpegMuxer(void) {};

};
class lavMuxer : public ADMMpegMuxer
{

		

public:
		lavMuxer(void );
		~lavMuxer(  );
	virtual uint8_t open( char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader);
        virtual uint8_t writeAudioPacket(uint32_t len, uint8_t *buf);
        virtual uint8_t writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe );
        virtual uint8_t forceRestamp(void);
        virtual uint8_t close( void );
        virtual uint8_t audioEmpty( void);
        virtual uint8_t needAudio(void);

	

};

class mplexMuxer : public ADMMpegMuxer
{
protected:
               
public:
                mplexMuxer(void );
                ~mplexMuxer(  );
        virtual uint8_t open( char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader);
        virtual uint8_t writeAudioPacket(uint32_t len, uint8_t *buf);
        virtual uint8_t writeVideoPacket(uint32_t len, uint8_t *buf,uint32_t frameno,uint32_t displayframe );
        virtual uint8_t forceRestamp(void);
        virtual uint8_t close( void );
        virtual uint8_t audioEmpty( void);
        virtual uint8_t needAudio(void);

};

//EOF

