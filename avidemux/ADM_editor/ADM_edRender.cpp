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
#include <assert.h>

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

// FIXME BADLY !!!
static uint8_t compBuffer[MAXIMUM_SIZE * MAXIMUM_SIZE * 3];
static uint8_t prepBuffer[MAXIMUM_SIZE * MAXIMUM_SIZE * 3];

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
/*
		Main function
		Caller ask for a frame from the editor
		We first split it to segment and frame from that segment
			then look if it is in the cache
		If not and if there is Bframe we look if it is the forward
			reference frame we currently have

*/
uint8_t  ADM_Composer::getUncompressedFrame (uint32_t frame, uint8_t * out,
				      uint32_t * flagz)
{
  // first get segment
  uint32_t relframe, seg, flags, len;
  uint8_t ret = 0;
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


//*************************
  // cached ?
  //
  //*************************

  if (_cached == 1)
  {
   // case of forward ref frame
    if (_videos[ref]._reorderReady) // if not reordered do not bother
    {
#if 0    
    	if(_videos[ref]._forwardReference==relframe)
    		{
			// gotcha
			aprintf("Cached as forward frame.\n");
			memcpy(out,vid->_forwardFrame, llen);
			if(flagz)
				*flagz=vid->_forwardFlag;
			_lastseg = seg;
	  		_lastframe = relframe;				
			return 1;
		}
#endif		
    }
    if((_lastseg == seg) && (_lastframe == relframe))
    {
      aprintf("Cached read (%lu) inst:%lx\n",_lastframe,this);
      memcpy (out, prepBuffer, llen);
      return 1;
    }
   
    aprintf("Cache miss : wanted seg:%lu frame:%lu , in cache seg:%lu frame:%lu \n",
					seg,relframe, _lastseg,_lastframe);
  }  
  else
  {
	aprintf("Cache not activated\n");
  }

  // now we got segment and frame
  //*************************
  // Is is a key frame ?
  //*************************
  _videos[ref]._aviheader->getFlags (relframe, &flags);

  if (flags & AVI_KEY_FRAME)
    {
    aprintf("keyframe\n");
      // yes

   	if (vid->_reorderReady)
    		if(vid->_forwardReference==relframe)
    		{ 
    		// if we got it allocated ...
			aprintf("It is forward ref..\n");
			if(vid->_forwardFrame)
			{
				memcpy(out,vid->_forwardFrame, llen);
				if(_cached==1)
					memcpy(prepBuffer,vid->_forwardFrame, llen);
				if(flagz)
					*flagz=vid->_forwardFlag;
  				_lastseg = seg;
	  			_lastframe = relframe;
				aprintf(" using forward ref..\n");
				return 1;
			}
    		}
      if (_videos[ref]._aviheader->needDecompress ())
	{
	  if (!_videos[ref]._aviheader->getFrameNoAlloc (relframe,
							 compBuffer,
							 &len, &flags))
	    return 0;
	  _lastseg = seg;
	  _lastframe = relframe;

	  // decode
	  ret =  _videos[ref].decoder->uncompress (compBuffer, out, len, &flags);
	  if(flagz)
	  	*flagz=flags;
	  if ((_cached == 1) && ret)
	    {
	      memcpy (prepBuffer, out, llen);
	    }
	  return (ret);
	}
      else
	{			// does not need decompress (for example mpeg stream)
	  if (!_videos[ref]._aviheader->getFrameNoAlloc (relframe,
							 out, &len, &flags))
	    return 0;
	  _lastseg = seg;
	  _lastframe = relframe;

	  if ((_cached == 1))
	    {
	      memcpy (prepBuffer, out, llen);
	    }
	  return 1;

	}
    }
  else
    {
    //*************************
    // following frame ?
    // If it a  next b-frame we already have the forward reference set
    // if it is a first B-frame --> do it
    //
    //*************************
      if ((seg == _lastseg) && ((_lastframe + 1) == relframe))
	{
	aprintf("following frame\n");
	  _lastframe++;
	//*************************
    	// forward ref ?
    	//*************************
	    if (_videos[ref]._reorderReady)
    		if(_videos[ref]._forwardReference==relframe)
    		{
    		// if we got it allocated ...
			aprintf("Not kf, It is forward ref..\n");
			if(_videos[ref]._forwardFrame)
			{
				memcpy(out,_videos[ref]._forwardFrame, llen);
				if(flagz)
					*flagz=vid->_forwardFlag;
				aprintf(" using forward ref..\n");
				if(_cached==1)
					memcpy(prepBuffer,vid->_forwardFrame, llen);	
				return 1;
			}
    		}

	  if (_videos[ref]._aviheader->needDecompress ())
	    {
	      _videos[ref]._aviheader->getFlags(relframe,&flags);
	         // and this video is B-frame friendly..
      		if(_videos[ref]._reorderReady)
	        if(flags & AVI_B_FRAME)
    		  {
			// we are at the right frame and it is a B-frame
			// decompress the next P/I frame, store it in cache
			// then decompress the current one
			if(0==setForward(ref,relframe))
			{
				printf("\n error setting fw frame \n");
			}
    		  }
	      if (!_videos[ref]._aviheader->getFrameNoAlloc (relframe,
							     compBuffer,
							     &len, &flags))
		return 0;

	      ret =	_videos[ref].decoder->uncompress (compBuffer, out, len,
						  &flags);
		if(flagz)
				*flagz=flags;
	    }
	  else
	    {
	      ret = _videos[ref]._aviheader->getFrameNoAlloc (relframe,
							      out,
							      &len, &flags);
		if(flagz)
				*flagz=flags;
	    }
	  if ((_cached == 1))
	    {
	      memcpy (prepBuffer, out, llen);
	    }
	  return (ret);

	}
    }
    //*************************
  // completly async frame
  // rewind
  //*************************
  aprintf("async  frame\n");
  uint32_t rewind;
  flags = 0;
  ref = _segments[seg]._reference;
  // We clear the forward frame as it need to be stored properly later
  _videos[ref]._forwardReference=0xffffffff;
  rewind = relframe;
  assert(rewind); // the first frame should be a keyframe !
  	while (!(flags & AVI_KEY_FRAME))
    	{
      		rewind--;
      		_videos[ref]._aviheader->getFlags (rewind, &flags);
    	}
  // now forward
  for (uint32_t i = rewind; i <= relframe; i++)
    {

      _videos[ref]._aviheader->getFlags (i, &flags);
      // do not bother decoding B frame
      if((flags & AVI_B_FRAME) && (i!=relframe)) continue;

      // if we want a B frame
      // and this video is B-frame friendly..
      if(_videos[ref]._reorderReady)
      if(flags & AVI_B_FRAME &&(i==relframe))
      {
		// we are at the right frame and it is a B-frame
		// decompress the next P/I frame, store it in cache
		// then decompress the current one
		if(0==setForward(ref,relframe))
		{
			printf("\n error setting fw frame \n");
		}
      }

	// then just
      if (!_videos[ref]._aviheader->getFrameNoAlloc (i,
						     compBuffer,
						     &len, &flags))
	{
	  printf ("\n rewind failed.%ld -> %ld..(%ld)\n", rewind, relframe,
		  i);
	  return 0;
	}
      if (_videos[ref]._aviheader->needDecompress ())
	{
	  if (!_videos[ref].decoder->uncompress (compBuffer, out, len, &flags))
	    {
	      return 0;
	    }
	    if(flagz)
	    	*flagz=flags;
	}
      else
	{
	  memcpy (out, compBuffer, len);
	}
    }
  if (_cached == 1)
    memcpy (prepBuffer, out, llen);

  _lastseg = seg;
  _lastframe = relframe;

  return 1;
}

