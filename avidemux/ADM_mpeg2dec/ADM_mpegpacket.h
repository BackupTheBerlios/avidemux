/***************************************************************************
                          ADM_mpegpacket.h  -  description
                             -------------------
		Demuxer for mpeg
				- Elementary : Does nothing
				- Program		: Demux program stream
				
				!!!! Warning, the goTo is absolute to the file, not relative to the stream we are intersted in                             
                             
                             
    begin                : Thu Oct 17 2002
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

 #ifndef ADM_mpgpack
 #define ADM_mpgpack
 #include "ADM_mpegindexer/ADM_mpegparser.h"
 
 class ADM_mpegpacket
 {
	  private : 
	          //  ADM_mpegpacket *_next;
	  
	  
	  public:
	            uint64_t _abs;					// abs position in file
	            
	            uint64_t _rel;          // relative position in mpeg stream
	            uint64_t _size;
	 
	            
	 
	};
class ADM_mpegDemuxer
 {
	  protected : 
	          
	            uint64_t  _size;
	            uint64_t _pos;
	            uint8_t   _lastErr;
	            		  
	  public:
	  					ADM_mpegDemuxer();
		virtual    ~ADM_mpegDemuxer();	       
		virtual    uint32_t 	read(uint8_t *w,uint32_t len)=0;
	         	   uint32_t 	read(uint32_t len,uint8_t *w) { return read(w,len);};
		virtual    uint8_t 	goTo(uint64_t offset)=0;	            	 
		virtual    uint8_t   	open(char *name)=0;
	         
		virtual uint8_t 	getpos(uint64_t *p)=0;
		virtual uint8_t 	forward(uint32_t f)=0;
	         
            	virtual uint8_t		sync( uint8_t *stream);
   	        virtual uint8_t		read8i(void);
   	        virtual uint16_t	read16i(void);
		virtual uint32_t	read32i(void);
            
		virtual	uint64_t	getAbsPos( void)=0;
            
		virtual uint8_t		_asyncJump(uint64_t relative,uint64_t absolute)=0;
		virtual uint32_t	getOtherSize(void)=0;  
		virtual	uint64_t	getSize( void);
		virtual void 		dump( void ) {};
		virtual int32_t 	getPTSDelta( void ) { return 0;};
	};
	
class ADM_mpegDemuxerElementaryStream : public   ADM_mpegDemuxer
 {
	  private : 
			    FILE 			*_vob;
	  public:
	  					ADM_mpegDemuxerElementaryStream() ;
	       	virtual    ~ADM_mpegDemuxerElementaryStream() ;	       
	        virtual    uint32_t 		read(uint8_t *w,uint32_t len);
	        virtual    uint8_t 		goTo(uint64_t offset);	            	 
	        virtual    uint8_t   		open(char *name);
	         virtual uint8_t getpos(uint64_t *p);
	         virtual uint8_t forward(uint32_t f);
	          
            virtual	uint64_t		getAbsPos( void);
            virtual uint8_t			_asyncJump(uint64_t relative,uint64_t absolute);
             virtual uint32_t			getOtherSize(void) { return 0;};  

	};
	

#endif
