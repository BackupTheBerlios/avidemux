/***************************************************************************
                          audioeng_44sample.cpp  -  description
                             -------------------

		Borrowed from DVD2AVI, borrowed from wavefs44
		See below

    begin                : Sun Jan 13 2002
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
/*
 *	Copyright (c) 2000 by WTC
 *
 *  This file is part of WAVEFS44, a free sound sampling rate converter
 *	
 *  WAVEFS44 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  WAVEFS44 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stream.h>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
//#include "../toolkit.hxx"



//____________________________________________________
#if !defined(M_PI)
#define M_PI		3.1415926535897932384626433832795
#endif
#define ISRM		147
#define OSRM		160
#define WINSIZ		16384
#define MAXFIRODR	65535
#define ALPHA		10.0

typedef struct {
    short l;
    short r;
} SmplData;
//____________________________________________________

static int firodr[5] = { 0, 4095, 12287, 24575, 32767 };
static int lpfcof[5] = { 0, 19400, 21200, 21400, 21600 };

static int sptr, sptrr, eptr, eptrr, ismd, ismr, winpos, iptr, firodrv;
static double hfir[MAXFIRODR + 1];
static double suml, sumr, frqc, frqs, regv, wfnc, divisor, hgain;
static SmplData smpld[WINSIZ * 2], smplo, *spp;
static int16_t Sound_Max = 0;
uint32_t left = 0;

//_____________________
static double bessel0(double);

// Ctor: Duplicate
//__________

AVDMProcessAudio_Resample::AVDMProcessAudio_Resample(AVDMGenericAudioStream * instream, uint8_t quality):AVDMBufferedAudioStream
    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->frequency = 44100;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    strcpy(_name, "PROC:RESMP");
    Sound_Max = 0;
    _quality = quality;
    assert(quality > 0);
    assert(quality < 5);
    InitialSRC();

    _length = _instream->getLength() / 12;
    _length = _instream->getLength() * 11;

/*
SRC_NONE		0
SRC_LOW			1
SRC_MID			2
SRC_HIGH		3
SRC_UHIGH		4
*/

};

//_____________________________________________
uint32_t AVDMProcessAudio_Resample::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in;
    uint32_t nbout, asked;

#define ONE_CHUNK (WINSIZ<<2)
//      
// Read n samples
    // Go to the beginning of current block
    in = (uint8_t *) & smpld[winpos * WINSIZ];
    while (rdall < ONE_CHUNK)
      {
	  // don't ask too much front.
	  asked = (ONE_CHUNK) - rdall;
	  if (asked > 8192)
	      asked = 8192;

	  rd = _instream->read(asked, in + rdall);
	  rdall += rd;
	  if (rd == 0)
	      break;
      }
    // Block not filled
    if (rdall != ONE_CHUNK)
      {
	  printf("\n not enough...\n");
	  if (rdall == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
	  memset(in + rdall, 0, ONE_CHUNK - rdall);
      }
    // input buffer is full , convert it
    nbout = DownKernel(obuffer);
    winpos ^= 1;		// Swap buffer
    return nbout;
}

//_____________________________________________

uint32_t AVDMProcessAudio_Resample::DownKernel(uint8_t * out)
{
    int i, j, l;
    int32_t xx, yy;

    uint32_t done = 0;
    iptr += WINSIZ;

    while (iptr > eptr)
      {
	  suml = 0;
	  sumr = 0;
	  j = -firodrv - sptrr;

	  for (i = sptr; i <= eptr; i++, j += ISRM)
	    {
		l = j > 0 ? j : -j;

		spp = &smpld[i & ((WINSIZ << 1) - 1)];
		hgain = hfir[l];
		suml += hgain * spp->l;
		sumr += hgain * spp->r;
	    }
#define SAT(x,y) if(x>0.) y=(int32_t)floor(x+0.5); 	\
				else y=(int32_t)floor(x-0.5); 							\
				if(y<(-32768)) y=-32768; else if(y>32768) y=32768;

	  SAT(suml, xx);
	  SAT(sumr, yy);

	  smplo.l = (int16_t) xx;
	  smplo.r = (int16_t) yy;;

	  //      fwrite(&smplo, sizeof(SmplData), 1, file);
	  memcpy(out, &smplo, sizeof(SmplData));
	  out += sizeof(SmplData);
	  done += sizeof(SmplData);

#define inc(x,y)			x += ismd;			y += ismr; \
			if (y>0)		{  				y -= ISRM;				x++; 			}

	  // increment sample window
	  //
	  inc(sptr, sptrr);
	  inc(eptr, eptrr);


      }				// End while
    return done;
}

//___________________________

static double bessel0(double x)
{
    double value = 1.0, step = 1.0, part = x / 2;

    while (part > 1.0E-10)
      {
	  value += part * part;
	  step += 1.0;
	  part = (part * x) / (2.0 * step);
      }

    return value;
}

//___________________________
void AVDMProcessAudio_Resample::InitialSRC(void)
{
    int i;

    frqs = 44100 * OSRM;	// virtual high sampling rate
    frqc = lpfcof[_quality];	// cutoff freq.
    firodrv = firodr[_quality];	// FIR order = firodrv*2+1

    divisor = bessel0(ALPHA);
    hgain = (2.0 * ISRM * frqc) / frqs;
    hfir[0] = hgain;

    for (i = 1; i <= firodrv; i++)
      {
	  regv = (double) (i * i) / (firodrv * firodrv);
	  wfnc = bessel0(sqrt(1.0 - regv) * ALPHA) / divisor;
	  regv = (2.0 * M_PI * frqc * i) / frqs;
	  hfir[i] = (hgain * sin(regv) * wfnc) / regv;
      }

    ismd = OSRM / ISRM;
    ismr = OSRM % ISRM;


    eptr = firodrv / ISRM;
    eptrr = firodrv % ISRM;
    sptr = -eptr;
    sptrr = -eptr;
//      ZeroMemory(smpld, sizeof(smpld));
    memset(smpld, 0, sizeof(smpld));

    winpos = 0;
    iptr = 0;
}
