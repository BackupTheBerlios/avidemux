/***************************************************************************
                          avidemutils.cpp  -  description
                             -------------------
    begin                : Sun Nov 11 2001
    copyright            : (C) 2001 by mean
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
#include <stdlib.h>

#include <string.h>
//#include <sstream>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "riffparser.h"
#include "subchunk.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/bitmap.h"
void memcpyswap(uint8_t *dest, uint8_t *src, uint32_t size);
//_________________________________________________
//      Convert a frame number into equivalent in ms
//_________________________________________________



// misc dump functions
extern "C" 
{
	void mixDump_c(uint8_t * ptr, uint32_t len)
	{
		mixDump(ptr,len);
	}
}
void mixDump(uint8_t * ptr, uint32_t len)
{
    char string[200];
    char string2[200];
    char tiny[10];

    char *str, *str2;
    uint32_t i;

    str = (char *) string;
    str2 = (char *) string2;
    *str = *str2 = 0;

    for (i = 0; i < len; i++)
      {
	  			if (*ptr < 32)
			    {
						strcat(str, ".");
				  } else
			    {
						sprintf(tiny, "%c", *ptr);
						strcat(str, tiny);
			    }

	  	sprintf(tiny, " %02x", *ptr);
		  strcat(str2, tiny);
		  ptr++;

		  if ((i % 16) == 15)
		    {
					printf("\n %04lx : %s %s", (i >> 4) << 4, str, str2);
					*str = 0;
					*str2 = 0;
		    }
      }
	// left over
	if(len%16!=0)
	{
		 printf("\n %04lx : %s %s", (len >> 4) << 4, str, str2);
	}      
}
/*
	A bunch of Endianness swapper to ease handling
	avi on BE processor (sparc/Mac)

*/
void Endian_AviStreamHeader(AVIStreamHeader *s)
{

#ifdef ADM_BIG_ENDIAN
	#define SWAP32(x)  s->x=R32(s->x);
		SWAP32(fccType);
		SWAP32(fccHandler);
		SWAP32(dwFlags);
		SWAP32(dwScale);
		SWAP32(dwRate);
		SWAP32(dwStart);
		SWAP32(dwLength);
		SWAP32(dwSampleSize);
	#undef SWAP32

#endif

}
void Endian_AviMainHeader(MainAVIHeader *m)
{
#ifdef ADM_BIG_ENDIAN
	#define SWAP32(x) m->x=R32(m->x)

		SWAP32(dwMicroSecPerFrame);
		SWAP32(dwMaxBytesPerSec);
		SWAP32(dwPaddingGranularity);

		SWAP32(dwFlags);
		SWAP32(dwTotalFrames);
		SWAP32(dwInitialFrames);
		SWAP32(dwStreams);
		SWAP32(dwSuggestedBufferSize);

		SWAP32(dwWidth);
		SWAP32(dwHeight);
	
	#undef SWAP32

#endif

}

void Endian_BitMapInfo( BITMAPINFOHEADER *b)
{
#ifdef ADM_BIG_ENDIAN
	#define SWAP32(x) b->x=R32(b->x)
	#define SWAP16(x) b->x=R16(b->x)

		SWAP32(biSize);
		SWAP32(biWidth);
		SWAP32(biHeight);
		SWAP16(biPlanes);
		SWAP16(biBitCount);
		SWAP32(biCompression);
		SWAP32(biSizeImage);
		SWAP32(biXPelsPerMeter);
		SWAP32(biYPelsPerMeter);
		SWAP32(biClrUsed);
	#undef SWAP32(x)
	#undef SWAP16
#endif
}


void Endian_WavHeader(WAVHeader *w)
{
#ifdef ADM_BIG_ENDIAN
	#define SWAP32(x) w->x=R32(w->x)
	#define SWAP16(x) w->x=R16(w->x)
		SWAP16(encoding);	
		SWAP16(channels);	
		SWAP32(frequency);	
		SWAP32(byterate);	
		SWAP16(blockalign);	
		SWAP16(bitspersample);	

	#undef SWAP32
	#undef SWAP16

#endif


}
// Here we copy in reverse order
// Useful to do LE/BE conv on nomber
// 
void memcpyswap(uint8_t *dest, uint8_t *src, uint32_t size)
{
	dest+=size-1;
	while(size--)
	{
		*dest=*src;
		dest--;
		src++;
	}

}