//
//	Set the forward reference frame from the current frame
//
uint8_t ADM_Composer::setForward(uint32_t ref, uint32_t frame)
{
		uint32_t forw=frame;
		uint32_t len;
		uint32_t max=0,flags=0;;

		_VIDEOS *vid;

		vid=&_videos[ref];
		max=vid->_nb_video_frames;

		aprintf("\n Forwarding from frame :%lu \n",frame);

		if(!vid->_reorderReady)
		{
			printf("\n Reorder not available\n");
			return 0;
		}
		do
		{	 forw++;
   			vid->_aviheader->getFlags (forw, &flags);
		}while((flags & AVI_B_FRAME) && (forw < max));
		aprintf("\n Next i/p is  from frame :%lu \n",forw);
		if(forw == max)
		{
			printf("\n could not find the forward ref for frame %lu \n",ref);
			return 0;
		}

		if(!vid->_forwardFrame)
		{
			printf("\n Looking for forward frame for  %lu, but there is no allocated buffer ? \n",ref);
			return 0;
		}
		// already got it ?
		if(forw==vid->_forwardReference)
		{
			aprintf("\n re-using old one\n");
			 return 1;
		}
		// get forward frame
  		if (!vid->_aviheader->getFrameNoAlloc (
				forw,
				compBuffer,
				 &len, &vid->_forwardFlag))
				{
				    		printf("\n Reading error\n");
					     	return 0;
				}
		// and unpack it to the forward buffer
				if(!vid->decoder->uncompress ((uint8_t *)compBuffer,
						vid->_forwardFrame,
						len,
						&vid->_forwardFlag))
					{
						printf("\n Decompressing error\n");
						return 0;
					}
			// remember if have it
					vid->_forwardReference=forw;
			return 1;
}
//
//      Render previous kf
//
uint8_t
  ADM_Composer::getUncompressedFramePKF (uint32_t * frame, uint8_t * out)
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
  assert (convSeg2Frame (frame, seg, relframe));

  return getUncompressedFrame (*frame, out);
}

//
//      Render Next kf
//
uint8_t
  ADM_Composer::getUncompressedFrameNKF (uint32_t * frame, uint8_t * out)
{
  uint32_t fr, seg, relframe, nf;	//flags,ret,len;

  fr = *frame;

  if (!searchNextKeyFrame (fr, &seg, &relframe))
    {
      printf (" NKF not found\n");
      return 0;
    }


  assert (nf = convSeg2Frame (frame, seg, relframe));

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
