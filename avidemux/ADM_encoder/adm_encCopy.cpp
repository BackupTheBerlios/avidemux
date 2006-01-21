/***************************************************************************
             Fake encoder that is just a pass through from the
                video body frame.
        
             A couple of gotcha :

        
                - the first frame is not frame 0 but startFrame, remove lagging B 
                - we have to reorder the frame in DTS order 
                - we have to present the fourCCs of video_body
    
    copyright            : (C) 2002/2006 by mean
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
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>


#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encCopy.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ENCODER
#include "ADM_toolkit/ADM_debug.h"


static uint32_t searchForward(uint32_t startframe);
EncoderCopy::EncoderCopy (COMPRES_PARAMS *codecconfig )
{
        _frameStart=_total=0;
        _lastIPFrameSent=0xFFFFFFFF;
        
         
}
EncoderCopy::~EncoderCopy() 
{

}
   uint8_t EncoderCopy::isDualPass (void) { return 0;}
   uint8_t EncoderCopy::setLogFile (const char *p, uint32_t fr) {return 1;}
   uint8_t EncoderCopy::stop (void) {return 1;}
   uint8_t EncoderCopy::startPass2 (void) {return 1;}
   uint8_t EncoderCopy::startPass1 (void) {return 1;}
const char *EncoderCopy::getDisplayName(void ) {return "Copy";}


// that one is used as fourcc
   const char *EncoderCopy::getCodecName(void ) 
   {
aviInfo info;
video_body->getVideoInfo(&info);
        return fourCC::tostring(info.fcc );
   }
   const char *EncoderCopy::getFCCHandler(void )
   {
                return "Copy";
   }
   
   uint8_t     EncoderCopy::getLastQz( void)
   {
                return 31;
   }
/****************************************************************/
uint8_t EncoderCopy::configure (AVDMGenericVideoStream * instream)
{
aviInfo info;
                _in=instream;
                _frameStart=frameStart;
                video_body->getVideoInfo(&info);
                if(info.nb_frames==frameEnd+1) // last frame included ?
                {
                        _total=_total=frameEnd-frameStart+1;
                }
                else
                        _total=frameEnd-frameStart;
                
                uint32_t end,flags;
next:
                end=frameStart+_total-1;
                video_body->getFlags(end,&flags);
                if((flags & AVI_B_FRAME)&&_total)
                {
                        _total--;
                        goto next;
                }

                return 1;
}
/****************************************************************/
uint8_t EncoderCopy::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags)
{
        uint8_t ret=0;

        if(frame>=_total)
        {
                printf("EncCopy: Going out of bound %d/%d\n",frame,_total);
                return 0;
        }
        // No B frames, take as is
        if(!video_body->isReordered(frameStart+frame))
        {
               ret=video_body->getFrameNoAlloc (_frameStart + frame, out, len,flags);
               return ret;
        }
        // it has PTS/DTS stuff so we need to reorder it

        video_body->getFlags(frameStart + frame, flags);
        if(*flags & AVI_B_FRAME)
        {       // search if we have to send a I/P frame in adance
                  aprintf("\tIt is a B frame\n");
                  uint32_t forward;

                        forward=searchForward(_frameStart+frame);
                        // if we did not sent it, do it now
                        if(forward!=_lastIPFrameSent)
                        {
                                        aprintf("\tP Frame not sent, sending it :%lu\n",forward);
                                        ret = video_body->getFrameNoAlloc (forward, out, len,flags);
                                        _lastIPFrameSent=forward;

                                }
                                else
                                {
                                        // we already sent it :)
                                        // send n-1
                                        aprintf("\tP Frame already, sending  :%lu\n",frameStart+frame-1);
                                        ret = video_body->getFrameNoAlloc (_frameStart+frame-1,out, len,flags);

                                }

                        }
                        else // it is not a B frame and we have nothing on hold, sent it..
                        {
                                // send n-1 if we reach the fwd reference frame
                                if((frame+frameStart)==_lastIPFrameSent)
                                {
                                        aprintf("\tSending Last B-frame :(%lu)\n",_frameStart + frame-1);
                                        ret= video_body->getFrameNoAlloc (_frameStart + frame-1,out, len,flags);

                                }
                                else
                                {
                                        aprintf("\tJust sending it :(%lu)-(%lu)\n",_frameStart + frame,_lastIPFrameSent);
                                        ret= video_body->getFrameNoAlloc (_frameStart + frame, out, len,flags);

                                }
                        }
        if(!ret) printf("Get frame error for frame %d+%d\n",frameStart,frame);
        return ret;
}

 uint8_t EncoderCopy::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{

        return  video_body->getExtraHeaderData(l,data);
}

uint32_t searchForward(uint32_t startframe)
{
                uint32_t fw=startframe;
                uint32_t flags;
                uint8_t r;

                        while(1)
                        {
                                fw++;
                                r=video_body->getFlags (fw, &flags);
                                if(!(flags & AVI_B_FRAME))
                                {
                                        return fw;

                                }
                                ADM_assert(r);
                                if(!r)
                                {
                                        printf("\n Could not locate last non B frame \n");
                                        return 0;
                                }

                        }
}

