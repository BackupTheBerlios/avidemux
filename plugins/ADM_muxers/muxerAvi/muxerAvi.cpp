/***************************************************************************
            \file            muxerMP4
            \brief           i/f to lavformat mpeg4 muxer
                             -------------------
    
    copyright            : (C) 2008 by mean
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

#include "ADM_default.h"
#include "fourcc.h"
#include "muxerAvi.h"
#include "DIA_coreToolkit.h"

//#include "DIA_encoding.h"

#define ADM_NO_PTS 0xFFFFFFFFFFFFFFFFLL // FIXME

// Fwd ref
uint8_t isMpeg4Compatible (uint32_t fourcc);
uint8_t isH264Compatible (uint32_t fourcc);
uint8_t isMSMpeg4Compatible (uint32_t fourcc);
uint8_t isDVCompatible (uint32_t fourcc);

#if 1
#define aprintf(...) {}
#else
#define aprintf printf
#endif

AVIMUXERCONFIG muxerConfig=
{
    0
};


/**
    \fn     muxerMP4
    \brief  Constructor
*/
muxerAvi::muxerAvi() 
{
};
/**
    \fn     muxerMP4
    \brief  Destructor
*/

muxerAvi::~muxerAvi() 
{
    printf("[AVI] Destructing\n");

}

/**
    \fn open
    \brief Check that the streams are ok, initialize context...
*/

bool muxerAvi::open(const char *file, ADM_videoStream *s,uint32_t nbAudioTrack,ADM_audioStream **a)
{
    
        if(!writter.saveBegin (
             file,
		     s,
		     0,
             nbAudioTrack,
             a))
        {
            GUI_Error_HIG("Error","Cannot create avi file");
            return false;

        }
        vStream=s;
        nbAStreams=nbAudioTrack;
        aStreams=a;
        return true;
}

/**
    \fn save
*/
bool muxerAvi::save(void) 
{
    printf("[AVI] Saving\n");
    uint32_t bufSize=vStream->getWidth()*vStream->getHeight()*3;
    uint8_t *buffer=new uint8_t[bufSize];
    uint32_t len,flags;
    uint64_t pts,dts,rawDts;
    uint64_t lastVideoDts=0;
    uint64_t videoIncrement;
    int ret;
    int written=0;
    float f=(float)vStream->getAvgFps1000();
    f=1000./f;
    f*=1000000;
    videoIncrement=(uint64_t)f;
#define AUDIO_BUFFER_SIZE 48000*6*sizeof(float)
    uint8_t *audioBuffer=new uint8_t[AUDIO_BUFFER_SIZE];


    printf("[AVI]avg fps=%u\n",vStream->getAvgFps1000());

    while(true==vStream->getPacket(&len, buffer, bufSize,&pts,&dts,&flags))
    {
            rawDts=dts;
            if(rawDts==ADM_NO_PTS)
            {
                lastVideoDts+=videoIncrement;
            }else
            {
                lastVideoDts=dts;
            }
            uint32_t frameNo=lastVideoDts/videoIncrement;
        
            if(!writter.saveVideoFrame( len, flags,buffer))
            {
                    printf("[AVI] Error writting video frame\n");
                    break;
            }
            written++;
            // Now send audio until they all have DTS > lastVideoDts+increment
            for(int audioIndex=0;audioIndex<nbAStreams;audioIndex++)
            {
                uint32_t audioSize,nbSample;
                uint64_t audioDts;
                ADM_audioStream*a=aStreams[audioIndex];
                uint32_t fq=a->getInfo()->frequency;
                int nb=0;
                while(a->getPacket(audioBuffer,&audioSize, AUDIO_BUFFER_SIZE,&nbSample,&audioDts))
                {
                    nb=writter.saveAudioFrame(audioIndex,audioSize,audioBuffer) ;
                    aprintf("%u vs %u\n",audioDts/1000,(lastVideoDts+videoIncrement)/1000);
                    if(audioDts!=ADM_NO_PTS)
                    {
                        if(audioDts>lastVideoDts+videoIncrement) break;
                    }
                }
                if(!nb) printf("[AVI] No audio for video frame %d\n",written);
            }

    }
    writter.setEnd();
    delete [] buffer;
    delete [] audioBuffer;
    printf("[AVI] Wrote %d frames, nb audio streams %d\n",written,nbAStreams);

    return true;
}
/**
    \fn close
    \brief Cleanup is done in the dtor
*/
bool muxerAvi::close(void) 
{
 
    printf("[AVI] Closing\n");
    return true;
}

extern "C" bool AviConfigure(void)
{
    return true;
}
//EOF



