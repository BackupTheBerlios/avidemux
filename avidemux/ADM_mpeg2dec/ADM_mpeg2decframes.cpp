/***************************************************************************
                          ADM_mpeg2decframes.cpp  -  description
                             -------------------
    begin                : Sat Nov 2 2002
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include <ADM_assert.h>

#include "ADM_mpeg2dec/ADM_mpegscan.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"
#include "ADM_mpeg2dec/ADM_mpegAudio.h"

#include "ADM_toolkit/toolkit.hxx"

#define W _mainaviheader.dwWidth
#define H _mainaviheader.dwHeight

#define TRANSLATE

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG_DEMUX
#include "ADM_toolkit/ADM_debug.h"
 //-----------------------------------
//-----------------------------------

uint8_t  mpeg2decHeader::setFlag(uint32_t frame,uint32_t flags)
{
        UNUSED_ARG(frame);
    	UNUSED_ARG(flags);
		return 0;
};
//-----------------------------------
//
//	Return the I frame starting the GOP in which in is
// in is display, return is index
//
//-----------------------------------
uint8_t  	mpeg2decHeader::getRaw(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
// rewind to the last know frame I

//	printf("\n seeking\n");
	if(framenum)
	{
		uint32_t i=framenum;
		
			while(i && _indexMpegPTS[i].type!='I') i--;
			asyncJump(i);
	}
//	printf("\n going\n");
	demuxer->goTo(_indexMpegPTS[framenum].offset);
//	printf("\n reading\n");
	demuxer->read(ptr,_indexMpegPTS[framenum].size);
	*framelen= _indexMpegPTS[framenum].size;
	return 1;
	
}
uint8_t	mpeg2decHeader::asyncJump(uint32_t frame)
{
		ADM_assert(_indexMpegPTS[frame].type=='I');
		 if(!demuxer->_asyncJump(_indexMpegPTS[frame].offset,
                                                                _indexMpegPTS[frame].absoffset))
			{
					printf("\n Asynchronous jump failed ! (frame : %ld)\n",frame);
			
			}
		return 1;
}

uint8_t   mpeg2decHeader::getRawStart(uint8_t *ptr,uint32_t *len)
{
		
		 *len=_startSeqLen;
		 memcpy(ptr,_startSeq,_startSeqLen);
		 return 1;
}				

uint32_t mpeg2decHeader::getFlags(uint32_t frame,uint32_t *flags)
{
	//uint32_t j=0;
	*flags=0;
	if(frame>=(uint32_t)_syncPoints)
		{
				printf(" out of bound (%lu/%lu/%lu)!\n",frame,_videostream.dwLength,_syncPoints);
				return 0;
		}
	if(_fieldEncoded) frame=_fields[frame].indexA;
	switch(_indexMpegPTS[frame].type)
	{
			case 'I': *flags=AVI_KEY_FRAME; break;
			case 'P': *flags=0; break;
			case 'B': *flags=AVI_B_FRAME; break;
			default:
					ADM_assert(0);
					return 0;
	}
	return 1;

};
//-----------------------------------
//-----------------------------------

uint8_t  	mpeg2decHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
//-----------------------------------
//-----------------------------------

{
	return   getFrameNoAlloc( framenum,ptr,framelen,NULL);
}

//-----------------------------------
//
//
//-----------------------------------
uint8_t  mpeg2decHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags)
{
    if(framenum>= _nbFrames)
			{
					printf("\n Mpeg out of bound ! (%ld/%ld)\n",framenum,_nbFrames);
					return 0;
			}
					// here we go, first check
        if(flags)
        {
                *flags=0;
  		        getFlags(framenum,flags);
         } 		
            if(!_fieldEncoded) return getFrameMpeg(framenum,ptr,framelen,flags);
      
    uint32_t len,dummyflag;
            if(!getFrameMpeg(_fields[framenum].indexA,ptr,&len,flags)) return 0;
            
            if(!getFrameMpeg(_fields[framenum].indexB,ptr+len,framelen,&dummyflag)) return 0;
            
            *framelen+=len;
            return 1;
}
uint8_t  mpeg2decHeader::getFrameMpeg(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags)
{
        ADM_assert(framenum<_syncPoints);
		aprintf("\t\tMpeg: get frame req for %lu\n",framenum);
		*framelen=0;

		if(_indexMpegPTS[framenum].type=='I')
				asyncJump(framenum);
		else
				demuxer->goTo(_indexMpegPTS[framenum].offset);
		if(!framenum)
		{
			*framelen=_startSeqLen;
			memcpy(ptr,_startSeq,_startSeqLen);
			ptr+=_startSeqLen;
		}
		*framelen+=_indexMpegPTS[framenum].size;
		demuxer->read(_indexMpegPTS[framenum].size,ptr);
		return 1;

}
//EOF
