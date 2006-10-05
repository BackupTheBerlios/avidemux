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
#include "ADM_encoder/ADM_vidEncode.hxx"

extern void setVideoEncoderSettings (COMPRESSION_MODE mode, uint32_t param,     uint32_t extraConf, uint8_t * extraData);

uint8_t A_autoDrive(Action action)
{
        //
        if(!currentaudiostream)
        {
          GUI_Error_HIG(_("No audio track"),_( "An audio track is necessary to create such file"));
                return 0;
        }

      
#define PSP_AUDIO_FQ 24000        
        switch(action)
        {
                case ACT_AUTO_PSP:
                                // Resize
                                if(!setPSP()) return 0;
                                // Video codec
#ifdef USE_XVID_4
                    if(!videoCodecSelectByName("XVID4")) 
#else
                    if(!videoCodecSelectByName("FFMpeg4"))            
#endif
                    {
                      GUI_Error_HIG(_("Codec Error"),_( "Cannot select mpeg4 sp codec."));
                        return 0;
                    }
                    // Set mode & bitrate 
                    setVideoEncoderSettings(COMPRESS_CBR,768,0,NULL);
                    // Audio Codec
                    if((currentaudiostream->getInfo()->frequency==PSP_AUDIO_FQ)&&
                        (currentaudiostream->getInfo()->channels==2)&&
                        (currentaudiostream->getInfo()->encoding==WAV_AAC))
                    {
                        audioCodecSetcodec(AUDIOENC_COPY);
                    }
                    else
                    {
#ifdef USE_FAAC
                          audioCodecSetcodec(AUDIOENC_FAAC);
#else
                          GUI_Error_HIG(_("Codec Error"),_( "You don't have FAAC!.\nIt is needed to create PSP compatible video."));
#endif
                                    // ? Needed ?
                          if(currentaudiostream->getInfo()->frequency!=PSP_AUDIO_FQ)
                          {
                              audioFilterResample(PSP_AUDIO_FQ);
                          }
                          audioFilter_SetBitrate(64);
                      }
                                break;
                case ACT_AUTO_VCD:
                                // Check video size
                                if(!setVCD()) return 0;
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
                                        if(currentaudiostream->getInfo()->channels!=2)
                                        {
                                          setCurrentMixerFromString("DOLBY_PROLOGIC2");
                                        }else
                                        {
                                          setCurrentMixerFromString("NONE");
                                        }
                                }
                                break;
                case ACT_AUTO_SVCD:
                                // Check video size
                                if(!setSVCD()) return 0;
                                // Set codec
                                if(!videoCodecSelectByName("XSVCD"))  return 0;
                                setVideoEncoderSettings (COMPRESS_2PASS_BITRATE,2000,0,NULL);
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
                                        if(currentaudiostream->getInfo()->channels!=2)
                                        {
                                          setCurrentMixerFromString("DOLBY_PROLOGIC2");
                                        }else
                                        {
                                          setCurrentMixerFromString("NONE");
                                        }
                                        audioFilter_SetBitrate(160);
                                }
                                break;
                                break;

                case ACT_AUTO_DVD:
                                // Check video size
                                if(!setDVD()) return 0;
                                // Set codec
                                if(!videoCodecSelectByName("XDVD"))  return 0;
                                // Set 2 pass, avg bitrate  6 M
                                setVideoEncoderSettings (COMPRESS_2PASS_BITRATE,6000,0,NULL);
                                // Set audio
                                if(currentaudiostream->getInfo()->frequency!=48000
                                        || (currentaudiostream->getInfo()->encoding!=WAV_MP2 &&
                                         currentaudiostream->getInfo()->encoding!=WAV_AC3))
                                {
                                        audioCodecSetcodec(AUDIOENC_2LAME);
                                        audioFilterResample(48000);
                                        audioFilter_SetBitrate(160);
                                        if(currentaudiostream->getInfo()->channels!=2)
                                        {
                                          setCurrentMixerFromString("DOLBY_PROLOGIC2");
                                        }else
                                        {
                                          setCurrentMixerFromString("NONE");
                                        }
                                }
                                else
                                {
                                        audioCodecSetcodec(AUDIOENC_COPY);
                                }
                                break;

        }
        // Set output format to mpeg PS
        // Except for PSP
        if(action==ACT_AUTO_PSP)
          UI_SetCurrentFormat(ADM_PSP);
        else
          UI_SetCurrentFormat(ADM_PS);
        
        return 1;
}
//EOF
