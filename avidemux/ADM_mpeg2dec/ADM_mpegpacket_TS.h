//
// C++ Interface: ADM_mpegpacket_TS
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#define TS_PACKET 188

class ADM_mpegDemuxerTransportStream : public   ADM_mpegDemuxer
{
	  private : 
	  		
	  		int32_t		_pesLen,_pesRead;
	  		uint64_t 	_firstPacketOffset;
	            	uint64_t 	_packetOffset;
	              	uint64_t 	_packetLen;
	               	uint64_t 	_currentOffset;	
	                              
			uint8_t  	_otherStream;
			uint8_t  	_otherStreamSubId;

			uint32_t  	_otherLen;

			uint8_t 	_streamId;
			uint8_t 	_streamSubId;
	                  	                
			uint8_t		_buffer[TS_PACKET];
			uint8_t		_TSbuffer[TS_PACKET];
	                   
	                 mParser 	*parser;           
			uint8_t		_nextPacket(void);	
			uint32_t  	_skipPacketHeader( uint8_t sid,uint8_t *subid,
							uint32_t *pts,uint8_t start, uint8_t len,uint32_t *end );
			uint32_t  	_firstPTS;
			uint32_t  	_otherPTS;
    
	  public:
	  				ADM_mpegDemuxerTransportStream(uint8_t stream,uint8_t stream2) ;
		virtual			~ADM_mpegDemuxerTransportStream();       
	        virtual    uint32_t 	read(uint8_t *w,uint32_t len);
	        virtual    uint8_t 	goTo(uint64_t offset);	            	 
	        virtual    uint8_t   	open(char *name);
	        
		virtual uint8_t 	getpos(uint64_t *p);
		virtual uint8_t 	forward(uint32_t f);
           
		virtual	uint64_t	getAbsPos( void);
		virtual uint8_t		_asyncJump(uint64_t relative,uint64_t absolute);;		
		virtual uint8_t		_asyncJump2(uint64_t relative,uint64_t absolute);
		virtual uint32_t	getOtherSize(void) {return _otherLen;};  
		virtual void 		dump( void ) ;
		virtual int32_t 	getPTSDelta( void ) ;
};
#define PRIVATE_STREAM_1 	0xbd
#define MINUS_ONE 0xffffffff

