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
char *ADM_escape(const ADM_filename *incoming);
uint8_t ADM_findMpegStartCode(uint8_t *start, uint8_t *end,uint8_t *outstartcode,uint32_t *offset);
void memcpyswap(uint8_t *dest, uint8_t *src, uint32_t size);
uint32_t ADM_computeBitrate(uint32_t fps1000, uint32_t nbFrame, uint32_t sizeInMB);
uint32_t ADM_UsecFromFps1000(uint32_t fps1000);
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
/*

    Find mpeg1/2/4 video startcode
    00 00 01 xx yy
    return xx + offset to yy

*/
uint8_t ADM_findMpegStartCode(uint8_t *start, uint8_t *end,uint8_t *outstartcode,uint32_t *offset)
{
    uint32_t startcode=0xffffffff;
    uint8_t  *ptr=start;
  
    
    while(ptr<end)
	{
		startcode=(startcode<<8)+*ptr;		
		if((startcode&0xffffff00)==0x100)
		{
			*outstartcode=*ptr;
			*offset=ptr-start+1;
			return 1;
		}
		ptr++;
	}
	return 0; // startcode not found
}
//**********************************************************
// Convert \ to \\ 
// Needed for win32 which uses \ to store filename+path
//**********************************************************
char *ADM_escape(const ADM_filename *incoming)
{
char *out,*cur;
int to_escape=0;
int l=0;

    if(incoming)     l=strlen((char *)incoming);
    if(!l)
    {
        printf("[ADM_escape] Null string ?\n");
        out=new char[1];
        out[0]=0;
        return out;
    }
    
    for(int i=0;i<l;i++) if(incoming[i]=='\\') to_escape++;
    out=new char[l+to_escape+1];
    cur=out;
    for(int i=0;i<l;i++)
    {
        *cur++=incoming[i];
        if(incoming[i]=='\\') *cur++=incoming[i];
    }
    *cur++=0;
    return out;
}
/*
        Return average bitrate in bit/s
*/
uint32_t ADM_computeBitrate(uint32_t fps1000, uint32_t nbFrame, uint32_t sizeInMB)
{
  double    db,    ti;
  uint32_t    vbr = 0;

  db = sizeInMB;
  db = db * 1024. * 1024. * 8.;
  // now deb is in Bits

  // compute duration
  ti = nbFrame;
  ti *= 1000;
  ti /= fps1000;			// nb sec
  db = db / ti;

  vbr = (uint32_t) floor (db);
  return vbr;
}
uint32_t ADM_UsecFromFps1000(uint32_t fps1000)
{
float f;
      if(fps1000>250000) fps1000=25000; // safe default;
      if(!fps1000) fps1000=25000; // safe default;

      f=fps1000;
      f=1/f;
      f=f*1000; // In seconds
      f=f*1000000; // In us;
      return (uint32_t) floor(f);

}
//EOF
