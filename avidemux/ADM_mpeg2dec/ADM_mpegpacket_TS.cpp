/***************************************************************************
                          ADM_mpegpacket_PS.cpp  -  description
                             -------------------
    begin                : Sat Nov 2 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Small TS demuxer
    It extracts the ES from the TS packet 
    
    TS PACKET=188    4 bytes header
    ===============
    
    Sync/8 = 0x47
    
    Trans error /1
    Payload Start/1
    Priority/1
    PID / 13
    
    Scrambling/2
    Payload/2
    continuity/4
    
    
            
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"

#include "ADM_mpegindexer/ADM_mpegparser.h"

#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_TS.h"



#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_toolkit/ADM_debug.h"


//
#define BUFFERED
//
#define USER_DATA_START_CODE 	0xb2
#define PACK_START_CODE		0xba
#define SYSTEM_START_CODE       0xbb

#define PRIVATE_STREAM_2 	0xbf
#define SYSTEM_END_CODE		0xb9


static uint64_t _lastSync;
extern void mixDump(uint8_t *i,uint32_t l);

//#define PRINT_PTS
//_______________________________________________________
//_______________________________________________________
//_______________________________________________________
//	Transport stream demuxer
//_______________________________________________________
//_______________________________________________________
//_______________________________________________________
ADM_mpegDemuxerTransportStream::ADM_mpegDemuxerTransportStream(uint8_t stream,uint8_t stream2) 
 {
		printf("\n Transport stream demuxer initialized with stream 1 = %x",stream);
		printf("stream 2 = %x\n",stream2);
	 
		parser=NULL;
			
		_firstPacketOffset=_packetOffset=0;
		_packetLen=_currentOffset=0;
		_otherLen=0;
			
		 _otherStream=stream2;
		 _firstPTS=_otherPTS=MINUS_ONE;
		_otherLen=0;

		if((stream<8) || (stream<0xA8 && stream>=0xA0))
		{
			_streamId=PRIVATE_STREAM_1;
			_streamSubId=stream;
		}
		else
		{
			_streamId=stream;
			_streamSubId=0xff;
		}		
		if((stream2<8) || (stream2<0xA8 && stream2>=0xA0))
		{
			_otherStream=PRIVATE_STREAM_1;
			_otherStreamSubId=stream2;
		}
		else
		{
		      _otherStream=stream2;
		     _otherStreamSubId=0xff;
		      
		}

		printf(" Internal  stream 1 = %x",_streamId);
		printf("stream 1s= %x\n",_streamSubId);
		printf(" Internal  stream 2 = %x",_otherStream);
		printf("stream 2s= %x\n",_otherStreamSubId);
}
int32_t 	ADM_mpegDemuxerTransportStream::getPTSDelta( void ) 
{
	if(_firstPTS==MINUS_ONE) return 0;
	if(_otherPTS==MINUS_ONE) return 0;
	
	double delta;
	
	delta=_firstPTS;
	delta-=_otherPTS;
	delta/=90.;
	printf("\n>>Delta PTS = %f ms<<\n",delta);
	return (int32_t)floor(delta);
}
 
 ADM_mpegDemuxerTransportStream::~ADM_mpegDemuxerTransportStream() 
 {
		if(parser) delete parser;			
		parser=NULL;
			
}
	  
	  
uint8_t ADM_mpegDemuxerTransportStream::open(char *name)
{
	
	//uint8_t subid;
	
		parser=NULL;
		parser=new mParser();
		if(!parser->open(name)) 
		{
			return 0;
		}
		_size=parser->getSize();
		printf("\n Size of PES stream : %llu",_size);
		// search the first video packet....
		if(!_nextPacket())
				return 0;
				
		_firstPacketOffset=_lastSync;	
	
		printf("\n Demuxer : %llx first,  %x\n",    _firstPacketOffset,_streamId);
		parser->setpos(	_firstPacketOffset);	
		_packetOffset=0;
		_currentOffset=0;
		_packetLen=0;
		return 1;	
}

//_________________________________________________________________________
uint32_t ADM_mpegDemuxerTransportStream::read(uint8_t *w,uint32_t len)
{
uint32_t leftover;
			leftover=    _packetLen-_currentOffset;
			// everything in current packet ?
			if(leftover	> len)
				{
						memcpy(w,_buffer+_currentOffset,len);
						_currentOffset+=len;	
						return len;
				}
			// else drop what's in this packet
			if(leftover)
			{
				memcpy(w,_buffer+_currentOffset,leftover);
				_currentOffset+=leftover;
			}
			if(!_nextPacket())
			{
				printf("\n**error reading packet**\n");
				_lastErr=1;
				return 0;
			}
			return leftover+read(w+leftover,len-leftover); // yes recursive is good for you !
}
//_________________________________________________________________________

uint8_t ADM_mpegDemuxerTransportStream::getpos(uint64_t *p)
{
			*p=  _packetOffset+ _currentOffset;
			return 1;
		
}
//_________________________________________________________________________
// Despite the name, offset refers to the absolute position in a ES file
//
uint8_t ADM_mpegDemuxerTransportStream::goTo(uint64_t offset)
{
uint8_t r;

	if(offset==0)
	{
		parser->setpos(_firstPacketOffset);
		_packetOffset=0;
		_packetLen=0;
		_currentOffset=0;
		r= _nextPacket();
              	return r;
	}

	// Means we have to go backward
	if(offset<_packetOffset)
	{
		printf("Rewinding, that can take a while...(%lx/%lx)\n",offset,_packetOffset);
		parser->setpos(_firstPacketOffset);
		_packetOffset=0;
		_packetLen=0;			
		_currentOffset=0;						
		while(1)
		{
			if(!_nextPacket()) return 0;
			if(_packetOffset+_packetLen>offset) break; // > ?? FIXME
					
		}
		_currentOffset=offset-_packetOffset;
       		return 1;
        }
        else
        {        // is in in the current packet ?
		if( offset<  _packetOffset+_packetLen)
		{
          		_currentOffset=offset-_packetOffset;
               		return 1;
		}
		else // somewhere afterward
		{
		   uint32_t skip;
//				printf("\n forward...\n");
							   
			   while(1)
			  {			   
				if(!_nextPacket()) return 0;
				skip=offset-_packetOffset;
				if(skip<_packetLen)
			      	{
					_currentOffset=skip;
					return 1;		 
				}
			   };
		}
	}
}
uint8_t ADM_mpegDemuxerTransportStream::forward(uint32_t f)
{	
//	printf("\n forward  : %lu\n",f);
		while( f >= _packetLen-_currentOffset)
		{
				f-=_packetLen-_currentOffset;
				if(!_nextPacket()) return 0;
		} ;
		_currentOffset+=f;

		return 1;
		
}
//	
//  		Search the next packet
//
uint8_t  ADM_mpegDemuxerTransportStream::_nextPacket(void)
{
	uint8_t stream,subid;
	uint32_t len;
	uint32_t pts=0;
	uint32_t pid,adapt,start;
	
		if(_lastErr) return 0;	 
	   	_packetOffset+=_packetLen;
		_packetLen=0;
		// search the first video packet....
		while(1)
		{
			if(!parser->read32(TS_PACKET,_TSbuffer))
			{
					printf("\nTS: could not read!\n");
					return 0;			
			}
			if(_TSbuffer[0]!=0x47)
			{
				printf("TS: no ts sync word\n");
				return 0;
			}
			// used to mark first packet && async jump
			parser->getpos(& _lastSync);
			
			// check we are not at the end of the stream
			if(_lastSync==_size) 
				{
					printf("End of file reached\n");
					return 0;				
				}
			_lastSync-=TS_PACKET;
			// get pid
			pid=((_TSbuffer[1]<<8)+_TSbuffer[2]) & 0x1FFF;
			
			if(pid!=0x10 && pid!=0x11) continue;
			adapt=(_TSbuffer[3]&0x30)>>4;
			if(!(adapt&1)) continue; // no payload
			start=4;
			len=184;
			if(adapt==3) //payload + adapt
			{			
				start+=1+_TSbuffer[4];
				len-=1+_TSbuffer[4];
			}
			// We got a packet of the correct pid
			if(_TSbuffer[1]&0x40) // PES header ?
			{
				len=  _skipPacketHeader(stream,&subid,&pts,start,len,&start);
			}
			if(!len)
			{
				printf("<+>");
				continue;
			}
			if(0 && pid==0x10)
			{
				mixDump(_TSbuffer,16);
				mixDump(_TSbuffer+TS_PACKET-16,16);
			}
			switch(_streamId)
			{
				case 0xe0 : 
						if(pid==0x11) { _otherLen+=len;continue;};
						if(pid!=0x10) {continue; }		
						break;
				case 0xbd: 	
						if(pid==0x10) {_otherLen+=len;continue;}
						if(pid!=0x11) continue;						
						break;
				default:	continue;
			
			}
			_packetLen=len;
			memcpy(_buffer,_TSbuffer+start,len);			
			_currentOffset=0;
			return 1;
		
		};
		return 0;
}
/*  ______________________________________________________
   	In case of mpeg TS
	4 Bytes header 0x47 xx xx xx xx
	Adapt layer (padding) if flag
	Normally a PES packet always start at a TS packet boundary
	It makes thing simpler :)
	
	PES 
		PES header is flag 00 00 01 XX xxx
		or cont. packet data if not set
	return the data left in the TS packet
    ______________________________________________________
*/
uint32_t  ADM_mpegDemuxerTransportStream::_skipPacketHeader( uint8_t sid,uint8_t *subid,
					uint32_t *pts,uint8_t start, uint8_t totallen,uint32_t *end )
{


uint8_t align=0;
uint16_t tag,ptsdts;
		// We got fitst 00 00 01 ID	
		*subid=0xff;
		*pts=MINUS_ONE;
		
		uint64_t pos;
		parser->getpos(&pos);		
		if(_TSbuffer[start] || _TSbuffer[start+1] || _TSbuffer[start+2]!=01)
		{
			printf("TS: No 00 00 01 xx as PES header\n");		
			return 0;
		}
/*		
		if(_TSbuffer[start+3]==PRIVATE_STREAM_1) //0xbd
		{
			return totallen-4;
		
		}
		if((_TSbuffer[start+3]&0xe0)!=0xe0)
		{
			printf("TS: No 01 E0 as PES header\n");		
			return 0;
		}	
*/
		// 00 00 01 E0	
		totallen-=4;
		start+=4;			
		// size 2 bytes (ignored)
		if((_TSbuffer[start+3]&0xe0)==_streamId)
		{
			if(_streamId==0xe0) _pesLen=-1;
			else
				_pesLen=(_TSbuffer[start]<<8)+_TSbuffer[start+1];			
		}	
		totallen-=2;
		start+=2;	
		// scrambling 1 byte
		totallen-=1;
		start+=1;	
		//PTS/DST 1 byte
		ptsdts=_TSbuffer[start];			
		start++;
		totallen--;
		//flags 1 byte
		start++;
		totallen--;
		
		if(ptsdts )
		{
			if(ptsdts&0x3F)
			{
				printf("More data : %x\n",ptsdts);
			}
			// Nb of bytes (ignored), we actually should use it
			ptsdts>>6;
			if(ptsdts&1)
			{
				start+=5;
				totallen-=5;
				_pesLen-=5;
			}
			if(ptsdts&2)
			{
				start+=5;
				totallen-=5;
				_pesLen-=5;
			}
		}
	*end=start;
    	return totallen;
}



