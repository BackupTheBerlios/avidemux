/***************************************************************************
                          ADM_mpegparser.h  -  description
                             -------------------
    begin                : Tue Oct 15 2002
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
#ifndef MPARSER_
#define MPARSER_
#define MP_BUFFER 4096
#define MP_NOINLINE
 class mParser
{
	private:
	 
	    uint8_t  _buffer[MP_BUFFER];
	    uint64_t _off;
	    
	    uint32_t _curFd;	    
	    uint32_t _nbFd;
	    FILE		 **_fd;
	    uint64_t *_sizeFd;
	   
	public:
					     mParser(void);
							 ~mParser();					     
	     	 virtual uint8_t open(char *name);
	     	 virtual uint8_t forward(uint64_t u);
	      	virtual uint8_t sync(uint8_t *t );
	     	uint8_t getpos(uint64_t *o);
       		uint8_t setpos(uint64_t o);

       		uint8_t nearEnd(uint32_t w);
       		virtual uint64_t getSize( void ) ;
		virtual uint8_t read32(uint32_t l, uint8_t *buffer);
#ifndef MP_NOINLINE
       			uint32_t read32i(void )
			{
					uint32_t v;
					uint8_t c[4];
					read32(4,c);

					v= (c[0]<<24)+(c[1]<<16)+(c[2]<<8)+c[3];
					return v;
			}
			uint16_t read16i(void )
			{
					uint16_t v;
					uint8_t c[2];

					read32(2,c);
					v= (c[0]<<8)+c[1];
					return v;
			}
			uint8_t read8i(void )
			{
					uint8_t u;
					read32(1,(uint8_t *)&u);
					return u;
			}
#else
			uint32_t read32i(void );
			uint16_t read16i(void );
			uint8_t read8i(void );
#endif			


} ;
#define TS_PACKET 188
 class mParserTS : public mParser
{
	private:
	 	uint8_t		 bufferTS[TS_PACKET];
	   	uint32_t	 _tsPos,_tsSize;
		mParser		*parser;
		
		uint8_t  	fillTsBuffer( void );
	   
	public:
			     mParserTS(void);
			     ~mParserTS();
	     	
	     //	virtual uint8_t forward(uint64_t u);
	      	virtual uint64_t getSize( void ) { return parser->getSize();}
		virtual uint8_t read32(uint32_t l, uint8_t *buffer);       			
		virtual uint8_t open(char *name);
		virtual uint8_t forward(uint64_t u);// { return parser->forward(u);}
		virtual uint8_t sync(uint8_t *t );
} ;
typedef struct mpegAudioTrack
{
	uint32_t presence;
	uint32_t channels;
	uint32_t bitrate; 
	int32_t	 shift;

}mpegAudioTrack;

uint8_t  MpegaudoDetectAudio(char *mpeg, mpegAudioTrack *audioTrack);

#endif
