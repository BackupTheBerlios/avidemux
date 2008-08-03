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
    \fn     getPCMPacket
    \brief  Get audio packet

*/

uint8_t ADM_Composer::getPCMPacket(float  *dest, uint32_t sizeMax, uint32_t *samples,uint64_t *odts)
{
uint32_t nbSamples;
uint64_t dts;
uint32_t inSize,nbOut;
    if(!_videos[0]._audiostream) return 0;
    // Read a packet from stream 0
    if(!_videos[0]._audiostream->getPacket(audioBuffer,&inSize,ADM_EDITOR_AUDIO_BUFFER_SIZE,&nbSamples,&dts))
    {
            adm_printf(ADM_PRINT_ERROR,"[Composer::getPCMPacket] Read failed\n");
            return 0;
    }
    // Call codec...
    if(!_videos[0]._audioCodec->run(audioBuffer, inSize, dest, &nbOut))
    {
            adm_printf(ADM_PRINT_ERROR,"[Composer::getPCMPacket] codec failed failed\n");
            return 0;
    }
    // Update infos
    *samples=nbOut;
    *odts=dts;
    return 1;
}
/**
        \fn getPacket
        \brief
*/
uint8_t ADM_Composer::getPacket(uint8_t  *dest, uint32_t *len,uint32_t sizeMax, uint32_t *samples,uint64_t *odts)
{
    if(!_videos[0]._audiostream) return 0;
    // Read a packet from stream 0
     return _videos[0]._audiostream->getPacket(dest,len,sizeMax,samples,odts);
    
}
/**
    \fn goToTime
    \brief Audio Seek in ms

*/
bool ADM_Composer::goToTime (uint64_t ustime)
{
    printf("[Editor] go to time %02.2f secs\n",((float)ustime)/1000000.);
    if(!_videos[0]._audiostream) return false;
    return _videos[0]._audiostream->goToTime(ustime);
 
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

