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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <ADM_assert.h>

#include <time.h>
#include <sys/time.h>
#include "config.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
#include "avilist.h"


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


GenericAviSaveCopyDualAudio::GenericAviSaveCopyDualAudio (AVDMGenericAudioStream	*track)
			: GenericAviSaveCopy()
{
   printf("**********************************\n");
   printf("second audio track set\n");
    audio_filter2= track;
    if(!track)
    {
		printf("BUT NULL!\n");
    }
}

//
//      Just to keep gcc happy....
//
GenericAviSaveCopyDualAudio::~GenericAviSaveCopyDualAudio ()
{

}

uint8_t GenericAviSaveCopyDualAudio::setupAudio (void)
{
//AVDMGenericAudioStream *dual;
//uint8_t status=0;

	if(! currentaudiostream)
		{
			GUI_Alert("No audio track. ");
			return 0; // we have at least one audio track ...
		}

   if (has_audio_vbr)	//VBR
			{
            		GUI_Alert("No VBR MP3 in dual audio !");
					return 0;
			}

		audio_filter=audio_filter2;
		stored_audio_frame = 0;		  

	  uint32_t	    tstart;
	  tstart = video_body->getTime (frameStart);
	  (audio_filter)->goToTime (tstart);
  //
  //  Now that the filter is ready compute the duration of a audio chunk
  //
	printf("\n______Second audio track infos__________");
  // cbr only for now
      WAVHeader *wav = NULL;

      wav = audio_filter->getInfo (); 
      computeAudioCBR ();


	  wav->blockalign = 1;

//-------------- back to track 1 ____________________
			
	   two_audio_frame_full=one_audio_frame_full;
           two_audio_frame_left=one_audio_frame_left;
           two_pcm_audio_frame=one_pcm_audio_frame;
           two_audio_stock=one_audio_stock;

			audio_filter=NULL;
			wav=NULL;
	printf("\n______First audio track infos__________");

//-----------------------------------------------------------------

  stored_audio_frame = 0;


      // else prepare the incoming raw stream
      // audio copy mode here
      audio_filter = (AVDMGenericAudioStream *) currentaudiostream;

	  tstart = video_body->getTime (frameStart);
	  (audio_filter)->goToTime (tstart);
	
  //
  //  Now that the filter is ready compute the duration of a audio chunk
  //
  // cbr only for now

      wav = audio_filter->getInfo ();
      has_audio_vbr = audio_filter->isVBR ();
      computeAudioCBR ();
	  wav->blockalign = 1;
  
  return 1;
}
//---------------------------------------------------------------------------
uint8_t
GenericAviSaveCopyDualAudio::writeAudioChunk (void)
{
  uint32_t    len;
  // if there is no audio, we do nothing
  if (!audio_filter)
    ADM_assert(0);
  //******************************
  // Audio CBR
  //******************************

      len = 0;
      one_audio_stock += one_audio_frame_left;
      one_audio_stock += 1000 * one_audio_frame_full;


	  while (one_audio_stock > 1000)
	    {
	      len++;
	      one_audio_stock -= 1000;
	    }

      if (len & 1)
	{
	  len--;
	  one_audio_stock += 1000;
	}


      // dumb me !!!!
      // hitokiri fix 2

      len = audio_filter->read (len, abuffer);

      if (len)
	{
	  writter->saveAudioFrame (len, abuffer);
	}
		// now do the second track...
//___________________________________________________________
      len = 0;
      two_audio_stock += two_audio_frame_left;
      two_audio_stock += 1000 * two_audio_frame_full;


	  while (two_audio_stock > 1000)
	    {
	      len++;
	      two_audio_stock -= 1000;
	    }

      if (len & 1)
	{
	  len--;
	  two_audio_stock += 1000;
	}


      // dumb me !!!!
      // hitokiri fix 2

      len = audio_filter2->read (len, abuffer);

      if (len)
	{
	  writter->saveAudioFrameDual (len, abuffer);
	}


      return 1;

}
