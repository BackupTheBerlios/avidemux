/***************************************************************************
                          ADM_mpegpacket.cpp  -  description
                             -------------------
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
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"

#include "ADM_mpegindexer/ADM_mpegparser.h"

#include "ADM_toolkit/toolkit.hxx"

//
#define VOB          


ADM_mpegDemuxer::ADM_mpegDemuxer(void )
{
		_lastErr=0;
		_firstPTS=MINUS_ONE;
		_otherPTS=MINUS_ONE;	
	
}
ADM_mpegDemuxer::~ADM_mpegDemuxer( )
{
}


uint8_t		ADM_mpegDemuxerElementaryStream::read8i(void)
{
	uint8_t r;
		fread(&r,1,1,_vob)  ;
		_pos+=1;
		if(_pos>=_size) _lastErr=1;	
		return r;					
};
uint16_t	ADM_mpegDemuxerElementaryStream::read16i(void)
{
	uint16_t v;
	uint8_t r[2];
			fread(r,1,2,_vob)  ;
			_pos+=2;
			if(_pos>=_size) _lastErr=1;	
			return v=(r[0]<<8)+r[1];					
}

uint32_t	ADM_mpegDemuxerElementaryStream::read32i(void)
{
	static uint8_t c[4];
	uint32_t v;

		fread(c,1,4,_vob)  ;
		_pos+=4;
		if(_pos>=_size) _lastErr=1;	
			
		v= (c[0]<<24)+(c[1]<<16)+(c[2]<<8)+c[3];
		return v;					
}

// a demuxer is also a king of hi-level parser
uint8_t		ADM_mpegDemuxer::sync( uint8_t *stream)
{
uint32_t val,hnt;
//uint64_t p,tail;

				val=0;
				hnt=0;			
			
			// preload
				hnt=(read8i()<<16) + (read8i()<<8) +read8i();
				if(_lastErr)
						{
							_lastErr=0;
							printf("\n io error , aborting sync\n");
							return 0;	
						}		
				while((hnt!=0x00001))
				{
					
					hnt<<=8;
					val=read8i();					
					hnt+=val;
					hnt&=0xffffff;	
					
					if(_lastErr)
					{
						_lastErr=0;
						// check if we are near the end, if so we abort	
						// else we continue
						uint64_t pos;
							pos=getAbsPos();
							//	if(_size-pos<1024)
							{
								printf("\n io error , aborting sync\n");
								return 0;	
							}
							printf("\n  Bumped into something at %llu / %llu but going on\n",pos,_size);
							if(!_size) assert(0);
							// else we go on...
							hnt=(read8i()<<16) + (read8i()<<8) +read8i();
					}
									
				}
				
	      *stream=read8i();
	      return 1;
}

//---------------------------------------------------------------------------------------------
//		Elementary (video) stream demuxer
//------------------
//---------------------------------------------------------------------------
ADM_mpegDemuxerElementaryStream::ADM_mpegDemuxerElementaryStream( void )
{
_vob=NULL;
_pos=0;
}

ADM_mpegDemuxerElementaryStream::~ADM_mpegDemuxerElementaryStream(  )
{
		if(_vob)
			{
				fclose(_vob);
				_vob=NULL;				
	   }
}

uint8_t ADM_mpegDemuxerElementaryStream::open(char *name)
{
		_vob=fopen(name,"rb");
		if(!_vob) return 0;
		// grab size
		fseeko( _vob,0,SEEK_END);
		_size=ftello(_vob);  // it is safe with nuv file , 1 Gb max per file
		fseeko(_vob,0,SEEK_SET);
		_pos=0;
		return 1;
	
}
uint8_t ADM_mpegDemuxerElementaryStream::goTo(uint64_t offset)
{
		fseeko(_vob,offset,SEEK_SET);
		_pos=offset;
		return 1;	
}
uint8_t ADM_mpegDemuxerElementaryStream::forward(uint32_t f)
{
		fseeko(_vob,f,SEEK_CUR);
		_pos+=f;
		return 1;
		
}
uint64_t		ADM_mpegDemuxerElementaryStream::getAbsPos( void)
{
    	uint64_t i;
     i=ftello(_vob);
     return i;
		
}
uint64_t		ADM_mpegDemuxer::getSize( void)
{
	    return _size;
}

uint8_t ADM_mpegDemuxerElementaryStream::getpos(uint64_t *p)
{

	*p=ftello(_vob);
	return 1;	
}

// 
// in case of elementary stream absolute=relative
uint8_t			ADM_mpegDemuxerElementaryStream::_asyncJump(uint64_t relative,uint64_t absolute)
{
        UNUSED_ARG(absolute);
		goTo(	relative);
		return 1;
	
}
 int32_t ADM_mpegDemuxer::getPTSDelta( void ) 
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
