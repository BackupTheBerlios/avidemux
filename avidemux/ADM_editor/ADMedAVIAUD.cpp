/***************************************************************************
                          ADMedAVIAUD.cpp  -  description
                             -------------------

    Deals with the audio stream coming from the segment and try to
    present a uniq stream to ADM_edAudio

    begin                : Sat Mar 16 2002
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
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_editor/ADM_edAudio.hxx"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_EDITOR
#include "ADM_osSupport/ADM_debug.h"


#define AUDIOSEG 	_segments[_audioseg]._reference
#define SEG 		_segments[seg]._reference

extern char* ms2timedisplay(uint32_t ms);
/**
    \fn getPacket
    \brief Get audio packet

*/

uint8_t ADM_Composer::getPacket(uint8_t *dest, uint32_t *len,uint32_t sizeMax, uint32_t *samples)
{
uint8_t r;	
uint32_t ref; 
_VIDEOS *currentVideo;
	currentVideo=&_videos[AUDIOSEG];
	
	if(_audioSample<_segments[_audioseg]._audio_duration)
	{
                if( !currentVideo->_audiostream)
                {
                        printf("No soundtrack");        
                        *len=0;
                        *samples=0;
                        return 0;
                }
		r=currentVideo->_audiostream->getPacket(dest,len,sizeMax,samples); 
		if(r)
		{
			// ok we update the current sample count and go on
			_audioSample+=*samples;
			return r;		
		}
		// could not get the cound, rewind and retry
		printf("EditorPacket:Read failed; retrying (were at seg %u"   ,_audioseg);
                printf("sample %u\n",_audioSample); // FIXME use proper stuff
                printf("/ %d)\n",_segments[_audioseg]._audio_duration);
		if(_audioseg == (_nb_segment - 1))
		{		
			printf("EditorPacket : End of *last* stream\n");
			return 0;
		}
                // If we get here, it means we have less audio that it should   
                // In the current segment. Generally we repeat the segment
                // Except if the missing data is less than 20 ms, we will compensate later
                float drift;

                drift=(float)_segments[_audioseg]._audio_duration;
                drift-=(float)_audioSample;
                drift/=(float)currentVideo->_audiostream->getInfo()->frequency;
                drift*=1000.;
                printf("Seg :%u, Drop %3.3f ms\n",_audioseg,drift);
                if(drift>10.)
                {
                        printf("Drop too high, filling...\n");
                        currentVideo->_audiostream->goToTime(0);
                        r=currentVideo->_audiostream->getPacket(dest,len,sizeMax,samples); 
                        if(r)
                        {
                                printf("Filled with data from beginning (%u bytes %u samples)\n",*len,*samples);
                                // read it again sam
                                _audioSample+=*samples;
                        }
                        return r;
                }
	}
	// We have to switch seg
	// We may have overshot a bit, but...
	
	// this is the end ?
	if(_audioseg == (_nb_segment - 1))
	{
		
		printf("EditorPacket : End of stream Segment :%u/%u \n",_audioseg,_nb_segment);
                printf("This sample : %u (%s) ",_audioSample,ms2timedisplay(1000*_audioSample/currentVideo->_audiostream->getInfo()->frequency));
                printf("total :%u (%s)\n" ,_segments[_audioseg]._audio_duration,ms2timedisplay(1000*_segments[_audioseg]._audio_duration/currentVideo->_audiostream->getInfo()->frequency));
		return 0;
	}
	// switch segment
	// We adjust the audiosample to avoid adding cumulative shift
        uint8_t ret;
        int64_t adjust=_audioSample-=_segments[_audioseg]._audio_duration;

        if(adjust>0) _audioSample=adjust;
	       else  _audioSample=0;
	_audioseg++;
	// Next audio seg has audio ?
        
	// Compute new start time
	uint32_t starttime;
	if(!_videos[AUDIOSEG]._audiostream)
        {
                printf("No soundtrack\n");
                *len=0;
                *samples=0;
               // _audioseg--; // Stay in a valid one to avoid crash later
                return 0;
        }
	starttime= _videos[AUDIOSEG]._aviheader->getTime (_segments[_audioseg]._start_frame);
	_videos[AUDIOSEG]._audiostream->goToTime(starttime);	
	 // Fresh start samuel
	printf("EditorPacket : switching to segment %lu\n",_audioseg);
	ret= getPacket(dest, len,sizeMax, samples);
        if(adjust<0)
        {
                adjust=-adjust;
                if(adjust>_audioSample) _audioSample=0;
                else _audioSample-=adjust;
        }
        return ret;
}
/**
    \fn goToTime
    \brief Audio Seek

*/
uint8_t ADM_Composer::goToTime (uint64_t mstime)
{

  uint32_t	    cumul_offset =    0;
  uint32_t      frames =    0,    fi =    0;
  aviInfo       info;
  uint64_t      sample;
  double        fn;

  getVideoInfo (&info);
 // audioFlushPacket();
  
  fn = mstime;
  fn = fn * info.fps1000;
  fn /= 1000000.;

  fi = (uint32_t) floor (fn);

  aprintf (" Editor audio GOTOtime : %lu ms\n", mstime);
  aprintf (" Editor audio frame eq: %lu \n", fi);

  // now we have virtual frame number, convert to seg & real frame
  uint32_t    seg,    relframe;

  if (!convFrame2Seg (fi, &seg, &relframe))
    return 0;

  aprintf (" Editor audio seg : %lu frame %ld\n", seg, relframe);
  // compute position from previous seg
  if (seg)
    {
      for (uint32_t i = 0; i < seg; i++)
	{
	  cumul_offset += _segments[i]._audio_size;
	  frames += _segments[i]._nb_frames;
	}
    }
  // compute start frame time
  uint32_t    time_start,    time_masked,    jump;

  time_masked = getTime (frames);
  time_start = getTime (_segments[seg]._start_frame);

  jump = mstime - time_masked;
  jump = jump + time_start;

  aprintf (" Editor audio time start   : %lu\n", time_start);
  aprintf (" Editor audio time masked  : %lu\n", time_masked);
  aprintf (" Editor audio time jump    : %lu\n", jump);
  aprintf (" Editor audio seg          : %lu\n", seg);

  _audioseg = seg;
/* BAZOOKA
  _videos[AUDIOSEG]._audiostream->goToTime (jump);
  _audiooffset = _videos[AUDIOSEG]._audiostream->getPos ();
  *off = cumul_offset + _videos[AUDIOSEG]._audiostream->getPos ()
    - _segments[seg]._audio_start;
    
  // Get current duration
  // To compute how much is left in current segment
  
  float duration;
  	duration=mstime-time_masked; // Time left in current seg	
	duration/=1000;		    // how much we are in the current seg in second
	duration*=_videos[AUDIOSEG]._audiostream->getInfo()->frequency;
   _audioSample=(uint64_t)floor(duration);
   aprintf("Editor audio : we are at %llu in seg which has %llu sample\n",_audioSample,_segments[_audioseg]._audio_duration);
*/
  return 1;
}
#if 0
/*
		Go to Frame num, from segment seg
		Used to compute the duration of audio track

*/
uint8_t ADM_Composer::audioGoToFn (uint32_t seg, uint32_t fn, uint32_t * noff)
{
  uint32_t    time;

 aprintf("Editor: audioGoToFn go to : seg %lu fn %lu \n",seg,fn);

// since we got the frame we can find the segment


  if (seg >= _nb_segment)
        {
                printf("[audioGotoFn] asked : %d max :%d\n",seg,_nb_segment);
                ADM_assert(seg<_nb_segment);
        }
  ADM_assert (_videos[SEG]._audiostream);
  _audioseg=seg;
#undef AS
#define AS _videos[SEG]._audiostream

  time = _videos[SEG]._aviheader->getTime (fn);
  AS->goToTime (time);
  *noff=0;
  return AS->goToTime (time);
}

//
//  Return audio length of all segments.
//
//
uint32_t ADM_Composer::getAudioLength (void)
{
  uint32_t
    seg,
    len =
    0;				//, sz;
  //
  for (seg = 0; seg < _nb_segment; seg++)
    {
      // for each seg compute size
      len += _segments[seg]._audio_size;
    }
  _audio_size = len;
  return len;
}
#endif
/**
    \fn getAudioStream

*/
uint8_t ADM_Composer::getAudioStream (ADM_audioStream ** audio)
{
  if (*audio)
    {
      delete *audio;
      *audio = NULL;
    }
  if (!_videos[0]._audiostream)
    {
      *audio = NULL;
      return 0;

    }
  *audio = this;
  return 1;
};

/**
    \fn getInfo
    \brief returns synthetic audio info
*/
WAVHeader       *ADM_Composer::getInfo(void)
{
    if(!_videos[0]._audiostream) return NULL;
    return _videos[0]._audiostream->getInfo();
}
//EOF

