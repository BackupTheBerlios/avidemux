/***************************************************************************
                          ADM_mpegpacket_PS.h  -  description
                             -------------------
    begin                : Fri Oct 25 2002
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
#define  PES_BUFFER_SIZE 64*1024
 	class ADM_mpegDemuxerProgramStream : public   ADM_mpegDemuxer
 {
	  private : 
	  					uint64_t _firstPacketOffset;
	            		uint64_t _packetOffset;
	              	uint64_t _packetLen;
	               	uint64_t _currentOffset;	
	                              
                     uint8_t  _otherStream;
                     uint8_t  _otherStreamSubId;

                      uint32_t  _otherLen;

	                  uint8_t 	_streamId;
	                   uint8_t 	_streamSubId;
	                  
	                   uint8_t  _muxTypeMpeg2;
	                   uint8_t	_buffer[PES_BUFFER_SIZE];
	                   
	                 	mParser *parser;           
	                  uint8_t	_nextPacket(void);	
	                  uint32_t  _skipPacketHeader( uint8_t sid,uint8_t *subid,uint32_t *pts );
			  uint32_t  _firstPTS;
			  uint32_t  _otherPTS;
    
	  public:
	  					ADM_mpegDemuxerProgramStream(uint8_t stream,uint8_t stream2) ;
	       		  ~ADM_mpegDemuxerProgramStream() ;	       
	        virtual    uint32_t 	read(uint8_t *w,uint32_t len);
	        virtual    uint8_t 	goTo(uint64_t offset);	            	 
	        virtual    uint8_t   	open(char *name);
	        
   	         virtual uint8_t getpos(uint64_t *p);
        	  virtual uint8_t forward(uint32_t f);
           
             virtual	uint64_t		getAbsPos( void);
             virtual uint8_t			_asyncJump(uint64_t relative,uint64_t absolute);;
             virtual uint8_t			_asyncJump2(uint64_t relative,uint64_t absolute);;
             
             virtual uint32_t			getOtherSize(void) { return _otherLen;};  
                              virtual void dump( void ) ;
	     virtual int32_t 	getPTSDelta( void ) ;
	};

