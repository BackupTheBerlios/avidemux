/***************************************************************************
                          ADM_edRender.cpp  -  description
                             -------------------
     This file renders the compressed video depending on the availabilty of
     		CODECs.      It also deals with key frame and stuff


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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "avi_vars.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_EDITOR
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_pp.h"

// FIXME BADLY !!!
static uint8_t compBuffer[MAXIMUM_SIZE * MAXIMUM_SIZE * 3];



uint8_t
  ADM_Composer::getRaw (uint32_t framenum, uint8_t * ptr, uint32_t * framelen)
{
  uint32_t relframe, seg;	//,flags,len;

//uint8_t ret=0;

  if (!convFrame2Seg (framenum, &seg, &relframe))
    {
      printf ("\n Conversion failed !\n");
      return 0;
    }
  uint32_t ref = _segments[seg]._reference;
  return _videos[ref]._aviheader->getRaw (relframe, ptr, framelen);
}

uint8_t
  ADM_Composer::getRawStart (uint32_t frame, uint8_t * ptr, uint32_t * len)
{
  uint32_t relframe, seg;	//,flags;
//uint8_t ret=0;

  if (!convFrame2Seg (frame, &seg, &relframe))
    {
      printf ("\n Conversion failed !\n");
      return 0;
    }
  uint32_t ref = _segments[seg]._reference;
  return _videos[ref]._aviheader->getRawStart (ptr, len);

}
/**_____________________________________________________________________
		Main function
		Caller ask for a frame from the editor
		We first split it to segment and frame from that segment
			then look if it is in the cache
		If not and if there is Bframe we look if it is the forward
			reference frame we currently have

_______________________________________________________________________*/
uint8_t  ADM_Composer::getUncompressedFrame (uint32_t frame, ADMImage * out,
				      uint32_t * flagz)
{
  // first get segment
  uint32_t relframe, seg, flags, len;
  uint8_t ret = 0;
  EditorCache   *cache;
  ADMImage	*result;
//    static uint32_t lastlen=0;

	if(flagz)
			*flagz=0;

  // convert frame to block, relative frame
  if (!convFrame2Seg (frame, &seg, &relframe))
    {
      printf ("\n Conversion failed !\n");
      return 0;
    }
  uint32_t ref = _segments[seg]._reference;
  uint32_t llen = _videos[ref]._aviheader->getWidth ()
  				  * _videos[ref]._aviheader->getHeight ();
	llen = llen + (llen >> 1);
	_VIDEOS *vid=&_videos[ref];
	cache=_videos[ref]._videoCache;
	ADM_assert(cache);
	
	aprintf("Ed: Request for frame %lu seg %lu, old frame:%lu old seg:%lu\n",relframe,seg,_lastframe,_lastseg);
	
	// First look in the cache
	if((result=cache->getImage(relframe)))
	{
		aprintf(">>frame %lu is cached...\n",relframe);
		out->duplicate(result);
		if(flagz)
			*flagz=result->flags;
		return 1;
	}
	else
	{
		aprintf("frame %lu is not cached...\n",relframe);
	}
//
	
//	Prepare the destination...
	result=cache->getFreeImage();
  
	
	
	cache->dump();
  // now we got segment and frame
  //*************************
  // Is is a key frame ?
  //*************************
  _videos[ref]._aviheader->getFlags (relframe, &flags);

     if (flags & AVI_KEY_FRAME)
    {
    	aprintf("keyframe\n");
     	if(!decodeCache(relframe,ref, result))	
		{
			printf("Editor: Cannot key frame %lu\n",relframe);
			return 0;
		}
	  _lastseg = seg;
	  _lastframe = relframe;
	  if(flagz)
	  	*flagz=result->flags;
	  out->duplicate(result);
	  return (1);
    }
  
    //*************************
    // following frame ?
    // If it a  next b-frame we already have the forward reference set
    // if it is a first B-frame --> do it
    //
    //*************************
      if ((seg == _lastseg) && ((_lastframe + 1) == relframe))
	{
	aprintf("following frame\n");
	// B Frame ?
	if(_videos[ref]._aviheader->isReordered())
	{
		// The frame we seek is not in cache, so 
		if(!_videos[ref]._aviheader->getFlags (relframe, &flags))
		{
			printf("Editor : Getflags failed\n");
			return 0;
		}
		// if it is a b frame we decode all of them up to 
		// the next ip included and put all this in cache
		if(flags & AVI_B_FRAME)
		{
			// decode all of them up to the next I/P frame 
			uint32_t nextIp=relframe;
			
			while((flags&AVI_B_FRAME))
			{
				nextIp++;
				_videos[ref]._aviheader->getFlags (nextIp, &flags);
				
			}
			// Decode it
			if(!decodeCache(nextIp,ref, result))	
			{
				printf("Editor: Cannot read ip frame %lu\n",nextIp);
				return 0;
			}
			
			
			// And now go forward...
			uint32_t seeked=relframe;
			while(seeked<nextIp)
			{
				result=cache->getFreeImage();
				if(!decodeCache(seeked,ref, result))	
				{
					printf("Editor: Cannot read ip frame %lu\n",nextIp);
					return 0;
				}
				if(seeked==relframe)
				{
					 out->duplicate(result);
					 if(flagz) *flagz=result->flags;
				}
				seeked++;
			}	
			_lastframe=nextIp;
			_lastseg = seg;
			return 1;	
		}
		
	}
	// No b frame...
      	 if(!decodeCache(relframe,ref, result))			
			{
				printf("Editor: Cannot read ip frame %lu\n",relframe);
				return 0;
			}
	if(flagz)
		*flagz=result->flags;	
	out->duplicate(result);
	_lastframe=relframe;
	_lastseg = seg;
	return 1;	 
    }
  //*************************
  // completly async frame
  // rewind
  //*************************
  aprintf("async  frame, wanted : %lu last %lu (%lu - %lu seg)\n",relframe,_lastframe,seg,_lastseg);
  uint32_t rewind;
  uint32_t seekFlag=0;
  
  _videos[ref]._aviheader->getFlags (relframe, &seekFlag);
  
  flags = 0;
  rewind = relframe;
  ADM_assert(rewind); // the first frame should be a keyframe !
  while (!(flags & AVI_KEY_FRAME))
  {
  	rewind--;
  	_videos[ref]._aviheader->getFlags (rewind, &flags);	
   }
  // now forward
  // IP seen is the last P (or I) before the frame we seek
  for (uint32_t i = rewind; i <relframe; i++)
    {

      _videos[ref]._aviheader->getFlags (i, &flags);
      // Skip B frames, there can be a lot of them
      if((flags&AVI_B_FRAME)) continue;
     
       if(!decodeCache(i,ref, result))			
	{
		printf("Editor: Cannot read ip frame %lu\n",relframe);
		return 0;
	}
	result=cache->getFreeImage();      
      }
      
      // Time to decode our frame
      // if it is not a B, just decode it
      // it it is a B, the usual stuff
      if(!seekFlag)
      {
      		if(!decodeCache(relframe,ref, result))
			{
				printf("Editor: Cannot read ip frame %lu\n",relframe);
				return 0;
			}
		if(flagz)
			*flagz=result->flags;	
		out->duplicate(result);
		_lastframe=relframe;
		_lastseg=seg;
		return 1;	
      
      }
      // it is a b frame      
      // decode all of them up to the next I/P frame 
	uint32_t nextIp=relframe;
	flags=AVI_B_FRAME;	
	while((flags&AVI_B_FRAME))
	{
		nextIp++;
		_videos[ref]._aviheader->getFlags (nextIp, &flags);		
	}
	// Decode it
	if(!decodeCache(nextIp,ref, result))	
	{
		printf("Editor: Cannot read ip frame %lu\n",nextIp);
		return 0;
	}
			
			
	// And now go forward...
	uint32_t seeked=relframe;
	while(seeked<nextIp)
	{
		result=cache->getFreeImage();
		if(!decodeCache(seeked,ref, result))	
		{
			printf("Editor: Cannot read ip frame %lu\n",nextIp);
			return 0;
		}
		if(seeked==relframe)
		{
			 out->duplicate(result);
			 if(flagz) *flagz=result->flags;
		}
		seeked++;
	} 
	_lastframe=nextIp;
	_lastseg=seg;
  	return 1;
}


