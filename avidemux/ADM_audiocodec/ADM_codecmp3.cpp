/***************************************************************************
                          ADM_codecmp3.cpp  -  description
                             -------------------
    begin                : Fri May 31 2002
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
#include <string.h>
//#include <sstream>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"

#ifdef USE_MP3
#include "mad.h"

#define Stream ((mad_stream *)_stream)
#define Frame ((mad_frame *)_frame)
#define Synth ((mad_synth *)_synth)

ADM_AudiocodecMP3::ADM_AudiocodecMP3( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
        if((fourcc!=WAV_MP3) && (fourcc!=WAV_MP2))
            ADM_assert(0);
        if(fourcc==WAV_MP2) printf("Mpeg1/2 audio codec created\n");
        _stream=(void *)ADM_alloc(sizeof( mad_stream));
        _frame=(void *)ADM_alloc(sizeof( mad_frame));
        _synth=(void *)ADM_alloc(sizeof( mad_synth));
        
        
        mad_stream_init(Stream);
        mad_frame_init(Frame);
        mad_synth_init(Synth);
        
        _head=_tail=0;

}
ADM_AudiocodecMP3::~ADM_AudiocodecMP3( )
{
    mad_synth_finish(Synth);
    mad_frame_finish(Frame);
    mad_stream_finish(Stream);
    ADM_dealloc(_stream);
    ADM_dealloc(_frame);
    ADM_dealloc(_synth);
    _synth=_synth=_stream=NULL;
    
}
uint8_t ADM_AudiocodecMP3::beginDecompress( void )
{
        mad_stream_init(Stream);
        mad_frame_init(Frame);
        mad_synth_init(Synth);
        _head=_tail=0;
        return 1;
}
uint8_t ADM_AudiocodecMP3::endDecompress( void )
{
    mad_synth_finish(Synth);
    mad_frame_finish(Frame);
    mad_stream_finish(Stream);
    _head=_tail=0;
    return 1;
}
static inline signed int scale(mad_fixed_t sample)
{
signed int r;
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
	sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
	sample = -MAD_F_ONE;

    /* quantize */
    r= sample >> (MAD_F_FRACBITS + 1 - 16);	
// this was wrong    
//#ifdef ADM_BIG_ENDIAN && 0 
#if defined(ADM_BIG_ENDIAN)   && 0
	r= (r>>8)+((r&0xff)<<8);	
#endif
  return r;
}
uint8_t ADM_AudiocodecMP3::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
int i;
signed int Sample;
     *nbOut = 0;
     // Shrink ?
     if(ADM_MP3_BUFFER<=_tail+nbIn)
     {
        memmove(_buffer,_buffer+_head,_tail-_head);
        _tail-=_head;
        _head=0;
     }
     ADM_assert(_tail+nbIn<ADM_MP3_BUFFER);
     memcpy(_buffer+_tail,ptr,nbIn);
     _tail+=nbIn;
     
     // Now feed to libmad
     mad_stream_buffer(Stream, _buffer+_head, _tail-_head);
     int16_t *o=(int16_t *) outptr;
     while(1)
     {
         Stream->error = MAD_ERROR_NONE;
        if ((i = mad_frame_decode(Frame, Stream)))
        {
            if (MAD_RECOVERABLE(Stream->error))
            {
                    printf(" error :%x \n",(int)Stream->error);
            } else
            {
                if (Stream->error == MAD_ERROR_BUFLEN)	// we consumed everything
                {
                    uint32_t left=0;
                    //printf("Empty,Stream.next_frame : %lx, Stream.bufend :%lx,delta :%ld",
                    //            Stream.next_frame,Stream.bufend,Stream.bufend-Stream.next_frame);  
                    // Update _head
                    if (Stream->next_frame != NULL)
                    {
                        left = Stream->bufend - Stream->next_frame;                        
                    }    
                    ADM_assert(left<=_tail-_head);
                    _head=_tail-left; 
                    return 1;
                 } else
                {
                     fprintf(stderr," unrecoverable frame level error ");
                     return 0;
                }
            }
        }

        mad_synth_frame(Synth, Frame);
        for (i = 0; i < Synth->pcm.length; i++)
        {
            /* Left channel */
                Sample = scale(Synth->pcm.samples[0][i]);
                *(o++) = Sample;
                *nbOut += 2;
                /* Right channel. If the decoded stream is monophonic then
                 * the right output channel is the same as the left one.
                */
                // mean: in case of mono, we take only one channel....
                if (MAD_NCHANNELS(&(Frame->header)) == 2)
                {
                    Sample = scale(Synth->pcm.samples[1][i]);
                    *(o++) = Sample;
                    *nbOut += 2;
                }
          }
     
     }
     return 0;
}

#endif
