/**
    \file ADM_psAudioProbe.cpp
    \brief Handle index file reading
    copyright            : (C) 2009 by mean
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
#include "ADM_audiodef.h"
#include "dmxPSPacket.h"
#include "ADM_psAudioProbe.h"
//
#include "ADM_a52info.h"
#include "ADM_mp3info.h"
#include "ADM_dcainfo.h"
// Number of video packet seen to be enough to sample the audio tracks
#define PROBE_PACKET_VIDEO_COUNT 500
// Max size of a packet. Usually it is a bit more than 2300, so 10000 should be safe
#define PACKET_PROBE_SIZE 10000
// Minimum of packet seen to declare it valid
#define PROBE_MIN_PACKET 5
#define PROBE_MIN_SIZE   5000

#define MP2_AUDIO_VALUE 0xC0

static bool addAudioTrack(int pid, listOfPsAudioTracks *list, psPacketLinearTracker *p);

/**
    \fn listOfPsAudioTracks
    \brief returns a list of audio track found, null if none found

*/
listOfPsAudioTracks *psProbeAudio(const char *fileName)
{
    uint32_t size;
    uint64_t dts,pts,startAt;
    uint8_t buffer[PACKET_PROBE_SIZE];
    uint64_t fileSize;

    listOfPsAudioTracks *tracks=new listOfPsAudioTracks;
    psPacketLinearTracker *packet=new psPacketLinearTracker(0xE0);

    printf("[MpegPS] Probing audio for %s\n",fileName);

    if(!packet->open(fileName,1)) goto end;
    fileSize=packet->getSize();

    packet->setPos(fileSize/2); // Jump in the middle of the stream

    while(packet->getPacketOfType(0xE0,PACKET_PROBE_SIZE,&size,&dts,&pts,buffer,&startAt))
    {
        packetStats *stat=packet->getStat(0xE0);
        if(stat->count > PROBE_PACKET_VIDEO_COUNT)
                break;
    }
    // Now synthetize
    for(int i=0x0;i<0xFF;i++)   
    {
         packetStats *stat=packet->getStat(i);
        if(stat->count)
        {
            printf("[PsProbeAudo] Pid:%x count:%d size:%d\n",i,stat->count,stat->size);
        }

         if(stat->count>=PROBE_MIN_PACKET && stat->size>PROBE_MIN_SIZE)
         {
                packet->setPos(fileSize/2); 
                addAudioTrack(i,tracks,packet);
         }

    }

end:
    printf("[PsDemux] Audio probe done, found %lu tracks\n",tracks->size());
    delete packet;
    
    if(tracks->size()==0) 
    {   
        delete tracks;
        return NULL;
    }
    return tracks;
}
/**
    \fn addAudioTrack
    \brief gather information about audio & add audio track to the list

*/
bool addAudioTrack(int pid, listOfPsAudioTracks *list, psPacketLinearTracker *p)
{
#define PROBE_ANALYZE_SIZE 5000 // Should be enough in all cases
uint8_t audioBuffer[PROBE_ANALYZE_SIZE];
        uint64_t pts,dts,startAt;
        uint32_t packetSize;

        //
        int masked=pid&0xF0;
        if(masked!=MP2_AUDIO_VALUE &&  // MP2
            masked!=0xA0 && // PCM
            masked!=0x80  // AC3 & DTS
            ) return false;

        // Go back where we were
        p->changePid(pid); 
        p->getPacketOfType(pid,PROBE_ANALYZE_SIZE, &packetSize,&pts,&dts,audioBuffer,&startAt);
        //Realign
        p->seek(startAt,0);
        int rd=PROBE_ANALYZE_SIZE;
        if(!p->read(PROBE_ANALYZE_SIZE,audioBuffer))
            return false;
        psAudioTrackInfo *info=new psAudioTrackInfo;
        info->esID=pid;
        uint32_t fq,br,chan,off;
        switch(pid & 0xF0)
        {
            case 0xA0: // LPCM
                            info->header.frequency=48000;
                            info->header.channels=2;
                            info->header.byterate=48000*4;
                            info->header.encoding=WAV_LPCM;
                            break;
            case MP2_AUDIO_VALUE: // MP2
                            {
                                info->header.encoding=WAV_MP2;
                                MpegAudioInfo mpeg;
                                if(!getMpegFrameInfo(audioBuffer, rd, &mpeg,NULL,&off))
                                {
                                        printf("[PsProbeAudio] Failed to get info on track :%x\n",pid);
                                        goto er;
                                }
                                info->header.frequency=mpeg.samplerate;
                                if(mpeg.mode==3)
                                    info->header.channels=1;
                                else
                                    info->header.channels=2;
                                info->header.byterate=mpeg.bitrate>>3;
                            }
                            break;
            case 0x80: // AC3 or DTS
                            if(pid>=0x88) // DTS
                            {
                                info->header.encoding=WAV_DTS;
                                uint32_t flags,nbSample;
                           //     if(!ADM_DCAGetInfo(audioBuffer, rd, &fq, &br, &chan,&off,&flags,&nbSample))
                                {
                                        printf("[PsProbeAudio] Failed to get info on track :%x\n",pid);
                                        goto er;
                                }
                                info->header.frequency=fq;
                                info->header.channels=chan;
                                info->header.byterate=(br>>3);
                                break;
                            }else // AC3
                            {
                                info->header.encoding=WAV_AC3;
                                if(!ADM_AC3GetInfo(audioBuffer, rd, &fq, &br, &chan,&off))
                                {
                                        printf("[PsProbeAudio] Failed to get info on track :%x\n",pid);
                                        goto er;
                                }
                                info->header.frequency=fq;
                                info->header.channels=chan;
                                info->header.byterate=(br>>3);
                                break;
                            }
                            
            default:
                        ADM_assert(0);

        }
        list->push_back(info);
        return true;
er:
        delete info;
        return false;
}
/**
        \fn DestroyListOfPsAudioTracks
        \brief cleanly destroy it
*/
bool DestroyListOfPsAudioTracks(listOfPsAudioTracks *list)
{
    while( list->size())
    {
        delete (*list)[0];
        list->erase(list->begin());
    }
    delete list;
    return true;
}
//EOF