//________________________________________________________________
//	Read and decode the given frame into image cache entry
//
//
//________________________________________________________________
 //*****
uint8_t		ADM_Composer::decodeCache(uint32_t frame,uint32_t seg, ADMImage *image)
{
uint32_t len;
uint32_t flags;
float	 sum;
EditorCache *cache=_videos[seg]._videoCache;	
	 if (!_videos[seg]._aviheader->getFrameNoAlloc (frame,
						     compBuffer,
						     &len, &flags))
	{
	  printf ("\nEditor: last decoding failed.%ld)\n",   frame );
	  return 0;
	}
	ADM_assert(_imageBuffer);
	// Do pp, and use imageBuffer as intermediate buffer
	if (!_videos[seg].decoder->uncompress (compBuffer, _imageBuffer, len, &flags))
	    {
	      printf ("\nEditor: Last Decoding2 failed for frame %lu\n",frame);
	      return 0;
	    }
	// Quant ?
	if(!_imageBuffer->quant || !_imageBuffer->_qStride)
	{
		_imageBuffer->_Qp=2;
		image->duplicate(_imageBuffer);
		cache->updateFrameNum(image,frame);
		aprintf("EdCache: No quant avail\n");
		return 1;
	}
	// We got everything, let's go
	// 1 compute average quant
	for(uint32_t z=0;z<_imageBuffer->_qSize;z++)
			sum+=_imageBuffer->quant[z];
	sum+=(_imageBuffer->_qSize-1);
	sum*=2;
	sum/=_imageBuffer->_qSize;
	if(sum>31) sum=31;
	if(sum<1) sum=1;
		
	_imageBuffer->_Qp=(uint32_t)floor(sum);
	
	// Pp deactivated ?
	if(!_pp.postProcType || !_pp.postProcStrength)
	{
		image->duplicate(_imageBuffer);
		cache->updateFrameNum(image,frame);
		aprintf("EdCache: Postproc disabled\n");
		return 1;	
	}
	
	int type;	
	#warning FIXME should be FF_I_TYPE/B/P
	if(_imageBuffer->flags & AVI_KEY_FRAME) type=1;
		else if(_imageBuffer->flags & AVI_B_FRAME) type=3;
			else type=2;
	
	// we do postproc !
	// keep
	uint8_t *oBuff[3],*iBuff[3];
	int	strideTab[3];
	int	strideTab2[3];
	aviInfo _info;
		
		getVideoInfo(&_info);
		iBuff[0]= YPLANE((_imageBuffer));
		if(1)
		{
			iBuff[1]= UPLANE((_imageBuffer));
 			iBuff[2]= VPLANE((_imageBuffer));
		}
		else
		{
			iBuff[2]= UPLANE((_imageBuffer));
 			iBuff[1]= VPLANE((_imageBuffer));
		}
			oBuff[0]= YPLANE(image);
			oBuff[1]= UPLANE(image);
 			oBuff[2]= VPLANE(image);
        			
			strideTab[0]=strideTab2[0]=_info.width;
			strideTab[1]=strideTab2[1]=_info.width>>1;
			strideTab[2]=strideTab2[2]=_info.width>>1;
            			
		 pp_postprocess(
		 		iBuff,
		 		strideTab,
		 		oBuff,
		 		strideTab2,
		 		_info.width,
		        	_info.height,
		          	(int8_t *)(_imageBuffer->quant),
		          	_imageBuffer->_qStride,
		         	_pp.ppMode,
		          	_pp.ppContext,
		          	type);			// img type
				// update some infos
		image->copyInfo(_imageBuffer);
		cache->updateFrameNum(image,frame);
		aprintf("EdCache: Postproc done\n");
		return 1;	

}
uint8_t ADM_Composer::setPostProc( int type, int strength, int swapuv)
{
	if(!_nb_video) return 0;
	_pp.postProcType=type;
	_pp.postProcStrength=strength;
	updatePostProc(&_pp); // DeletePostproc/ini missing ?
	return 1;
}
uint8_t ADM_Composer::getPostProc( int *type, int *strength, int *swapuv)
{
	if(!_nb_video) return 0;
	*type=_pp.postProcType;
	*strength=_pp.postProcStrength;
	*swapuv=0;
	return 1;
}
//______________________________________________
//_______________________________________________
//
//      Render previous kf
//
uint8_t	ADM_Composer::getPKFrame(uint32_t *frame)
{
	uint32_t fr, seg, relframe;	//,len; //flags,ret,nf;

  fr = *frame;

  if (*frame == 0)
    {
      return 0;
    }
  if (!searchPreviousKeyFrame (fr, &seg, &relframe))
    {
      printf (" NKF not found\n");
      return 0;
    }
  ADM_assert (convSeg2Frame (frame, seg, relframe));
  return 1;
}
uint8_t
  ADM_Composer::getUncompressedFramePKF (uint32_t * frame, ADMImage * out)
{
  uint32_t fr, seg, relframe;	//,len; //flags,ret,nf;



  fr = *frame;

  if (*frame == 0)
    {
      return getUncompressedFrame (0, out);
    }


  if (!searchPreviousKeyFrame (fr, &seg, &relframe))
    {
      printf (" NKF not found\n");
      return 0;
    }
  ADM_assert (convSeg2Frame (frame, seg, relframe));

  return getUncompressedFrame (*frame, out);
}
uint8_t	ADM_Composer::getNKFrame(uint32_t *frame)
{
	uint32_t fr, seg, relframe;	//,len; //flags,ret,nf;

  fr = *frame;  
  if (!searchNextKeyFrame (fr, &seg, &relframe))
    {
      printf (" NKF not found\n");
      return 0;
    }
  ADM_assert (convSeg2Frame (frame, seg, relframe));
  return 1;
}

//
//      Render Next kf
//
uint8_t
  ADM_Composer::getUncompressedFrameNKF (uint32_t * frame, ADMImage * out)
{
  uint32_t fr, seg, relframe, nf;	//flags,ret,len;

  fr = *frame;

  if (!searchNextKeyFrame (fr, &seg, &relframe))
    {
      printf (" NKF not found\n");
      return 0;
    }


  ADM_assert (nf = convSeg2Frame (frame, seg, relframe));

  return getUncompressedFrame (*frame, out);

}
uint8_t	ADM_Composer::isReordered( uint32_t framenum )
{
uint32_t seg,relframe;
 if (!convFrame2Seg (framenum, &seg, &relframe))
    {
      printf ("\n Conversion failed !\n");
      return 0;
    }
    uint32_t ref=_segments[seg]._reference;
   return _videos[ref]._aviheader->isReordered();
}
