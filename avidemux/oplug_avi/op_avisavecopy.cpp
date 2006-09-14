/***************************************************************************
                          op_avisavecopy.cpp  -  description
                             -------------------

	We bypass the use of _incoming to have easy access to furure
	frame.
	In fact only the getflags is necessary, other stuff will be done throught
	incoming.

    begin                : Fri May 3 2002
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

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_SAVE_AVI
#include "ADM_toolkit/ADM_debug.h"


static void updateUserData(uint8_t *start, uint32_t len);
uint8_t ADM_findMpegStartCode(uint8_t *start, uint8_t *end,uint8_t *outstartcode,uint32_t *offset);
uint8_t GenericAviSaveCopy::setupVideo (char *name)
{
  //  Setup avi file output, all is coming from original avi
  // since we are inc copy mode
  memcpy(&_bih,video_body->getBIH (),sizeof(_bih));
  _bih.biSize=sizeof(_bih);  //fix old version of avidemux
  _bih.biXPelsPerMeter=_bih.biClrUsed=_bih.biYPelsPerMeter=0;
  //
  memcpy(&_videostreamheader,video_body->getVideoStreamHeader (),sizeof( _videostreamheader));
  memcpy(&_mainaviheader,video_body->getMainHeader (),sizeof(_mainaviheader));
  
  // Change both to divx/DX50
  if(_needUserDataUpdate)
  {
  	_videostreamheader.fccHandler=fourCC::get((uint8_t *)"divx");
	_bih.biCompression=fourCC::get((uint8_t *)"DX50");
  
  }
  /* update to fix earlier bug */
   _mainaviheader.dwWidth=_bih.biWidth;
   _mainaviheader.dwHeight=_bih.biHeight;

   uint8_t *extraData;
   uint32_t extraLen;
  _lastIPFrameSent=0xfffffff;
   video_body->getExtraHeaderData(&extraLen,&extraData);

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
  return 1;
}

//
//      Just to keep gcc happy....
//
// GenericAviSaveCopy::~GenericAviSaveCopy ()
// {
// 
// }

// copy mode
// Basically ask a video frame and send it to writter
// If it contains b frame and frames have been re-ordered
// reorder them back ...
uint8_t
GenericAviSaveCopy::writeVideoChunk (uint32_t frame)
{
  uint32_t    len;
  uint8_t    ret1;

	if(!video_body->isReordered(frameStart+frame))
	{
		ret1 = video_body->getFrameNoAlloc (frameStart + frame, vbuffer, &len,
				      &_videoFlag);
	}
	else
	{
			// we are async...
			video_body->getFlags (frameStart + frame, &_videoFlag);
			if(_videoFlag & AVI_B_FRAME)
			{ 	// search if we have to send a I/P frame in adance
				aprintf("\tIt is a B frame\n");
				uint32_t forward;
				forward=searchForward(frameStart+frame);
				// if we did not sent it, do it now
				if(forward!=_lastIPFrameSent)
				{
					aprintf("\tP Frame not sent, sending it :%lu\n",forward);
					ret1 = video_body->getFrameNoAlloc (forward, vbuffer, &len,
				      		&_videoFlag);
					_lastIPFrameSent=forward;

				}
				else
				{
					// we already sent it :)
					// send n-1
					aprintf("\tP Frame already, sending  :%lu\n",frameStart+frame-1);
					ret1 = video_body->getFrameNoAlloc (frameStart+frame-1, vbuffer, &len,
				      	&_videoFlag);

				}

			}
			else // it is not a B frame and we have nothing on hold, sent it..
			{
				// send n-1 if we reach the fwd reference frame
				if((frame+frameStart)==_lastIPFrameSent)
				{
					aprintf("\tSending Last B-frame :(%lu)\n",frameStart + frame-1);
					ret1 = video_body->getFrameNoAlloc (frameStart + frame-1, vbuffer, &len,
				      		&_videoFlag);

				}
				else
				{
					aprintf("\tJust sending it :(%lu)-(%lu)\n",frameStart + frame,_lastIPFrameSent);
					ret1 = video_body->getFrameNoAlloc (frameStart + frame, vbuffer, &len,
				      		&_videoFlag);

				}
			}

	}

  if (!ret1)
    return 0;

    // check for split
     // check for auto split
      // if so, we re-write the last I frame
      if(muxSize)
      	{
				 		// we overshot the limit and it is a key frame
				   	// start a new chunk
				  	if(handleMuxSize() && (_videoFlag & AVI_KEY_FRAME))
				   	{		
							  uint8_t *extraData;
							  uint32_t extraLen;

   							video_body->getExtraHeaderData(&extraLen,&extraData);
					   
							if(!reigniteChunk(extraLen,extraData)) return 0;
						}
				 }
//  encoding_gui->feedFrame(len);  
  if(_needUserDataUpdate)
  	updateUserData(vbuffer,len);
  return writter->saveVideoFrame (len, _videoFlag, vbuffer);

}
//_____________________________________________________
// Update the user data field that is used to 
// detect in windows world if it is packed or not
//_____________________________________________________
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
		if(!strncmp((char *)start,"DivX",4))
		{

    		    //
    		    start+=4;
    		    rlen-=4; // skip "DivX"
				// looks for a p while not null
				// if there isnt we will reach a new startcode
				// and it will stop
				while((*start!='p') && rlen) 
				{
					if(!*start)
					{
						rlen=0;
						break;
					}
					rlen--;
					start++;
				}
				if(!rlen) 
					{
						printf("Unpacketizer:packed marker not found!\n");
					}
				else	
				        *start='n'; // remove 'p'
				return;			
		}
	}
}
//EOF