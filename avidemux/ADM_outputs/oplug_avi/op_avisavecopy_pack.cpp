/***************************************************************************
        
    Pack a mpeg4 the divx way
    Sometimes needed for some DVD/multimedia stuff
	
    
    copyright            : (C) 2007 by mean
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

#include <time.h>
#include <sys/time.h>
#include "config.h"
#include <pthread.h>
#define WIN32_CLASH
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
//#include "avilist.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "oplug_avi/op_savecopy.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_SAVE_AVI
#include "ADM_osSupport/ADM_debug.h"

uint8_t extractMpeg4Info(uint8_t *data,uint32_t dataSize,uint32_t *w,uint32_t *h,uint32_t *time_inc);
static  void updateUserData(uint8_t *start, uint32_t len);
uint8_t ADM_findMpegStartCode(uint8_t *start, uint8_t *end,uint8_t *outstartcode,uint32_t *offset);
static  void putNvop(ADMBitstream *data,uint32_t time_inc);

/**
      \fn  ~GenericAviSaveCopyUnpack
      \brief destructor
*/
GenericAviSaveCopyPack::~GenericAviSaveCopyPack ()
{
      if(lookAhead[0]) 
      {
          delete [] lookAhead[0]->data;
          delete [] lookAhead[1]->data;
          delete lookAhead[0];
          delete lookAhead[1];
      }
      lookAhead[0]=NULL;
      lookAhead[1]=NULL;
}
/**
      \fn GenericAviSaveCopyUnpack::setupVideo
      \brief init for unpacker code

*/
uint8_t GenericAviSaveCopyPack::setupVideo (char *name)
{
  printf("Setting up bitstream packer\n");
  //  Setup avi file output, all is coming from original avi
  // since we are inc copy mode
  memcpy(&_bih,video_body->getBIH (),sizeof(_bih));
  _bih.biSize=sizeof(_bih);  //fix old version of avidemux
  _bih.biXPelsPerMeter=_bih.biClrUsed=_bih.biYPelsPerMeter=0;
  //
  memcpy(&_videostreamheader,video_body->getVideoStreamHeader (),sizeof( _videostreamheader));
  memcpy(&_mainaviheader,video_body->getMainHeader (),sizeof(_mainaviheader));
  
  // Change both to divx/DX50
  	_videostreamheader.fccHandler=fourCC::get((uint8_t *)"divx");
	_bih.biCompression=fourCC::get((uint8_t *)"DX50");
  /* update to fix earlier bug */
   _mainaviheader.dwWidth=_bih.biWidth;
   _mainaviheader.dwHeight=_bih.biHeight;

   uint8_t *extraData;
   uint32_t extraLen;
  _lastIPFrameSent=0xfffffff;
   video_body->getExtraHeaderData(&extraLen,&extraData);
    if(extraLen>3)
    {
      uint32_t w,h,ti;
      if(extractMpeg4Info(extraData,extraLen,&w,&h,&ti) )
      {
        time_inc=ti;
        printf("Found info : %u  x %u, timeinc %u\n",w,h,ti); 
      }
    }
      
  	if (!writter->saveBegin (name,
			   &_mainaviheader,
			   frameEnd - frameStart + 1,
			   &_videostreamheader,
			   &_bih,
			   extraData,extraLen,
			   audio_filter,
			   audio_filter2
		))
    	{
          GUI_Error_HIG (_("Cannot initiate save"), NULL);
      		return 0;
    	}
	if(audio_filter2)
	{
		printf("Second audio track present\n");
	}
	else
	{
		printf("Second audio track absent\n");
	}
 _incoming = getFirstVideoFilter (frameStart,frameEnd-frameStart);
 encoding_gui->setFps(_incoming->getInfo()->fps1000);
 encoding_gui->setPhasis("Saving");
 
 // Set up our copy codec ...
  copy=new EncoderCopy(NULL);
  if(!copy->configure(_incoming))
  {
      printf("Copy cannot [configure] \n");
      return 0;
  }
  // Our buffer
#define LOOK_SIZE 2*3*_incoming->getInfo ()->width *   _incoming->getInfo ()->height * 3
  uint8_t *buf=new uint8_t[LOOK_SIZE];
           lookAhead[0]=new ADMBitstream(LOOK_SIZE);
           lookAhead[0]->data=buf;
            buf=new uint8_t[LOOK_SIZE];
           lookAhead[1]=new ADMBitstream(LOOK_SIZE);
           lookAhead[1]->data=buf;

  return 1;
}
/**
        \fn prefetch 
        \brief Read frame FRAME in buffer BUFFER
*/

