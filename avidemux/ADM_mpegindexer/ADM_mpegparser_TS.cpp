/***************************************************************************
                          ADM_mpegparser.cpp  -  description
                             -------------------
    begin                : Tue Oct 15 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This is 1st step Transport stream demuxer
    It works only when the transported data is a PES stream
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef __FreeBSD__
          #include <sys/types.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
#include "ADM_mpegindexer/ADM_mpegparser.h"
	      	

mParserTS::mParserTS( void )
{
		parser=new mParser;
}

mParserTS::~mParserTS()
{
	if(parser)
	{
		delete parser;
		parser=NULL;
	}
	
}
uint8_t mParserTS::forward(uint64_t u)
{

	if(u<_tsSize) {_tsPos+=u;_tsSize-=u;return 1;}
	while(u>TS_PACKET) {fillTsBuffer();u-=_tsSize;}
	return forward(u);

}
uint8_t mParserTS::open( char *name)
{
uint8_t check=1;
uint8_t count=1;


	   if(!parser->open(name)) return 0;	 
	   return 1;	
}


/*--------------------------------------------------
		Read l bytes from file
----------------------------------------------------*/
uint8_t mParserTS::read32(uint32_t l, uint8_t *buffer)
{
uint32_t r;
	if(_tsPos+l<_tsSize)
	{
		memcpy(buffer,bufferTS+_tsPos,l);
		_tsSize-=l;
		_tsPos+=l;
		return 1;	
	}
	// Flush
	memcpy(buffer,bufferTS+_tsPos,_tsSize);
	buffer+=_tsSize;
	l-=	_tsSize;
	_tsSize=0;
	_tsPos=0;
	// Fill a new one
	if(!fillTsBuffer())
	{
		printf("FillTs failed\n");
		return 0;
	}	
	return read32(l,buffer);
	
}	
// Fill a packet
uint8_t mParserTS::fillTsBuffer( void )
{
uint32_t payload;
uint32_t adapt;
uint32_t pid;

	assert(parser);
	while(1)
	{
	if(! parser->read32(TS_PACKET,bufferTS)) return 0;
	if(bufferTS[0]!=0x47)
	{
		printf("This is not a TS packet !\n");
		return 0;
	}
	_tsSize=TS_PACKET-4;
	_tsPos=4;
	
	pid=bufferTS[1]&0x1F;
	payload=bufferTS[3]&0x10;
	adapt=bufferTS[3]&0x20;
	
	printf("pid : %x payload size : %d \n",pid,_tsSize);
	
	if(pid!=41) continue;
	if(!payload) continue;
	
	if(adapt)
	{
		uint32_t l=bufferTS[4];
		_tsPos+=l+1;
		_tsSize-=l+1;
	}
	printf("pid : %x payload size : %d \n",pid,_tsSize);
	
	return 1;
	};
}
uint8_t mParserTS::sync(uint8_t *stream)
{
uint32_t val,hnt;

				val=0;
				hnt=0;			
			// preload
				hnt=(read8i()<<16) + (read8i()<<8) +read8i();								;				
				while((hnt!=0x00001))
				{
						
						hnt<<=8;
						val=read8i();					
						hnt+=val;
						hnt&=0xffffff;
						// TODO CHECK FOR OVERFLOW
				}
				
	      *stream=read8i();
	      return 1;
	
}