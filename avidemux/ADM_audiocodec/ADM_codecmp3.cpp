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
#include "ADM_assert.h"
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"

#ifdef USE_MP3
#include "../ADM_libMad/mad.h"
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

uint8_t ADM_AudiocodecMP3::run(uint8_t * inptr, uint32_t nbIn, float *outptr, uint32_t * nbOut)
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
     memcpy(_buffer+_tail,inptr,nbIn);
     _tail+=nbIn;

     // Now feed to libmad
     mad_stream_buffer(Stream, _buffer+_head, _tail-_head);
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
	if (MAD_NCHANNELS(&(Frame->header)) == 2) {//Stereo
		for (i = 0; i < Synth->pcm.length; i++) {
			*(outptr++) = (float) mad_f_todouble(Synth->pcm.samples[0][i]);
			*(outptr++) = (float) mad_f_todouble(Synth->pcm.samples[1][i]);
		}
		*nbOut += Synth->pcm.length * 2;
	} else {//Mono
		for (i = 0; i < Synth->pcm.length; i++) {
			*(outptr++) = (float) mad_f_todouble(Synth->pcm.samples[0][i]);
		}
		*nbOut += Synth->pcm.length;
	}
     }

     return 0;
}

#endif
