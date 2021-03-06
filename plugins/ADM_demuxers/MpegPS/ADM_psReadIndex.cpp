/***************************************************************************
    copyright            : (C) 2007/2009 by mean
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
#include "DIA_coreToolkit.h"
#include "ADM_indexFile.h"
#include "ADM_ps.h"

#include <math.h>
/**
        \fn readIndex
        \brief Read the [video] section of the index file

*/
bool    psHeader::readIndex(indexFile *index)
{
char buffer[2000];
        printf("[psDemuxer] Reading index\n");
        if(!index->goToSection("Data")) return false;
      
        while(1)
        {
            if(!index->readString(2000,(uint8_t *)buffer)) return true;
            if(buffer[0]=='[') return true;
            if(buffer[0]==0xa || buffer[0]==0xd) continue; // blank line
            // Now split the line
            if(!strncmp(buffer,"Video ",6))
            {
                processVideoIndex(buffer+6);
            }
            if(!strncmp(buffer,"Audio ",6))
            {
                processAudioIndex(buffer+6);
            }
        }
    return true;
}
/**
    \fn processAudioIndex
    \brief process audio seek points from a line from the index file
*/
bool psHeader::processAudioIndex(char *buffer)
{
    return true;

}

/**
    \fn processVideoIndex
    \brief process an mpeg index entry from a line from the index file
*/
bool psHeader::processVideoIndex(char *buffer)
{
            char *head=buffer;
            uint64_t pts,dts,startAt;
            uint32_t offset;
            if(4!=sscanf(head,"at:%"LLX":%"LX" Pts:%"LLD":%"LLD,&startAt,&offset,&pts,&dts))
            {
                    printf("[psDemuxer] cannot read fields in  :%s\n",buffer);
                    return false;
            }
            
            char *start=strstr(buffer," I:");
            if(!start) return true;
            start+=1;
            int count=0;
            while(1)
            {
                char *cur=start;
                char type=1;
                char *next;
                uint32_t len;
                type=*cur;
                if(type==0x0a || type==0x0d) break;
                cur++;
                if(*(cur)!=':')
                {
                    printf("[psDemux]  instead of : (%c %x %x):\n",*cur,*(cur-1),*cur);
                }
                *cur++;
                next=strstr(start," ");
                ADM_assert(1==sscanf(cur,"%"LX,&len));
                
                
                dmxFrame *frame=new dmxFrame;
                if(!count)
                {
                    frame->pts=pts;
                    frame->dts=dts;

                    frame->startAt=startAt;
                    frame->index=offset;

                }else       
                {
                    frame->pts=ADM_NO_PTS;
                    frame->dts=ADM_NO_PTS;
                    frame->startAt=0;
                    frame->index=0;
                }
                switch(type)
                {
                    case 'I': frame->type=1;break;
                    case 'P': frame->type=2;break;
                    case 'B': frame->type=3;break;
                    default: ADM_assert(0);
                }
                frame->len=len;
                ListOfFrames.push_back(frame);
                count++;
                if(!next) 
                {
                    break;
                }
                start=next+1;
            }

        return true;
}
/**
        \fn updatePtsDts
        \brief Update the PTS/DTS

TODO / FIXME : Handle wrap
TODO / FIXME : Handle PTS reordering 
*/
bool psHeader::updatePtsDts(void)
{
        uint64_t lastDts=0,lastPts=0,dtsIncrement=0;
        switch( _videostream.dwRate)
        {
            case 25000:   dtsIncrement=40000;break;
            case 23976:   dtsIncrement=41708;break;
            case 29970:   dtsIncrement=33367;break;
            default : dtsIncrement=1;
                    printf("[psDemux] Fps not handled for DTS increment\n");

        }
        for(int i=0;i<ListOfFrames.size();i++)
        {
            dmxFrame *frame=ListOfFrames[i];
            if(frame->pts==ADM_NO_PTS || frame->dts==ADM_NO_PTS)
            {
                lastDts+=dtsIncrement;
                lastPts+=dtsIncrement;
                frame->dts=lastDts;
                //frame->pts=lastPts; // THIS IS WRONG NEED REORDERING

            }else      
            {
                frame->dts=lastDts=timeConvert(frame->dts);
                frame->pts=lastPts=timeConvert(frame->pts);
            }
        }
        return 1;
                    
}
/**
        \fn readVideo
        \brief Read the [video] section of the index file

*/
bool    psHeader::readVideo(indexFile *index)
{
    printf("[psDemuxer] Reading Video\n");
    if(!index->readSection("Video")) return false;
    uint32_t w,h,fps,ar;
    
    w=index->getAsUint32("Width");
    h=index->getAsUint32("height");
    fps=index->getAsUint32("Fps");

    if(!w || !h || !fps) return false;

    interlaced=index->getAsUint32("Interlaced");
    
    _video_bih.biWidth=_mainaviheader.dwWidth=w ;
    _video_bih.biHeight=_mainaviheader.dwHeight=h;             
    _videostream.dwScale=1000;
    _videostream.dwRate=fps;

    _videostream.fccHandler=_video_bih.biCompression=fourCC::get((uint8_t *)"MPEG");

    return true;
}
/**
        \fn readAudio
        \brief Read the [Audio] section of the index file

*/
bool    psHeader::readAudio(indexFile *index,const char *name)
{
    printf("[psDemuxer] Reading Audio\n");
    if(!index->readSection("Audio")) return false;
    uint32_t nbTracks;
    
    nbTracks=index->getAsUint32("Tracks");
    if(!nbTracks)
    {
        printf("[PsDemux] No audio\n");
        return true;
    }
    for(int i=0;i<nbTracks;i++)
    {
        char header[40];
        char body[40];
        uint32_t fq,chan,br,codec,pid;
        sprintf(header,"Track%d.",i);
#define readInt(x,y) {sprintf(body,"%s"#y,header);x=index->getAsUint32(body);printf("%02d:"#y"=%"LU"\n",i,x);}
#define readHex(x,y) {sprintf(body,"%s"#y,header);x=index->getAsHex(body);printf("%02x:"#y"=%"LU"\n",i,x);}
        readInt(fq,fq);
        readInt(br,br);
        readInt(chan,chan);
        readInt(codec,codec);
        readHex(pid,pid);
        WAVHeader hdr;
            hdr.frequency=fq;
            hdr.byterate=br;
            hdr.channels=chan;
            hdr.encoding=codec;
        ADM_psAccess *access=new ADM_psAccess(name,pid,true);
        ADM_audioStream *audioStream=ADM_audioCreateStream(&hdr,access);
        if(!audioStream)
        {
            delete access;
        }else       
        {
            ADM_psTrackDescriptor *desc=new ADM_psTrackDescriptor;
            desc->stream=audioStream;
            desc->access=access;
            listOfAudioTracks.push_back(desc);
        }

    }
    return true;
}
//EOF