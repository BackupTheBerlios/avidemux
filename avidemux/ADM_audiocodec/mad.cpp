/***************************************************************************
                          mad.cpp  -  description
                             -------------------

	Mad decoding part for avidemux
		GPL v2.0 blah blah

Strongly derived from madlld 0.1 by Bertrand Petit	
	
    begin                : Fri Dec 28 2001
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
#if 0
# include <stdio.h>
# include <unistd.h>
# include <math.h>

# include <config.h>


#include "avi_vars.h"
#if defined( USE_MP3)

#include "avdm_mad.h"


#include <mad.h>

#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192	/* Must be an integer multiple of 4. */

static inline signed int scale(mad_fixed_t sample);

static struct mad_stream Stream;
static struct mad_frame Frame;
static struct mad_synth Synth;


static unsigned char InputBuffer[INPUT_BUFFER_SIZE];
//__________________________________________
//
//                      Init Mad decoder
//__________________________________________

uint8_t AVDM_MadInit(void)
{
    /* First the structures used by libmad must be initialized. */
    mad_stream_init(&Stream);
    mad_frame_init(&Frame);
    mad_synth_init(&Synth);

    return 1;

}

//__________________________________________
//
//                      End Mad decoder
//__________________________________________
uint8_t AVDM_MadEnd(void)
{
    mad_synth_finish(&Synth);
    mad_frame_finish(&Frame);
    mad_stream_finish(&Stream);
    return 1;
}

/****************************************************************************
 * Main decoding loop. This is where mad is used.							*
 ****************************************************************************/
uint8_t AVDM_MadRun(uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,
		    uint32_t * nbOut)
{

    uint32_t doLength;
    int i;
    size_t ReadSize, Remaining;
    unsigned char *ReadStart;
    signed int Sample;
    /* The input bucket must be filled if it becomes empty or if
     * it's the first execution of the loop.
     */
    *nbOut = 0;

    /* libmad does not consume all the buffer it's given. Some
       * datas, part of a truncated frame, is left unused at the
       * end of the buffer. Those datas must be put back at the
       * beginning of the buffer and taken in account for
       * refilling the buffer. */
    if (Stream.next_frame != NULL)
      {
	  Remaining = Stream.bufend - Stream.next_frame;
	  memmove(InputBuffer, Stream.next_frame, Remaining);
	  ReadStart = InputBuffer + Remaining;
	  ReadSize = INPUT_BUFFER_SIZE - Remaining;
    } else
      {
	  ReadSize = INPUT_BUFFER_SIZE;
	  ReadStart = InputBuffer;
	  Remaining = 0;
      }
    /* Fill-in the buffer. If an error occurs print a message
     * and leave the decoding loop. If the end of stream is
     * reached we also leave the loop but the return status is
     * left untouched.
     */
    // we took the minium, normally it's nbIn+Remaining...
//    doLength = MIN(ReadSize, nbIn);
		if(      ReadSize<nbIn) doLength=ReadSize;
  									else		doLength=nbIn;

    memcpy(ReadStart, ptr, doLength);
    mad_stream_buffer(&Stream, InputBuffer, doLength + Remaining);
    *nbOut = 0;
    int16_t *o;
    o = (int16_t *) outptr;

    do
      {

	  /* Decode the next mpeg frame. The streams is read from the
	   * buffer, its constituents are break down and stored the the
	   * Frame structure, ready for examination/alteration or PCM
	   * synthesis. Decoding options are carried in the Frame
	   * structure from the Stream structure.
	   *
	   * Error handling: mad_frame_decode() returns a non zero value
	   * when an error occurs. The error condition can be checked in
	   * the error member of the Stream structure. A mad error is
	   * recoverable or fatal, the error status is checked with the
	   * MAD_RECOVERABLE macro.
	   *
	   * When a fatal error is encountered all decoding activities
	   * shall be stopped, except when a MAD_ERROR_BUFLEN is
	   * signaled. This condition means the mad_frame_decode()
	   * function needs more input to achieve it's work. One shall
	   * refill the buffer and repeat the mad_frame_decode() call.
	   *
	   * Recoverable errors are caused by malformed bit-streams in
	   * this case one can call mad_frame_decode() in order to skip
	   * the faulty frame and re-sync to the next frame.
	   */
	  Stream.error = MAD_ERROR_NONE;
	  if ((i = mad_frame_decode(&Frame, &Stream)))
	    {
		//printf("err: %d\n",i);
		if (MAD_RECOVERABLE(Stream.error))
		  {
		      printf(" error :%x \n",(int)Stream.error);
		
		} else
		  {
		      if (Stream.error == MAD_ERROR_BUFLEN)	// we consumed everything
			{
			    //printf("Empty,Stream.next_frame : %lx, Stream.bufend :%lx,delta :%ld",
			    //                                              Stream.next_frame,Stream.bufend,Stream.bufend-Stream.next_frame);       
			    return 1;
		      } else
			{
			    fprintf(stderr,
				    " unrecoverable frame level error ");
			    return 0;
			}
		  }
	    }
	  /* Once decoded the frame is synthesized to PCM samples. No errors
	   * are reported by mad_synth_frame();
	   */
	  mad_synth_frame(&Synth, &Frame);

	  /* Synthesized samples must be converted from mad's fixed
	   * point number to the consumer format. Here we use unsigned
	   * 16 bit big endian integers on two channels. Integer samples
	   * are temporarily stored in a buffer that is flushed when
	   * full.
	   */

	  for (i = 0; i < Synth.pcm.length; i++)
	    {
		/* Left channel */
		Sample = scale(Synth.pcm.samples[0][i]);
		*(o++) = Sample;
		*nbOut += 2;
		/* Right channel. If the decoded stream is monophonic then
		 * the right output channel is the same as the left one.
		 */
		// mean: in case of mono, we take only one channel....
		if (MAD_NCHANNELS(&Frame.header) == 2)
		  {
		      Sample = scale(Synth.pcm.samples[1][i]);
		      *(o++) = Sample;
		      *nbOut += 2;
		  }
	    }
      }
    while (1);
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


#endif
#endif
