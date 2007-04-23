/***************************************************************************
    copyright            : (C) 2006 by mean
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "math.h"

#include "default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_mkv.h"

#include "mkv_tags.h"

typedef struct MKVCC
{
    const char *name;
    uint32_t isVideo;
    uint32_t audioCC;
    const char *videoCC;
};

MKVCC mkvCC[]=
{
  {"A_MPEG/L3",0,WAV_MP3,""},
  
  // Video
  {"V_MPEG4/ISO/AVC",1,0,"AVC1"}, //H264
  {"V_MS/VFW/FOURCC",1,0,"DIV3"}, // Divx 2.
  
  
  // Filler
  {"AVIDEMUX_RULES",1,0,"DIV2"} // DUMMY
};

uint32_t ADM_mkvCodecToFourcc(const char *codec)
{
int nbEntry=sizeof(mkvCC)/sizeof(MKVCC);
    for(int i=0;i<nbEntry;i++)
    {
      MKVCC *cur=&(mkvCC[i]);
      if(!strcmp(cur->name,codec))
      {
         if(cur->isVideo) return fourCC::get((uint8_t *)cur->videoCC);
               else return cur->audioCC;
      }
    }
    printf("[MKV] Warning type <%s> unkown!!\n",codec);
    return 0;
}
  //EOF