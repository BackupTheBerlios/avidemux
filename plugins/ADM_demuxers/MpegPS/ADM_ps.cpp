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

uint32_t ADM_UsecFromFps1000(uint32_t fps1000);

/**
      \fn open
      \brief open the flv file, gather infos and build index(es).
*/

uint8_t psHeader::open(const char *name)
{
    char idxName[strlen(name)+4];
    bool r=false;
    FP_TYPE appendType=FP_DONT_APPEND;
    uint32_t append;
    char *type;

    sprintf(idxName,"%s.idx",name);
    indexFile index;
    if(!index.open(idxName))
    {
        printf("[psDemux] Cannot open index file %s\n",idxName);
        return false;
    }
    if(!index.readSection("System"))
    {
        printf("[psDemux] Cannot read system section\n");
        goto abt;
    }
    type=index.getAsString("Type");
    if(!type || type[0]!='P')
    {
        printf("[psDemux] Incorrect or not found type\n");
        goto abt;
    }
    append=index.getAsUint32("Append");

    if(append) appendType=FP_APPEND;
    if(!parser.open(name,&appendType))
    {
        printf("[psDemux] Cannot open root file\n",name);
        goto abt;
    }
    if(!readVideo(&index)) 
    {
        printf("[psDemux] Cannot read Video section of %s\n",idxName);
        goto abt;
    }
    if(!readIndex(&index))
    {
        printf("[psDemux] Cannot read index for file %s\n",idxName);
        goto abt;
    }
    _videostream.dwLength= _mainaviheader.dwTotalFrames=ListOfFrames.size();
    printf("[psDemux] Found %d video frames\n",_videostream.dwLength);
    if(_videostream.dwLength)_isvideopresent=1;
//***********
    
    psPacket=new psPacketLinear(0xE0);
    if(psPacket->open(name,append)==false) 
    {
        printf("psDemux] Cannot psPacket open the file\n");
        goto abt;
    }
    r=true;
abt:
    index.close();
    printf("[psDemuxer] Loaded %d\n",r);
    return r;
}
/**
        \fn readVideo
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
            if(strncmp(buffer,"Video ",6))
            {
                    printf("[psDemuxer] Invalid line :%s\n",buffer);
                    return false;
            }
            char *head=buffer+6;
            uint64_t pts,dts,startAt;
            uint32_t offset;
            if(4!=sscanf(head,"at:%"LLX":%"LX" Pts:%"LLD":%"LLD,&startAt,&offset,&pts,&dts))
            {
                    printf("[psDemuxer] cannot read fields in  :%s\n",buffer);
                    return false;
            }
            
            char *start=strstr(buffer," I:");
            if(!start) continue;
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
                    printf("[psDemux] <%s> instead of : (%c %x %x):\n",*cur,*(cur-1),*cur);
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

         
        }

    return false;
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
        \fn getVideoDuration
        \brief Returns duration of video in us
*/
uint64_t psHeader::getVideoDuration(void)
{
    float f;
        f=_videostream.dwRate;
        f*=1000;
        f*=ListOfFrames.size();
     return (uint64_t)f;
}


/**
    \fn getAudioInfo
    \brief returns wav header for stream i (=0)
*/
WAVHeader *psHeader::getAudioInfo(uint32_t i )
{
 
      return NULL;
}
/**
   \fn getAudioStream
*/

uint8_t   psHeader::getAudioStream(uint32_t i,ADM_audioStream  **audio)
{
  
  *audio=NULL;
  return 0; 
}
/**
    \fn getNbAudioStreams

*/
uint8_t   psHeader::getNbAudioStreams(void)
{
 
  return 0; 
}
/*
    __________________________________________________________
*/

void psHeader::Dump(void)
{
 
}
/**
    \fn close
    \brief cleanup
*/

