//
//      chroma noise reduction II (version 2.6.1) - Avisynth filter reducing noise on chroma
//      Copyright (C) 2002 Marc Fauconneau
//
//      Inspired by :
//  chroma noise reduction (version 1.1) - VirtualDub filter reducing noise on chroma
//  Copyright (C) 2000 Gilles Mouchard
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  Please contact me for any bugs or questions.
//  marc.fd@libertysurf.fr
//
//  Change log :
//         30/06/2004 - ver 2.6.1 - YUY2 opts as I had slowed it down - tritical 
//         29/06/2004 - ver 2.6  - Some bug fixes and some code cleanup 
//                                 and rewriting (tritical - kes25c@mizzou.edu)
//         18/11/2003 - ver 2.51 - Further bug fixes (by Klaus Post)
//         13/11/2003 - ver 2.5  - Bug fixes (by Klaus Post)
//         15/12/2002 - ver 2.4  - Bug fixes
//         13/11/2002 - ver 2.3  - YV12 mode, scd (scenechange detection)
//         01/08/2002 - ver 2.2  - Ugly bug fixed
//         31/07/2002 - ver 2.1  - Bug Fixes (thx to dividee ;)
//         20/07/2002 - ver 2.0  - Avisynth filter coded (from scratch)


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
#include "ADM_video/ADM_cache.h"
#include "ADM_filter/video_filters.h"

#include "ADM_vidCNR2_param.h"

//#define SUBST 1

class vidDelta:public AVDMGenericVideoStream
{

protected:
  virtual char *printConf (void);
  VideoCache *vidCache;


public:

                        vidDelta (AVDMGenericVideoStream * in, CONFcouple * setup);
        virtual         ~vidDelta ();
  virtual uint8_t getFrameNumberNoAlloc (uint32_t frame, uint32_t * len,
					 ADMImage * data, uint32_t * flags);
  uint8_t configure (AVDMGenericVideoStream * instream);
  virtual uint8_t getCoupledConf (CONFcouple ** couples);

};

static FILTER_PARAM delta_template =
  { 0 };

BUILD_CREATE (delta_create, vidDelta);
SCRIPT_CREATE (delta_script, vidDelta, delta_template);


/*************************************/
uint8_t vidDelta::configure (AVDMGenericVideoStream * in)
{
        return 0;
}
/*************************************/
char *vidDelta::printConf (void)
{
  static char buf[50];
  sprintf ((char *) buf, " Delta");
  return buf;
}
/*************************************/
vidDelta::vidDelta (AVDMGenericVideoStream * in, CONFcouple * couples)
{

  _in = in;
  memcpy (&_info, _in->getInfo (), sizeof (_info));
  _info.encoding = 1;
  vidCache = new VideoCache (4, in);
  
  
}
//____________________________________________________________________
vidDelta::~vidDelta ()
{

  delete vidCache;
  vidCache = NULL;

}

//______________________________________________________________
uint8_t vidDelta::getFrameNumberNoAlloc (uint32_t frame,
				uint32_t * len,
				ADMImage * data, uint32_t * flags)
{
int w,h,page;
ADMImage *cur,*prev,*next;
        w=_info.width;
        h=_info.height;
        page=w*h;
  if(!frame || frame>=_info.nb_frames-1)
  {
        memset(YPLANE(data),0,page);
        memset(UPLANE(data),128,page>>2);
        memset(VPLANE(data),128,page>>2);
        return 1;
  }
  cur=vidCache->getImage (frame);
  prev=vidCache->getImage (frame-1);
  next=vidCache->getImage (frame+1);
  // No chroma
        memset(UPLANE(data),128,page>>2);
        memset(VPLANE(data),128,page>>2);

   uint8_t *c=YPLANE(cur);
   uint8_t *p=YPLANE(prev);
   uint8_t *n=YPLANE(next);
   uint8_t *out=YPLANE(data);
#define SAME_THRESH 32
   int a,b,d,cp,cn,e;
   for(int y=0;y<h;y++)
        for(int x=0;x<w;x++)
        {
                a=*c++;
                b=*p++;
                e=*n++;
                cp=abs(a-b);
                cn=abs(a-e);
                // Cp is the != between current and previous
                // cn is the != between current and next
                // We try to spo frames that are blended betwwen next & previous
                
                if(cp>SAME_THRESH && cn > SAME_THRESH && abs(cp-cn) < 128) *out++=255;//(cp+cn)/2;
                        else *out++=0;
        }


  vidCache->unlockAll ();
  return 1;
}

/*************************************/
uint8_t vidDelta::getCoupledConf (CONFcouple ** couples)
{

  *couples=NULL;
  return 1;
}

// EOF
