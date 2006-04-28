/***************************************************************************
                          gui_navigate.cpp  -  description
                             -------------------

            GUI Part of get next frame, previous key frame, any frame etc...

    begin                : Fri Apr 12 2002
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


#include "avi_vars.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h"

#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "gui_action.hxx"


#include "ADM_encoder/adm_encConfig.h"
#include "ADM_filter/vidVCD.h"

uint8_t A_autoDrive(Action action)
{
        //
        if(!currentaudiostream)
        {
                GUI_Error_HIG("No audio track", "An audio track is necessary to create such file");
                return 0;
        }

        // Set output format to mpeg PS
        // Except for PSP
      if(action==ACT_AUTO_PSP)
        UI_SetCurrentFormat(ADM_PSP);
      else
        UI_SetCurrentFormat(ADM_PS);
        
        
        switch(action)
        {
                case ACT_AUTO_PSP:
                                // Resize
                                setPSP();
                                // Video codec
#ifdef USE_XVID_4
                    if(!videoCodecSelectByName("XVID4")) 
#else
                    if(!videoCodecSelectByName("FFMpeg4"))            
#endif
                    {
                        GUI_Error_HIG("Codec Error", "Cannot select mpeg4 sp codec.");
                        return 0;
                    }
                                // Audio Codec
                                if((currentaudiostream->getInfo()->frequency==44100)&&
                                    (currentaudiostream->getInfo()->channels==2)&&
                                    (currentaudiostream->getInfo()->encoding==WAV_AAC))
                                {
                                    audioCodecSetcodec(AUDIOENC_COPY);
                                }
                                else
                                {
                                    audioCodecSetcodec(AUDIOENC_FAAC);
                                    // ? Needed ?
                                    if(currentaudiostream->getInfo()->frequency!=44100)
                                    {
                                        audioFilterResample(44100);
                                    }
                                    audioFilter_SetBitrate(160);
                                }
                                break;
                                return 1;
                case ACT_AUTO_VCD:
                                // Check video size
                                setVCD();
                                // Set codec
                                if(!videoCodecSelectByName("VCD"))  return 0;
                                // Set audio
                                // Check if it is ok
                               if((currentaudiostream->getInfo()->frequency==44100)&&
                                        (currentaudiostream->getInfo()->channels==2)&&
                                                (currentaudiostream->getInfo()->encoding==WAV_MP2)&&
                                                        (currentaudiostream->getInfo()->byterate==28000))
                                {
                                        audioCodecSetcodec(AUDIOENC_COPY);
                                }
                                else
                                {
                                        audioCodecSetcodec(AUDIOENC_2LAME);
                                        if(currentaudiostream->getInfo()->frequency!=44100)
                                        {
                                                audioFilterResample(44100);
                                        }
#warning use mixer!!
                                        audioFilter_SetBitrate(224);
                                }
                                break;
                case ACT_AUTO_SVCD:
                                // Check video size
                                setSVCD();
                                // Set codec
                                if(!videoCodecSelectByName("XSVCD"))  return 0;
                                if((currentaudiostream->getInfo()->frequency==44100)&&
                                        (currentaudiostream->getInfo()->channels==2)&&
                                                (currentaudiostream->getInfo()->encoding==WAV_MP2))
                                {
                                        audioCodecSetcodec(AUDIOENC_COPY);
                                }
                                else
                                {
                                        audioCodecSetcodec(AUDIOENC_2LAME);
                                        if(currentaudiostream->getInfo()->frequency!=44100)
                                        {
                                                audioFilterResample(44100);
                                        }
                                        if(currentaudiostream->getInfo()->channels==1)
                                        {
//                                                 audioFilterMono2Stereo(1);
#warning USE mixer
                                        }
                                        audioFilter_SetBitrate(160);
                                }
                                break;
                                break;

                case ACT_AUTO_DVD:
                                // Check video size
                                setDVD();
                                // Set codec
                                if(!videoCodecSelectByName("XDVD"))  return 0;
                                // Set audio
                                if(currentaudiostream->getInfo()->frequency!=48000
                                        || (currentaudiostream->getInfo()->encoding!=WAV_MP2 &&
                                         currentaudiostream->getInfo()->encoding!=WAV_AC3))
                                {
                                        audioCodecSetcodec(AUDIOENC_2LAME);
                                        audioFilterResample(48000);
                                        audioFilter_SetBitrate(160);
                                }
                                else
                                {
                                        audioCodecSetcodec(AUDIOENC_COPY);
                                }
                                break;

        }
        return 1;
}
//EOF