uint64_t		ADM_mpegDemuxerTransportStream::getAbsPos( void)
{
     uint64_t i;
     parser->getpos(&i);     
     i=i-_packetLen+_currentOffset;
     return i;
}
uint8_t			ADM_mpegDemuxerTransportStream::_asyncJump2(uint64_t relative,uint64_t absolute)
{
	uint64_t backward,ref;	
	uint8_t  backBuffer[2*TS_PACKET];
		
		if(absolute<TS_PACKET) 	backward=0;
		else			backward=absolute-TS_PACKET;
		parser->setpos(backward);
		// and read
		parser->read32(2*TS_PACKET,backBuffer);
		// Search from the middle to the beginning for a 0x47 (sync)
		uint32_t search=TS_PACKET-1;
		int found=0;
		while(search)
		{
			if(backBuffer[search]==0x47)
			{	// Next one is also a sync ?
				if(backBuffer[search+TS_PACKET]==0x47) 
				{
					found=1;
					break;
				}
			}		
			search--;
		}
		if(!found)
		{
			printf("TS: Cannot jump2 to something\n");
			return 0;
		
		}
		parser->setpos(backward+search);
		_packetOffset=relative;
		_packetLen=0;
		_currentOffset=0;
		//printf("\n new relative : %lu",_packetOffset);
		return _nextPacket();
}
//
//		The sync point is a byte which has
//			- The absolute position "absolute" in the file
//			- The relative position "relative" in the es
//
//		Jump to it and update everything
//		Normally a PES packet starts at a TS packet boundary
//		(for video at last)
//		That makes thing simpler
//
uint8_t	ADM_mpegDemuxerTransportStream::_asyncJump(uint64_t relative,uint64_t absolute)
{
		uint64_t backward,ref;
		uint64_t pos;
		
		uint8_t  backBuffer[2*TS_PACKET];
		
		if(absolute<TS_PACKET) 	backward=0;
		else			backward=absolute-TS_PACKET;
		parser->setpos(backward);
		// and read
		parser->read32(2*TS_PACKET,backBuffer);
		// Search from the middle to the beginning for a 0x47 (sync)
		uint32_t search=TS_PACKET-1;
		int found=0;
		while(search)
		{
			if(backBuffer[search]==0x47)
			{	// Next one is also a sync ?
				if(backBuffer[search+TS_PACKET]==0x47) 
				{
					found=1;
					break;
				}
			}		
			search--;
		}
		if(!found)
		{
			printf("Packet not found :     \n");
			printf(" abs %lld \n",absolute);
			printf(" ref %lld \n",relative);
			assert(0);
				
		}
		// found it, now resync just before
		// and update the positioning
		
		// we have now the packet in which is the jump point
		// Time to do something finer
		parser->setpos(backward+search);
		//  we have the absolute position of the end marker
		//  end is the last byte of the current TS packet
		uint64_t end=backward+search+TS_PACKET-1;
		// delta is the # of bytes left in current packet AFTER sync point
		uint64_t delta=end-absolute;
		// fill packet
		if(!_nextPacket()) return 0;
		// Offset in current packet is 
		_currentOffset=_packetLen-delta-1;
		_packetOffset=relative+delta-_packetLen;		
		return 1;
		
}
void ADM_mpegDemuxerTransportStream::dump( void ) 
{	

  uint64_t pos;
  	parser->getpos(&pos);                                                      
     aprintf("\n off : %llu ", _currentOffset);                                                  
     aprintf("\n len : %llu ", _packetLen);        
     aprintf("\n total off : %llu ", _packetOffset);    
     aprintf("\n abs pos : %llu ", pos);    

     aprintf("\nerr : %d ", _lastErr);                                                  
}

// EOF

