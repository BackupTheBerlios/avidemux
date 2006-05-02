
/***************************************************************************
    (c)Mean 2006 fixounet@free.fr
 ***************************************************************************/
 
 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"
#include "math.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"
#include "ADM_filter/video_filters.h"
#include"ADM_video/ADM_vidAnimated.h"

static FILTER_PARAM animated_template={8,
    {"tc0","tc1","tc2",
    "tc3","tc4","tc5",
    "isNTSC","backgroundImg"
    }};
BUILD_CREATE(animated_create,ADMVideoAnimated);
SCRIPT_CREATE(animated_script,ADMVideoAnimated,animated_template);

uint8_t ADMVideoAnimated::configure(AVDMGenericVideoStream *in)
{

  
   return 0;
}

char *ADMVideoAnimated::printConf( void )
{
   static char buf[50];
   sprintf((char *)buf," Animated Menu ");
   return buf;
}

ADMVideoAnimated::ADMVideoAnimated(AVDMGenericVideoStream *in,CONFcouple *couples) 
{
   _in=in;
   memcpy(&_info,_in->getInfo(),sizeof(_info));    
   _info.encoding=1;
   _uncompressed=NULL;
   _param=NEW(ANIMATED_PARAM);
   if(couples)
   {
        GET(isNTSC);
        GET(backgroundImg);
#undef GET
#define GET(x)  couples->getCouple((char *)"tc"#x,&(_param->timecode[x]))
        GET(0);
        GET(1);
        GET(2);
        GET(3);
        GET(4);
        GET(5);
   }
   else // Default
   {
#define MKP(x,y) _param->x=y;
            MKP(isNTSC,0);
            MKP(backgroundImg,NULL);
#undef MKP(x,y)
#define MKP(x,y) _param->timecode[x]=y
        MKP(0,0);
        MKP(1,100);
        MKP(2,200);
        MKP(3,300);
        MKP(4,500);
        MKP(5,600);
   }
   if(_param->isNTSC) _info.height=480;
        else _info.height=576;
    _info.width=720;
}
//____________________________________________________________________
ADMVideoAnimated::~ADMVideoAnimated()
{
   delete _param;
   _param=NULL;
   _uncompressed=NULL;
}
#define BYTE uint8_t 
#ifndef MAX
#define MAX(x,y) ((x)>(y) ?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ?(x):(y))
#endif
//______________________________________________________________
uint8_t ADMVideoAnimated::getFrameNumberNoAlloc(uint32_t frame,
  uint32_t *len,
  ADMImage *data,
  uint32_t *flags)
  {
     

	return 1;

}


uint8_t	ADMVideoAnimated::getCoupledConf( CONFcouple **couples)
{
   
      ADM_assert(_param);
      *couples=new CONFcouple(19);
#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
       CSET(isNTSC);
       CSET(backgroundImg);
#undef CSET
#define CSET(x)  (*couples)->setCouple((char *)"tc"#x,(_param->timecode[x]))
        CSET(0);
        CSET(1);
        CSET(2);
        CSET(3);
        CSET(4);
        CSET(5);

      return 1;
}



// EOF