uint8_t GenericAviSaveCopyPack::prefetch(uint32_t buffer,uint32_t frame)
{
  uint8_t r=0;
  ADM_assert(copy); 
  ADM_assert(buffer==0 || buffer==1);
  
  aprintf("Fetching frame %u buffer%u\n",frame,buffer);
        r=copy->encode(frame,lookAhead[buffer]);
        if(!r)
        {
            aprintf("Prefetching  frame %u in buffer %u failed\n",frame, buffer);
        }
        else
        {
          if((lookAhead[buffer]->flags & AVI_KEY_FRAME ) && !time_inc)
          {
                uint32_t w,h,ti;
                if(extractMpeg4Info(lookAhead[buffer]->data,lookAhead[buffer]->len,&w,&h,&ti) )
                {
                  time_inc=ti;
                  printf("Found info : %u  x %u, timeinc %u\n",w,h,ti); 
                }
          }
        }
      return r;
  
}
// copy mode
// Basically ask a video frame and send it to writter
// If it contains b frame and frames have been re-ordered
// reorder them back ...
/**
      \fn GenericAviSaveCopyUnpack::setupVideo
      \brief init for unpacker code

*/
uint8_t GenericAviSaveCopyPack::writeVideoChunk (uint32_t frame)
{
  
  uint8_t    ret1;
 ADMCompressedImage img;
 
      img.data=vbuffer;      
      
       if(!video_body->isReordered(frameStart+frame))
      {
          ret1 = video_body->getFrameNoAlloc (frameStart + frame,&img);
          _videoFlag=img.flags;
      }
      else
      {
            ret1=1;
            // We prefetch one frame...
           if(!frame) // First frame.
           {
                if(!prefetch(0,frame))
                {
                  return 0;
                }
                curToggle=0;
           }
           uint32_t len=0;
            ADMBitstream *current,*next;
                 current=lookAhead[curToggle];
                 next=lookAhead[curToggle^1];
           if(frame+2<_incoming->getInfo()->nb_frames)
           {
               
                 
                if( !prefetch(curToggle^1,frame+1))
                    {
                        return 0; 
                    }
                // Curtoggle holds the current frame, curToggle ^1 hold the next frame
                if(current->flags!=AVI_B_FRAME && next->flags==AVI_B_FRAME)
                {
                    aprintf("Packing frame :%u\n",frame);
                    // We need to pack this
                    len=current->len;
                    memcpy(vbuffer,current->data,len);
                    memcpy(vbuffer+len,next->data,next->len);
                    len+=next->len;
                    img.dataLength=len;
                    // Put nvop in next buffer
                    putNvop(next,time_inc);
                }else
                {
                    // Just send 
                    len=current->len;
                    memcpy(vbuffer,current->data,len);             
                }
        
           }
           else
           { 
              // Last frame
             aprintf("Last frame\n");
              len= current->len;
              memcpy(vbuffer,current->data,len);
           }
           img.dataLength=len;
           _videoFlag=img.flags=current->flags;
           if(_videoFlag==AVI_KEY_FRAME)
           {
            updateUserData(vbuffer,len); 
           }
           curToggle^=1;
      }

  if (!ret1)
    return 0;

     if(_videoFlag==AVI_KEY_FRAME)
          newFile();

  aprintf("Writting frame %u size %u flags %x\n",frame,img.dataLength,_videoFlag);  
  encoding_gui->setFrame(frame,img.dataLength,0,frametogo);
  return writter->saveVideoFrame (img.dataLength, img.flags, img.data);

}

//_____________________________________________________
// Update the user data field that is used to 
// detect in windows world if it is packed or not
//_____________________________________________________
static char signature[]="DivX999b0000p\0";
void updateUserData(uint8_t *start, uint32_t len)
{
      // lookup startcode
      uint32_t sync,off,rlen;
      uint8_t code;
      uint8_t *end=start+len;
      while(ADM_findMpegStartCode(start, end,&code,&off))
      {
              // update
              start+=off;
              rlen=end-start;
              if(code!=0xb2 || rlen<4)
                  continue;
      
              printf("User data found\n");
              // looks ok ?
              if(!strncmp((char *)start,"DivX",4) && start[7]=='b'&& rlen>=14)
              {
                memcpy(start,signature,4+3+4+1+1+1); // 14
              }
      }
}
/**
        \fn     putNvop
        \brief  Put a vop non coded bit
        
        
        00 00 01 B6
        2 bits : B vop : 10
        1*X : Stuffing
        1*0 : End of stuffing
        1*0 : Marker
        1*time_bit : time
        1*0 : Marker
        1*0 : Vop coded
    
*/
extern "C"
{
#define av_always_inline inline
#include "ADM_libraries/ADM_lavutil/bswap.h" 
#include "ADM_libraries/ADM_lavcodec/bitstream.h" 
}
void putNvop(ADMBitstream *data,uint32_t time_inc)
{
  ADM_assert(data->data[0]==0);
  ADM_assert(data->data[1]==0);
  ADM_assert(data->data[2]==1);
  ADM_assert(data->data[3]==0xB6); // Vop start
  
  
  ADM_assert(data->len>=6);
  ADM_assert(time_inc);
  
  PutBitContext pbs;
  init_put_bits(&pbs, data->data+4, data->len-4);
    
  put_bits(&pbs, 2,2); // Bvop
  put_bits(&pbs, 2,2); // Padding
  put_bits(&pbs, 1,0); // Marker
  if(time_inc!=0xFFFF)
    put_bits(&pbs, time_inc,0); // time_inc
  put_bits(&pbs, 1,0); // Marker
  put_bits(&pbs, 1,0); // vop not coded
  
  flush_put_bits(&pbs);
  int nb=put_bits_count(&pbs)>>3;
  data->len=nb;
}
//EOF
