/***************************************************************************
             Fake encoder that is just a pass through from the
                video body frame.
        
             A couple of gotcha :
                - the first frame is not frame 0 but startFrame
                - we have to reorder the frame in DTS order TODO FIXME
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


#define aprintf printf

EncoderCopy::EncoderCopy (COMPRES_PARAMS *codecconfig )
{
        _frameStart=_total=0;
         
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



   const char *EncoderCopy::getCodecName(void ) 
   {
        return "Copy";
   }
   const char *EncoderCopy::getFCCHandler(void )
   {
                return "Copy";
   }
   
   uint8_t     EncoderCopy::getLastQz( void)
   {
                return 31;
   }

   uint8_t EncoderCopy::configure (AVDMGenericVideoStream * instream)
   {
                _in=instream;
                _frameStart=frameStart;
                _total=frameEnd-frameStart;
                return 1;
   }
   uint8_t EncoderCopy::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags)
   {
                uint8_t ret=0;
                ADM_assert(frame<_total);
                ret=video_body->getFrameNoAlloc (_frameStart + frame, out, len,flags);
                return ret;
   }

 uint8_t EncoderCopy::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{

        return  video_body->getExtraHeaderData(l,data);
}



