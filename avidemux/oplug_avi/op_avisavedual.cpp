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

  return 1;
}
//---------------------------------------------------------------------------
uint8_t
GenericAviSaveCopyDualAudio::writeAudioChunk (uint32_t frame)
{
 
      return 1;

}
