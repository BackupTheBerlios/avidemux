/***************************************************************************
                          avifmt2.h  -  description
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr

	This part of header is ripped directly from wine project
  See http://winehq.com to see more of their great work

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __AVIFMT2__
#define __AVIFMT2__

#if !defined( WIN32_CLASH) || !defined(CYG_MANGLING)
typedef struct
{
    uint32_t 		biSize;
    uint32_t  	biWidth;
    uint32_t  	biHeight;
    uint16_t 		biPlanes;
    uint16_t 		biBitCount;
    uint32_t 		biCompression;
    uint32_t 		biSizeImage;
    uint32_t  	biXPelsPerMeter;
    uint32_t  	biYPelsPerMeter;
    uint32_t 		biClrUsed;
    uint32_t 		biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;
typedef struct {
	BITMAPINFOHEADER bmiHeader;
	int	bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO;
#endif //win32clash

void printBih(BITMAPINFOHEADER *bi);
/* Borrowed from vdub
	Thanks to avery lee */
#define AVI_INDEX_OF_CHUNKS 0X01



typedef struct _avistdindex_chunk {
/*	uint32_t 		fcc;					// ’ix##’
	DWORD 			cb;
*/
	uint16_t 	wLongsPerEntry;		// must be sizeof(aIndex[0])/sizeof(DWORD)
	uint8_t 	bIndexSubType;			// must be 0
	uint8_t 	bIndexType;			// must be AVI_INDEX_OF_CHUNKS
	uint32_t 	nEntriesInUse;		//
	uint32_t 	dwChunkId;			// ’##dc’ or ’##db’ or ’##wb’ etc..
/*	QUADWORD 	qwBaseOffset;		// all dwOffsets in aIndex array are
														// relative to this
*/
	uint32_t  qw1,qw2;			// MN.
	uint32_t 	dwReserved3;				// must be 0
	
} AVISTDINDEX, * PAVISTDINDEX;



#define AVI_KEY_FRAME 0x10
 #define AVI_B_FRAME		 0x4000	 // hopefully it is not used..
#endif

#include "ADM_audio/ADM_audiodef.h"

void Endian_AviMainHeader(MainAVIHeader *m);
void Endian_BitMapInfo( BITMAPINFOHEADER *b);
void Endian_AviStreamHeader(AVIStreamHeader *s);
void Endian_WavHeader(WAVHeader *w);
//void Endian_BitMapHeader( BITMAPHEADER *b);

