/***************************************************************************
                         Brute force ivtc

	We look at how interlaced the frames are
	On FILM -> NTSC the pattern should be like

	1  2 3 4 -> 11 22 23 34 44

	23 and 34 being interlaced. We recombine them (y & uv)

	If we cannot decide we drop a frame

	The dubious case is when we got 1st and last frame ilaced

    begin                : Thu Mar 21 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/
#if 0
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFieldUtil.h"
#include "ADM_video/ADM_vidIVTC.h"
#include "ADM_video/ADM_interlaced.h"
#include "ADM_toolkit/TLK_clock.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"

BUILD_CREATE(ivtc_create,ADMVideoIVTC);

// 0 -> Merge
// 1 -> Chroma forward
// 2-> Chroma backward
#define MERGE_FIELDS 0
char *ADMVideoIVTC::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," IVTC");
        return buf;
}
//_______________________________________________________________
ADMVideoIVTC::ADMVideoIVTC(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_info.fps1000=(_info.fps1000*4)/5;
	_info.nb_frames=(_info.nb_frames*4)/5;
	for(uint32_t i=0;i<5;i++)
	{
		_uncompressed[i]=new uint8_t[3*_info.width*_info.height];
	}
	_cacheStart=0xffffff;
	_old1=0;
	_old2=0;
	_confidence=0;
	_shifted=0;
}
// ___ destructor_____________
ADMVideoIVTC::~ADMVideoIVTC()
{
	for(uint32_t i=0;i<5;i++)
	{
 		delete [] _uncompressed[i];
	}
}


uint8_t ADMVideoIVTC::getFrameNumberNoAlloc(uint32_t frame,
										uint32_t *len	,
										uint8_t *data,
										uint32_t *flags)
{
//static Image in,out;
			if(frame>=_info.nb_frames)
			{
				printf("out of bound frame (%lu / %lu)\n",frame,_info.nb_frames);
				return 0;
			}

		uint32_t w=_info.width;
		uint32_t h=_info.height;
		uint32_t page=w*h;
		uint32_t i;

		uint32_t target;
		uint32_t loop=0;

		*len=(page*3)>>1;

cont:

		target=frame-((frame)%4);
		// got it ?
		if(_cacheStart==target)
		{
			uint32_t index;

			aprintf("Filter: It is in cache...(cachestart=%lu)\n",_cacheStart);
			index=frame%4;
			aprintf(" index : %lu skipped : %lu\n",index,_skipped);
			if(index>=_skipped) index++;
			aprintf(" index : %lu skipped : %lu\n",index,_skipped);
			memcpy(data,_uncompressed[index],*len);
			*flags=0;
			return 1;
		}
		else
		{
			aprintf("Not in cache...\n");
		}
		ADM_assert(!loop);
		loop++;
		// Else ask the 5 corresponding frame
		_cacheStart=target;
		target=(target*5)>>2;

		uint32_t dflags,dlen;

		for( i=0;i<5;i++)
			{
				
	 			if(!_in->getFrameNumberNoAlloc(target+i, &dlen,_uncompressed[i],&dflags))
				 {
				 	 	printf("Cannot get frame %lu\n",target+i);
					 	return 0;
				 }

			}
		// Now we look up, there must be 2 i-laced and 3 progressive
		// First try looking the 2 last i-laced we had to see if they are still there

		// if not, we do each one of them and keep the lowest interlaced
		uint32_t laced[5];
	// look up the one that as max interlacing
		int max1=-1;
		int max2=-1;
		int max=0;
		Clock clock;
		uint32_t k;

		k=clock.getElapsedMS();
		aprintf("\n confidence : %lu old1 :%lu old2: %lu\n",_confidence,_old1,_old2);
		if(_confidence>25) _confidence=2; // we reavluate from time to time
		if(_confidence>5) // just check oldmax1
		{
			// if old1 is interlaced, we assume it is ok
			if(ADMVideo_interlaceCount(_uncompressed[_old1],_info.width,_info.height)>10)
			{
				aprintf("\n Seems good %d is interlaced !\n",_old1);
				max1=_old1;
				max2=_old2;
				goto fnd;
			}
			else
			{
				aprintf(" Does not seems interlaced enough\n");
			}
		}
		// we are not very confident, check it
		for( i=0;i<5;i++)
			{

				laced[i]=ADMVideo_interlaceCount(_uncompressed[i],_info.width,_info.height);
				aprintf(" Frame %d has %lu icing factor\n",target+i,laced[i]);
			}
		printf("  bench : %lu\n",clock.getElapsedMS() -k);


		for( i=0;i<5;i++)
			{
				if(max<laced[i])
					{
						max=laced[i];
						max1=i;
					}
			}


		max=0;
		for( i=0;i<5;i++)
			{
				if(max<laced[i] && i!=max1)
					{
						max=laced[i];
						max2=i;
					}
			}

		if(max1>max2)
		{
			int tmp;
			tmp=max1;
			max1=max2;
			max2=tmp;
		}
		aprintf("Detect max1: %d max2: %d\n",max1,max2);
		if(max1==0 && max2==4)
		{
			printf("\n warning : dubious case \n");
		}
		if((max1==_old1) && (max2==_old2))
		{
			_confidence++;
		}
		else
		if((max1==_old1) || (max2==_old2)||(max1==_old2)||(max2==_old1) && _confidence>5)
		{	// One of them match we keep using it
			max2=_old2;
			max1=_old1;
			aprintf("we use hinted...\n");
		}
		else
		{ // neither match, we reset everything
			_confidence=0;
			if(((max1+1!=max2)&&(max2+1!=max1))||max1==-1||max2==-1)
			{
				aprintf("max1: %d max2: %d\n",max1,max2);
				aprintf("out of order fields\n");
				goto cont;
			}
			if(max1>max2)
				{
					max1=max2;
					max2++;
				}
			_old1=max1;
			_old2=max2;

			_confidence=1;

		}
fnd:
		aprintf(" Max1 is %d max2 is %d\n",max1,max2);

		// take upper field from max1 and lower field from max2

		uint8_t *in,*out;
		in=_uncompressed[max2];
		out=_uncompressed[max1];

		// merge them
		for(uint32_t y=0;y<_info.height>>1;y++)
		{
			memcpy(out,in,_info.width);
			in+=_info.width<<1;
			out+=_info.width<<1;
		}

		// ditto for chroma


		in=_uncompressed[max2]+page;
		out=_uncompressed[max1]+page;

		for(uint32_t y=0;y<_info.height>>2;y++)
		{
			memcpy(out,in,_info.width>>1);
			in+=_info.width;
			out+=_info.width;
		}
		// and copy others
		_skipped=max2;
		goto cont;
		return 1;
}



#endif
