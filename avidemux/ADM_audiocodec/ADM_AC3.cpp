/***************************************************************************
                          ADM_AC3.cpp  -  description
                             -------------------

	Decode AC3 stream using liba52

 	The resync stuff is still weak.


    begin                : Sat May 25 2002
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
#include <stdlib.h>
#include <math.h>
#include <ADM_assert.h>
#include "config.h"
#ifdef USE_AC3

#include "avi_vars.h"
extern "C" {
#include "a52dec/a52.h"
#include "a52dec/mm_accel.h"
};

#include "ADM_audiocodec/ADM_AC3.h"
 //#include "ADM_audio/ADM_a52info.h"


static a52_state_t *ac3_handle = NULL;
static sample_t *ac3_sample = NULL;

uint32_t inbuffer = 0;
#define        AC3_BUF_SIZE (32*4096)
uint8_t buffer[AC3_BUF_SIZE];

uint8_t init_done = 0;
uint8_t start_req = 0;

/*
===========================================

===========================================
*/
uint8_t ADM_AC3End(void)
{
    if (init_done)
      {
	  init_done = 0;
	  a52_free(ac3_handle);
	  ac3_handle = NULL;
	  ac3_sample = NULL;

    } else
      {
	  printf("\n multiple call to ADM_AC3end ?");
      }
    return 1;

}

//_________________________________________________
//
//_________________________________________________

uint8_t ADM_AC3Init(void)
{
    inbuffer = 0;
    if (init_done)
      {
	  printf("\n Multiple call to AC3Init !");
	  return 1;
      }

    ac3_handle = a52_init(MM_ACCEL_X86_MMX);
    if (ac3_handle == NULL)
      {
	  printf("\n AC3 init failed...");
	  return 0;
      }
    ac3_sample = a52_samples(ac3_handle);
    if (ac3_sample == NULL)
      {
	  printf("\n AC3 init failed...(2)");
	  return 0;
      }

    printf("\n AC3 decoder initialized.\n=============\n");
    init_done = 1;
    start_req = 1;
    inbuffer = 0;

    return 1;

}

/*
===========================================
Returning 0 means we have a problem
We can safely return 1 with nbOut=0, it simply means :
more data please

===========================================
*/

uint8_t
ADM_AC3Run(uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,
	   uint32_t * nbOut)
{
    uint32_t length;
    int flags = 0, samprate = 0, bitrate = 0, chan = 2;
    int16_t *ptrOut = (int16_t *) outptr;

    ADM_assert(init_done);
    *nbOut = 0;
    // First copy in -> Buffer
    memcpy(buffer + inbuffer, ptr, nbIn);
    inbuffer += nbIn;
    ADM_assert(inbuffer < AC3_BUF_SIZE);
    // Not enough data to work
    if ((nbIn == 0) && (inbuffer < (3840 + 7)))
	return 0;

  lop:
//      printf("\n %lu bytes in buffer",inbuffer);
    if (inbuffer < 7)
	return 1;
    // assume it is synced
    length = a52_syncinfo(buffer, &flags, &samprate, &bitrate);
    if (length == 0)
      {
	  length = AC3_Resync();
	  if (length == 0)	// resync failed
	    {
		if (nbIn == 0)
		    return 0;	// probably end of stream
		else
		    return 1;

	    }

      }
    // enougth data in buffer ?
//              printf("\n %lu decode",length);
    if (length > inbuffer)
	return 1;

    sample_t level = 16384, bias = 0;
//               sample_t level=1, bias=384;

//              printf("\n %lu decode(2)",length);
// synthethise it ...
    if (flags & A52_STEREO)
      {
	  flags = A52_STEREO | A52_ADJUST_LEVEL;
	  chan = 2;
    } else
      {
	  flags = A52_ADJUST_LEVEL;
	  chan = 1;
      }
    //             printf("\n length : %lu ",length);
    if (a52_frame(ac3_handle, buffer, &flags, &level, bias))
      {
	  printf("\n A52_frame failed!");
	  return 0;
      };



//  and now go PCM
    uint32_t i, j;

    for (i = 0; i < 6; i++)
      {
	  if (a52_block(ac3_handle))
	    {
		printf("\n A52_block failed! on fblock :%lu", i);
		// in that case we silent out the chunk
		memset(ptrOut, 0, 256 * 2 * chan);
		*nbOut += 256 * 2 * chan;

	    }
	  // convert the float in ac3_sample to
	  // integer
	  else
	    {
		for (j = 0; j < 256; j++)
		  {
		      *ptrOut++ = (int16_t) ceil(*(ac3_sample + j));
		      if (chan == 2)
			  *ptrOut++ =
			      (int16_t) ceil(*(ac3_sample + 256 + j));
		  }
		*nbOut += 256 * 2 * chan;

	    }
      }
    //                 printf(".");
// Remove consumed.. in fact remove just enougth to resync
    memcpy(buffer, buffer + length, inbuffer - length);
    inbuffer -= length;
// and try to decode more
    goto lop;

}

//__________________________________
//      Resync stream till next frame
//
//__________________________________

uint32_t AC3_Resync(void)
{
    uint32_t length = 0;
    int flags = 0, samprate = 0, bitrate = 0;
    static uint8_t sync[8];

    //            printf("\nAC3 Resyncing");
    // Resync
    uint32_t pos = 0;
    while (1)
      {
	  // Not enough datas to sync with
	  // we did not found sync till here
	  // better drop and continue later with more
	  // stuff in buffer
	  if ((pos + 7) >= inbuffer)
	    {
		memcpy(buffer, buffer + pos, inbuffer - pos);
		inbuffer -= pos;
		return 0;
	    }
	  // need 7 bytes to sync
	  memcpy(sync, buffer + pos, 7);
	  if ((length =
	       a52_syncinfo(sync, &flags, &samprate, &bitrate)) > 0)
	    {

		printf("\n Resync at pos : %lu", pos);
		memmove(buffer, buffer + pos, inbuffer - pos);
		inbuffer -= pos;
		return length;
	  } else
	    {
		pos++;
	    }
      }
    return 1;
}


#endif
// EOF
