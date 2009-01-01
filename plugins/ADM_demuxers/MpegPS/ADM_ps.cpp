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
        while(1)
        {
            if(!index->readString(2000,(uint8_t *)buffer)) return true;
            if(buffer[0]=='[') return true;
            // Now split the line
            if(strncmp(buffer,"Video ",6))
            {
                    printf("[psDemuxer] Invalid line :%s\n",buffer);
                    return false;
            }
            char *head=buffer+6;
            uint64_t pts,dts,startAt;
            uint32_t offset;
            if(4!=sscanf(head,"%"LLX":%"LX" ts:%"LLU":%"LLU,&startAt,&offset,&pts,&dts))
            {
                    printf("[psDemuxer] cannot read fields in  :%s\n",buffer);
                    return false;
            }
            dmxFrame *frame=new dmxFrame;
            frame->startAt=startAt;
            frame->index=offset;
            frame->type=1;
            frame->pts=pts;
            frame->dts=dts;
            ListOfFrames.push_back(frame);
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
     return 0;
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
        \fn getFrame
*/

uint8_t  psHeader::getFrame(uint32_t frame,ADMCompressedImage *img)
{
     
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
    *size=0;
    return true;
}


//EOF
