/***************************************************************************
                          ADM_mpegpacket_PS.cpp  -  description
                             -------------------
    begin                : Sat Nov 2 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This is basically a mpeg PES packet demuxer that extracts the payload
    from the packets and present it like it was a ES to the upper part.

	The KEY BUFFERED indicates if we buffer matching packet or not
	Faster but trickier    
            
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

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_toolkit/ADM_debug.h"

#define MINUS_ONE 0xffffffff
//
#define BUFFERED
//
#define USER_DATA_START_CODE 	0xb2
#define PACK_START_CODE					0xba
#define SYSTEM_START_CODE       	0xbb
#define PRIVATE_STREAM_1 				0xbd
#define PRIVATE_STREAM_2 				0xbf
#define SYSTEM_END_CODE					0xb9
static uint64_t _lastSync;

extern void mixDump(uint8_t *ptr,uint32_t len);

//#define PRINT_PTS
//_______________________________________________________
//_______________________________________________________
//_______________________________________________________
//		Program stream demuxer
//_______________________________________________________
//_______________________________________________________
//_______________________________________________________

 ADM_mpegDemuxerProgramStream::ADM_mpegDemuxerProgramStream(uint8_t stream,uint8_t stream2) 
 {
		printf("\n Program stream demuxer initialized with stream 1 = %x",stream);
		printf("stream 2 = %x\n",stream2);
	 
		parser=NULL;	 
		 _otherStream=stream2;
		 _firstPTS=_otherPTS=MINUS_ONE;
		_otherLen=0;
		_muxTypeMpeg2=0;
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
int32_t 	ADM_mpegDemuxerProgramStream::getPTSDelta( void ) 
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
 ADM_mpegDemuxerProgramStream::~ADM_mpegDemuxerProgramStream() 
 {
		if(parser) delete parser;			
		parser=NULL;
			
}
	  
	  
uint8_t ADM_mpegDemuxerProgramStream::open(char *name)
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
				assert(0);
				
			_firstPacketOffset=_lastSync-4;	
	
		printf("\n Demuxer : %llx first,  %x\n",    _firstPacketOffset,_streamId);
		parser->setpos(	_firstPacketOffset);	
		_packetOffset=0;
		_currentOffset=0;
		_packetLen=0;
		return 1;	
}

//_________________________________________________________________________
uint32_t ADM_mpegDemuxerProgramStream::	read(uint8_t *w,uint32_t len)
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

uint8_t ADM_mpegDemuxerProgramStream::getpos(uint64_t *p)
{
			*p=  _packetOffset+ _currentOffset;
			return 1;
		
}
//_________________________________________________________________________

uint8_t ADM_mpegDemuxerProgramStream::	goTo(uint64_t offset)
{
//uint32_t leftover;
//uint64_t pos;
uint8_t r;

		if(offset==0)
			{
//			printf("\n ..start..\n");
				parser->setpos(_firstPacketOffset);
				_packetOffset=0;
				_packetLen=0;				
				_currentOffset=0;						
				r= _nextPacket();
				
//		 	printf(" Cur: %llx Size : %llx(1)\n",_currentOffset,_packetLen);
              return r;
			}

		if(offset<_packetOffset)
			{
	//			printf("\n rewind...\n");
					// go back to the beginning and forward till we get there
				// to be optimised later
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
uint8_t ADM_mpegDemuxerProgramStream::forward(uint32_t f)
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
uint8_t  ADM_mpegDemuxerProgramStream::	_nextPacket(void)
{
	uint8_t stream,subid;
//	uint32_t sz;
	uint16_t s;
//	uint8_t ostream,istream;
	uint32_t len;
	uint32_t pts=0;
	
	//printf("*");
		if(_lastErr) return 0;

	 //        printf("\n next Packet !\n");
	   _packetOffset+=_packetLen;
		// search the first video packet....
		while(1)
		{
	//		printf("-");
			if(!parser->sync(&stream))
			{
					printf("\n could not get sync!\n");
					return 0;
			}
			// used to mark first packet && async jump
			parser->getpos(&   _lastSync);
			// check we are not at the end of the stream
			if(_lastSync==_size) 
				{
					printf("End of file reached\n");
					return 0;				
				}

			if((stream==_streamId) || (stream==_otherStream))
			{
				// good stream id .. what about sub id ?
				len=  _skipPacketHeader(stream,&subid,&pts);
				if(!len)
				{
					printf("+");
					continue;
				}
				if((subid==_streamSubId)&&(stream==_streamId))
				{
					_packetLen=len;

					assert(_packetLen<PES_BUFFER_SIZE);
			  		parser->read32(_packetLen,_buffer);
			      		_currentOffset=0;
			      		aprintf("Found packet id :%x subid :%x\n",stream,subid);
					/*mixDump(_buffer,_packetLen);*/
					if(pts!=MINUS_ONE)
					{
						if(_firstPTS==MINUS_ONE)
						{
							_firstPTS=pts;
							printf(">>>First PTS = %lu\n",_firstPTS);
						}
					
					}
				      return 1;
				}
				if((subid==_otherStreamSubId)&&(stream==_otherStream))
				{
					 _otherLen+=len;
					if(pts!=MINUS_ONE)
					{
						if(_otherPTS==MINUS_ONE)
						{
							_otherPTS=pts;
							printf(">>>Other PTS = %lu\n",_otherPTS);
						}
					
					}
				}
				parser->forward(len);
			}
			else
			{
					switch(stream)
					{

							case PACK_START_CODE :
												parser->forward(8);
												break; // pack start
							case SYSTEM_START_CODE : // system header
							case PRIVATE_STREAM_2:
							case PRIVATE_STREAM_1:
							case 0xbe:
												s=parser->read16i();
												parser->forward(s);
												break;
							/*case SYSTEM_END_CODE:
													printf("\n End stream reached\n");
													return 0;*/
								default:	// ignore unknown
									//			printf(" Stream not handled : %02x\n",stream);
												//s=parser->read16i();
												//parser->forward(s);
												//aprintf("\n Unknow stream %x stream \n",stream);
												//return 0;
// this prevents the deadlock at end of file
// ????
#if 1
												uint64_t o=0;
												parser->getpos(&o);
												aprintf("Pos : %llu \n",o);
#endif
												break;
					}


			}

		};

}



