/***************************************************************************
                          ADM_edIdentify.cpp  -  description
                             -------------------
	Identify a file by reading its magic


    begin                : Thu Feb 28 2003
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
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_inpics/ADM_pics.h"
#include "ADM_nuv/ADM_nuv.h"
#include "ADM_h263/ADM_h263.h"
#include "ADM_3gp/ADM_3gp.h"
#include "ADM_mpeg2dec/ADM_mpegscan.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_assert.h"

/**
	Read the magic of a file i.e. its 16 first bytes
*/
uint8_t ADM_Composer::getMagic (char *name, uint32_t * magic)
{
  FILE *
    tmp;
  uint8_t
    ret =
    1;

  tmp = fopen (name, "rb");
  if (!tmp)
    return 0;

  if (4 != fread (magic, 4, 4, tmp))
    ret = 0;

  fclose (tmp);
  return ret;

}

/**
   		Tries to identify the incoming stream using its magic


*/

uint8_t ADM_Composer::identify (char *name, fileType * type)
{
  uint32_t magic[4];
  uint32_t id;
  *type = Unknown_FileType;
  if (!getMagic (name, magic))
    return 0;


  id=R32(magic[0]);
  printf("%x -> %x\n",id,magic[0]);
  
  
  // Vcodec file ?
  
 
  if (fourCC::check (id, (uint8_t *) "ADVC"))
    {
	printf (" \n video codec  file detected...");
	*type = VCodec_FileType;
	return 1;	
    }
    
  // now try to identifies the filetype by its magic
  if (fourCC::check (id, (uint8_t *) "RIFF"))
    {
      printf (" \n Riff file detected...");
      if (fourCC::check (R32(magic[2]), (uint8_t *) "AVI "))
	{
	  printf (" \n AVI file detected...\n");
	  *type = AVI_FileType;
	  return 1;
	}
      printf (" \n Unknown Riff...");
      return 0;
    }

  if (fourCC::check (id, (uint8_t *) "IDXM")
      || fourCC::check (id, (uint8_t *) "IDXI")
      )
    {
      printf (" \n DVD2AVI Mpeg file detected...\n");
      *type = MpegIdx_FileType;
      return 1;
    }

  if (fourCC::check (id, (uint8_t *) "TOC "))
    {
      printf (" \n Mpeg file detected...\n");
      *type = Mpeg_FileType;
      return 1;
    }

  if (				// fixme: put a mask
       (magic[0] == R32(0xba010000)) 
        || (magic[0] == R32(0xb3010000)) 
        || (magic[0] == R32(0xe0010000)) 
        || (id &0xff)==0x47
       
       )
    {
      printf (" \n Mpeg file detected...\n");
      *type = Mpeg_FileType;
      return 1;
    }
  if (fourCC::check (id, (uint8_t *) "Nupp")
      || fourCC::check (id, (uint8_t *) "Myth")
	|| fourCC::check (id, (uint8_t *) "ADNV")
      )
    {
      printf (" \n Nuppelvideo file detected...\n");
      *type = Nuppel_FileType;
      return 1;
    }
    uint32_t id2=id&0xFFFFFF;
    if (magic[0]==0x474e5089) 
    {

      printf (" \n PNG file detected...\n");
      *type = BMP_FileType;
      return 1;
    }
  if (magic[0] == R32(0x05364d42) ||
 		(magic[0] &0xffff)== R32(0xd8ff) 
		||
		magic[0]==R32(0x84364d42)
		||
		(id &0xffff)==0x4d42)
    {

      printf (" \n BMP file detected...\n");
      *type = BMP_FileType;
      return 1;
    }

  if (magic[0] == R32(0x02800000))
    {
      printf (" \n Raw H263 file detected...\n");
      *type = H263_FileType;
      return 1;
    }
  if (magic[0] == R32(0x00010000))
    {
      printf (" \n Raw mpeg4 file detected...\n");
      *type = Mp4_FileType;
      return 1;
    }
    if(fourCC::check(id,(uint8_t *)"ADMW"))
    {
 		printf (" \n Workbench file detected...\n");
     		 *type = WorkBench_FileType;
      		return 1;

    }
    if(fourCC::check(id,(uint8_t *)"OggS"))
    {
 		printf (" \n Ogg file detected..\n");
     		 *type = Ogg_FileType;
      		return 1;

    }
    if(fourCC::check(id,(uint8_t *)"#!AD"))
        {
                printf (" \n ADM script file detected...\n");
                *type=Script_FileType;
                return 1;
        }
    if(fourCC::check(id,(uint8_t *)"//AD"))
        {
                printf (" \n ADM ECMAScript/Javascript file detected..\n");
                *type=ECMAScript_FileType;
                return 1;
        }
        if(fourCC::check(id,(uint8_t *)"ADMX"))
        {
                printf (" \n New mpeg index file detected..\n");
                *type=NewMpeg_FileType;
                return 1;
        }
      //
      //        Warning, from here we change endianness
      //
	id=R32(magic[1]);
    if(fourCC::check(id,(uint8_t *)"ftyp") ||
    	fourCC::check(id,(uint8_t *)"pnot") ||
	fourCC::check(id,(uint8_t *)"mdat") ||
	fourCC::check(id,(uint8_t *)"moov") ||
        fourCC::check(id,(uint8_t *)"skip") 
	)
    {
 		printf (" \n 3GPP /MP4/Quicktime file detected..\n");
     		 *type = _3GPP_FileType;
      		return 1;

    }
      
  printf ("\n unrecognized file detected...\n");
  fourCC::print(magic[0]); printf("\n");
  fourCC::print(magic[1]);
  return 0;
}
