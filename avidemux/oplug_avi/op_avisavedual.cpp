/***************************************************************************
                          op_avisavedual.cpp  -  description
                             -------------------

		Save avi in copy mode for audio & video but with 2 tracks

    begin                : Wed Sep 11 2002
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
#include "avilist.h"

#include <ADM_assert.h>


#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "oplug_avi/op_savecopy.h"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
extern uint8_t audioShift;
extern int32_t audioDelay;

GenericAviSaveCopyDualAudio::GenericAviSaveCopyDualAudio (AVDMGenericAudioStream	*track)
			: GenericAviSaveCopy()
{
   printf("**********************************\n");
   printf("second audio track set\n");
    audio_filter2= track;
    _audioCurrent2=0;
}

//
//      Just to keep gcc happy....
//

uint8_t GenericAviSaveCopyDualAudio::setupAudio (void)
{
  int32_t shift=0;
  if(!audio_filter2) return 0;
  if(!currentaudiostream) return 0;
  if(audioDelay && audioShift) shift=audioDelay;
  
  audio_filter=buildRawAudioFilter( video_body->getTime (frameStart), 
      		0xffffffff, shift);
  audio_filter2->goToTime(0);
  return 1;
}
//---------------------------------------------------------------------------
uint8_t
GenericAviSaveCopyDualAudio::writeAudioChunk (uint32_t frame)
{
  uint32_t    len;
  // if there is no audio, we do nothing
  if (!audio_filter)
    return 0;
    
  double t;
  
  t=frame+1;
  t=t/fps1000;
  t=t*1000*audio_filter->getInfo()->frequency;
  _audioTarget=(uint32_t )floor(t);
  
  	uint32_t sample,packetLen,packets=0;
	
	_audioInBuffer=0;
	if(audio_filter->packetPerFrame()
		|| audio_filter->isVBR() )
	{
		while(_audioCurrent<_audioTarget)
		{
			if(!audio_filter->getPacket(abuffer,&packetLen,&sample))
			{
				printf("AVIWR:Could not read packet\n");
				return 0;
			}
			_audioCurrent+=sample;
	 		writter->saveAudioFrame (packetLen,abuffer);
			encoding_gui->feedAudioFrame(packetLen);
		}
	 	
	}
	else
	{
		sample=0;
		// _audioTarget is the # of sample we want
		while(_audioCurrent<_audioTarget)
		{
			if(!audio_filter->getPacket(abuffer+_audioInBuffer,&packetLen,&sample))
			{
				printf("AVIWR:Could not read packet\n");
				break;
			}
			_audioInBuffer+=packetLen;
			_audioTotal+=packetLen;
			_audioCurrent+=sample;		
			packets++;
		}
	}

      if (_audioInBuffer)
	{
	  writter->saveAudioFrame (_audioInBuffer, abuffer);
	  encoding_gui->feedAudioFrame(_audioInBuffer);	  
	}
	//_____________________________________________________
	// now do track2
	//_____________________________________________________
      
	_audioInBuffer=0;
	if(audio_filter2->packetPerFrame()
		|| audio_filter2->isVBR() )
	{
		while(_audioCurrent2<_audioTarget)
		{
			if(!audio_filter2->getPacket(abuffer,&packetLen,&sample))
			{
				printf("AVIWR:Could not read packet\n");
				return 0;
			}
			_audioCurrent2+=sample;
			_audioTotal+=packetLen;
	 		writter->saveAudioFrameDual (packetLen,abuffer);
			encoding_gui->feedAudioFrame(packetLen);
		}
	 	
	}
	else
	{
		sample=0;
		// _audioTarget is the # of sample we want
		while(_audioCurrent2<_audioTarget)
		{
			if(!audio_filter2->getPacket(abuffer+_audioInBuffer,&packetLen,&sample))
			{
				printf("AVIWR:Could not read packet\n");
				break;
			}
			_audioInBuffer+=packetLen;
			_audioTotal+=packetLen;
			_audioCurrent2+=sample;	
			packets++;
		}
	}

      if (_audioInBuffer)
	{
	  writter->saveAudioFrameDual (_audioInBuffer, abuffer);
	  encoding_gui->feedAudioFrame(_audioInBuffer);
	  _audioInBuffer=0;
	}
      return 1;
}
//____________