uint64_t		ADM_mpegDemuxerProgramStream::getAbsPos( void)
{
    	uint64_t i;
     parser->getpos(&i);
#ifdef BUFFERED
     i=i-_packetLen+_currentOffset;
#endif
     return i;

}
//
//		Here it become tricky
//		We search the last video packet that contains the sync point
// 		and resync compared to it
//		we search 16k backward and take only that one
//
uint8_t			ADM_mpegDemuxerProgramStream::_asyncJump(uint64_t relative,uint64_t absolute)
{
		uint64_t backward,ref;
		uint64_t pos;
		//uint8_t stream,subid;
		uint64_t backward_jump;
		uint64_t backward_jump_backup;
#define BACKWARD 4096		
		uint8_t  backBuffer[BACKWARD];
		
//		printf("\n async : %llx %llx\n",relative,absolute);;
		if(absolute<BACKWARD) backward_jump=absolute;
		else backward_jump=BACKWARD;
		backward_jump_backup=backward_jump;
		backward=absolute-backward_jump;		
		// We rewind 
		parser->setpos(backward);
		// and read
		parser->read32(backward_jump,backBuffer);
		// now parse back until we reach the packet start tag
		ref=absolute-backward_jump;
		backward_jump-=4;;
		
		while(backward_jump)
		{
			// search 00 00 01 XX
			if(  (backBuffer[backward_jump+0]==0) && (backBuffer[backward_jump+1]==0) &&
			     (backBuffer[backward_jump+2]==1)  && (backBuffer[backward_jump+3]==_streamId) )
			{
				// gotcha
				backward=ref+backward_jump-2;						
				
				parser->setpos(backward);
				if(!_nextPacket())
				{
					printf("\n async jump error (next Packet failed) !!!\n");
					return 0;	
				}
				parser->getpos(&pos);
				// we are just after this packet (normally)
			//	if((absolute>=pos-_packetLen) && (absolute<pos) )	
				{	   
		      			// gotcha					
					//	printf("\n Found rel: %llx abs: %llx --> at : %llx size %lu",relative,absolute,pos,_packetLen);
					_currentOffset=absolute-(pos -_packetLen);
				  	 _packetOffset=relative-_currentOffset;	
					return 1;						
				}
				
				
				printf("\n strange things going on \n");
				printf(" abs %lld \n",absolute);
				printf(" ref %lld \n",ref);
				printf(" pos %lld \n",pos);
				printf(" pack %llu \n",_packetLen);
				printf(" back %llu \n",backward);
				
				printf(" back jump %llu \n",backward_jump);
				printf(" oriback %llu \n",backward_jump_backup);
				mixDump(backBuffer,BACKWARD);
				printf("\n ***************************************\n");
			}
		backward_jump--;
		}
		printf("\n scanned some bytes without finding it \n Mpeg pes demuxer error \n");
		return 0;
}
uint8_t			ADM_mpegDemuxerProgramStream::_asyncJump2(uint64_t relative,uint64_t absolute)
{
	//printf("\n async2: %llu %llu\n",relative,absolute);
	parser->setpos(absolute);
	_packetOffset=relative;
	_packetLen=0;
	_currentOffset=0;
	//printf("\n new relative : %lu",_packetOffset);
	return _nextPacket();
}
void ADM_mpegDemuxerProgramStream::dump( void ) 
{	

  uint64_t pos;
  	parser->getpos(&pos);                                                      
     aprintf("\n off : %llu ", _currentOffset);                                                  
     aprintf("\n len : %llu ", _packetLen);        
     aprintf("\n total off : %llu ", _packetOffset);    
     aprintf("\n abs pos : %llu ", pos);    

     aprintf("\nerr : %d ", _lastErr);                                                  
}
/*  ______________________________________________________
   	Skip the packet header and returns the size of payload data
    ______________________________________________________
*/
uint32_t  ADM_mpegDemuxerProgramStream::_skipPacketHeader( uint8_t sid,uint8_t *subid,uint32_t *pts )
{
//uint32_t un ,deux;
uint64_t size=0;
uint8_t c;
uint8_t align=0;
			
		*subid=0xff;
		*pts=MINUS_ONE;
		
			uint64_t pos;
			parser->getpos(&pos);
//			printf("\n ___syncpos : %lx",pos);
						
					
		  size=parser->read16i();
			if((sid==0xbe) || (sid==PRIVATE_STREAM_2)
			||(size==SYSTEM_START_CODE)
			) // special case, no header
			{
											return(size);				
			}
				
			// 	remove padding if any						
        
  			while((c=parser->read8i()) == 0xff) 
								{
									size--;
				//					printf(" padding\n");
								}
			//----------------------------------------------------------------------------------------------					
			//-------------------------------MPEG-2 PES packet style----------------------					
			//----------------------------------------------------------------------------------------------													
			if(((c&0xC0)==0x80))								//mpeg 2
			{
				uint32_t ptsdts,len;
			//	printf("\n mpeg2 type \n");
				_muxTypeMpeg2=1;
					// c			= copyright and stuff	
				//	printf(" %x align\n",c);	
					if(c & 4) align=1;						
					c=parser->read8i();		// PTS/DTS		
				//	printf("%x ptsdts\n",c
					ptsdts=c>>6;
					// header len
		   			len=parser->read8i();
   			      		size-=3;  

			      switch(ptsdts)
			      {
							case 2: // PTS=1 DTS=0
								if(len>=5)
								{
								uint32_t pts1,pts2,pts0;
								//	printf("\n PTS10\n");
									pts0=parser->read8i();	
									pts1=parser->read16i();	
			     						pts2=parser->read16i();			
			            					len-=5;
			                 	  			size-=5;
									*pts=(pts1>>1)<<15;
									*pts+=pts2>>1;
									*pts+=(((pts0&6)>>1)<<30);
#ifdef PRINT_PTS									
									printf("PTS: %lx %lx\n",*pts,sid);
#endif									
								}
								break;
							case 3: // PTS=1 DTS=1
								#define PTS11_ADV 10 // nut monkey
								 if(len>=PTS11_ADV)
								 {
								 uint32_t skip=PTS11_ADV;
								 uint32_t pts1,pts2,dts,pts0;
								//	printf("\n PTS10\n");
									pts0=parser->read8i();	
									pts1=parser->read16i();	
			     						pts2=parser->read16i();	
											
			            					*pts=(pts1>>1)<<15;
									*pts+=pts2>>1;
									*pts+=(((pts0&6)>>1)<<30);
#ifdef PRINT_PTS																		
									printf("PTS: %lx %x\n",*pts,sid);
#endif
									pts0=parser->read8i();	
									pts1=parser->read16i();	
			     						pts2=parser->read16i();			
			            					dts=(pts1>>1)<<15;
									dts+=pts2>>1;
									dts+=(((pts0&6)>>1)<<30);
			            					len-=skip;
			                 				size-=skip;
									//printf("DTS: %lx\n",dts);
									
								}
								break;
							case 1:
								assert(0); // forbidden !
								break;
							case 0: 
								// printf("\n PTS00\n");
								break; // no pts nor dts
										
							    
					}  
// Extension bit	
// >stealthdave<				
					if(c & 0xf) // extension flag ? (PSTD/CRC/...)
						{
#ifdef TELL_ME_ALL						
								printf("\n Other info : %x %x",c&0xf,sid);
#endif								
								if(c&1)  // extension flag set ?
								{
									c=parser->read8i();
#ifdef TELL_ME_ALL										
									printf("\n  extension : %x",c);
#endif									
									size--;
									if(c& 0x10) //  P-STD buffer, others ignored for now
									{
										
										uint16_t psdt;
										psdt=parser->read16i() & 0x1FFF;																				
#ifdef TELL_ME_ALL							
										printf("\n  pstd : %d (%x )",psdt,psdt);
#endif												
										size-=2;
										len-=2+1;	
#ifdef TELL_ME_ALL											
										printf("\n size: %d",size);																													
#endif												
									}
								}				
						}		
					    
					if(len) 
						{
							parser->forward(len);
							size=size-len;	
							
						}
       		if(sid==PRIVATE_STREAM_1)
				{
					if(size>5)
					{
                      // read sub id
                      *subid=parser->read8i();
  //                    printf("\n Subid : %x",*subid);
  		       switch(*subid)
		       {
		       		case 0x80:case 0x81:case 0x82:case 0x83:
				case 0x84:case 0x85:case 0x86:case 0x87:
							*subid=*subid-0x80;
							break;
							// PCM
				case 0xA0:case 0xA1:case 0xa2:case 0xa3:
				case 0xA4:case 0xA5:case 0xa6:case 0xa7:
							// we have an additionnal header
							// of 3 bytes
							parser->forward(3);
							size-=3;
							break;
				default:
						*subid=0xff;
			}
                      // skip audio header
                      parser->forward(3);
                      size-=4;
      				}
				}
	       //    printf(" pid %x size : %x len %x\n",sid,size,len);

					return size;
			}
//----------------------------------------------------------------------------------------------		
//-------------------------------MPEG-1 PES packet style----------------------					
//----------------------------------------------------------------------------------------------					
           if(_muxTypeMpeg2)
           	{
					printf("*** packet type 1 inside type 2 ?????*****\n");
					return 0; // mmmm			
				}
			// now look at  STD buffer size if present
			// 01xxxxxxxxx
			if ((c>>6) == 1) 
			{	// 01
						size-=2;
						parser->read8i();			// skip one byte
						c=parser->read8i();   // then another
			}			
			// PTS/DTS
			switch(c>>4)
				{
				case 2:
					{
						// 0010 xxxx PTS only
						uint32_t pts1,pts2,pts0;
								size -= 4;
								pts0=(c>>1) &7;
								pts1=parser->read16i()>>1;
								pts2=parser->read16i()>>1;
								*pts=pts2+(pts1<<15)+(pts0<<30);
								break;
					}
				case 3:
					{		// 0011 xxxx
						uint32_t pts1,pts2,pts0;
								size -= 9;
									
								pts0=(c>>1) &7;
								pts1=parser->read16i()>>1;
								pts2=parser->read16i()>>1;
								*pts=pts2+(pts1<<15)+(pts0<<30);
								parser->forward(5);
					}								
								break;
				case 1:
							// 0001 xxx		
									{     // PTSDTS=01 not allowed											
										assert(0);
									}
									break;
												
				}
								

		if(!align)	
	      		size--;		
			// remove padding

//		printf(" Packet size: %d",size);     
    	return size;
}
// EOF