uint8_t psHeader::close(void)
{
    // Destroy index
    while(ListOfFrames.size())
    {
        delete ListOfFrames[0];
        ListOfFrames.erase(ListOfFrames.begin());
    }
    if(psPacket)
    {
        psPacket->close();
        delete psPacket;
        psPacket=NULL;
    }
}
/**
    \fn psHeader
    \brief constructor
*/

 psHeader::psHeader( void ) : vidHeader()
{ 
    interlaced=false;
    lastFrame=0xffffffff;
    
}
/**
    \fn psHeader
    \brief destructor
*/

 psHeader::~psHeader(  )
{
  close();
}


/**
    \fn setFlag
    \brief Returns timestamp in us of frame "frame" (PTS)
*/

  uint8_t  psHeader::setFlag(uint32_t frame,uint32_t flags)
{
   
     uint32_t f=2;
     if(flags & AVI_KEY_FRAME) f=1;
     if(flags & AVI_B_FRAME) f=3;
     if(frame>=ListOfFrames.size()) return 0;
      ListOfFrames[frame]->type=f;
    return 1;
}
/**
    \fn getFlags
    \brief Returns timestamp in us of frame "frame" (PTS)
*/

uint32_t psHeader::getFlags(uint32_t frame,uint32_t *flags)
{
    if(frame>=ListOfFrames.size()) return 0;
    uint32_t f=ListOfFrames[frame]->type;
    switch(f)
    {
        case 1: *flags=AVI_KEY_FRAME;break;
        case 2: *flags=0;break;
        case 3: *flags=AVI_B_FRAME;break;
    }
    return  1;
}

/**
    \fn getTime
    \brief Returns timestamp in us of frame "frame" (PTS)
*/
uint64_t psHeader::getTime(uint32_t frame)
{
   if(frame>=ListOfFrames.size()) return 0;
    uint64_t pts=ListOfFrames[frame]->pts;
    return pts;
}
/**
    \fn timeConvert
    \brief FIXME
*/
uint64_t psHeader::timeConvert(uint64_t x)
{
    if(x==ADM_NO_PTS) return ADM_NO_PTS;
    x=x-ListOfFrames[0]->pts;
    return x;

}
/**
        \fn getFrame
*/

uint8_t  psHeader::getFrame(uint32_t frame,ADMCompressedImage *img)
{
    if(frame>=ListOfFrames.size()) return 0;
    dmxFrame *pk=ListOfFrames[frame];
    if(frame==(lastFrame+1) && pk->type!=1)
    {
        lastFrame++;
        bool r=psPacket->read(pk->len,img->data);
             img->dataLength=pk->len;
             img->demuxerFrameNo=frame;
             img->demuxerDts=timeConvert(pk->dts);
             img->demuxerPts=timeConvert(pk->pts);
             //printf("[>>>] %d:%02x %02x %02x %02x\n",frame,img->data[0],img->data[1],img->data[2],img->data[3]);
             getFlags(frame,&(img->flags));
             return r;
    }
    if(pk->type==1)
    {
        if(!psPacket->seek(pk->startAt,pk->index)) return false;
         bool r=psPacket->read(pk->len,img->data);
             img->dataLength=pk->len;
             img->demuxerFrameNo=frame;
             img->demuxerDts=timeConvert(pk->dts);
             img->demuxerPts=timeConvert(pk->pts);
             getFlags(frame,&(img->flags));
             //printf("[>>>] %d:%02x %02x %02x %02x\n",frame,img->data[0],img->data[1],img->data[2],img->data[3]);
             lastFrame=frame;
             return r;

    }
    printf(" [PsDemux] lastFrame :%d this frame :%d\n",lastFrame,frame);
    return false;
}
/**
        \fn getExtraHeaderData
*/
uint8_t  psHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
                *len=0; //_tracks[0].extraDataLen;
                *data=NULL; //_tracks[0].extraData;
                return true;            
}

/**
      \fn getFrameSize
      \brief return the size of frame frame
*/
uint8_t psHeader::getFrameSize (uint32_t frame, uint32_t * size)
{
    if(frame>=ListOfFrames.size()) return 0;
    *size=ListOfFrames[frame]->len;
    return true;
}


//EOF
