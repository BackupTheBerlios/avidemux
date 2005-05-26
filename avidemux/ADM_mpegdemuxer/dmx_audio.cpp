/***************************************************************************
                          dmx_audio
                          Audio mpeg demuxer
                             -------------------
    
    copyright            : (C) 2005 by mean
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
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include <ADM_assert.h>
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG_DEMUX
#include "ADM_toolkit/ADM_debug.h"

#include "dmx_audio.h"
//___________________________________________________                                                                                                                           
//___________________________________________________                                                                                                                           

//___________________________________________________                                                                                                                           
//___________________________________________________                                                                                                                           
dmxAudioStream::~dmxAudioStream ()
{
  if (index)
    delete[]index;
  if (_wavheader)
    delete _wavheader;
  if (demuxer)
    delete demuxer;

  index = NULL;
  _wavheader = NULL;
  demuxer = NULL;

}
#define MAX_LINE 4096
dmxAudioStream::dmxAudioStream (void)
{
  _wavheader = NULL;
  index = NULL;
  demuxer = NULL;
  nbIndex = 0;
}
uint8_t
dmxAudioStream::open (char *name)
{
  FILE *file;
  uint32_t dummy;		//,audio=0;
  char string[MAX_LINE];	//,str[1024];;
  uint32_t w = 720, h = 576, fps = 0;
  uint8_t type, progressif;
  char realname[1024];
  uint32_t aPid, vPid, aPesPid;
  uint32_t nbGop, nbFrame;


 file=fopen(name,"rt");
 if(!file) return 0;

  printf ("\n  opening dmx file for audio track : %s\n", name);
  fgets (string, MAX_LINE, file);	// File header
  if (strncmp (string, "ADMX", 4))
    {
      fclose (file);
      printf ("This is not a mpeg index G2\n");
      ADM_assert (0);
    }


  fgets (string, MAX_LINE, file);
  sscanf (string, "Type     : %c\n", &type);	// ES for now


  fgets (string, MAX_LINE, file);
  sscanf (string, "File     : %s\n", realname);

  fgets (string, MAX_LINE, file);
  sscanf (string, "Image    : %c\n", &progressif);	// Progressive

  fgets (string, MAX_LINE, file);
  sscanf (string, "Picture  : %04lu x %04lu %05lu fps\n", &w, &h, &fps);	// width...

  fgets (string, MAX_LINE, file);
  sscanf (string, "Nb Gop   : %05lu \n", &nbGop);	// width...

  fgets (string, MAX_LINE, file);
  sscanf (string, "Nb Images: %05lu \n", &nbFrame);	// width...

  fgets (string, MAX_LINE, file);
  //fscanf(string,"Nb Audio : %02lu\n",0); 

  fgets (string, MAX_LINE, file);
  sscanf (string, "Streams  : V%X:%X A%X:%X\n", &vPid, &dummy, &aPid,
	  &aPesPid);



  switch (type)
    {
    case 'P':
      demuxer = new dmx_demuxerPS (aPesPid, 0);
      break;
    default:
      ADM_assert (0);
    }

  // Now build the index
  nbIndex = nbGop;
  index = new dmxAudioIndex[nbGop + 2];
  printf ("Building index with %lu sync points\n", nbGop);

  uint32_t read = 0, img, count;
  uint64_t abs;

  while (read < nbGop)
    {
      if (!fgets (string, MAX_LINE, file))
	break;
      if (string[0] != 'A')
	continue;
      sscanf (string, "A %lu %llx %llu\n", &img, &abs, &count);
      index[read].img = img;
      index[read].start = abs;
      index[read].count = count;
      read++;
    }
  fclose (file);
  nbIndex = read;
  if (!read)
    {
      printf ("No audio at all!\n");
      return 0;
    }
  // now fill in the header
  _length = index[nbIndex - 1].count;
  _wavheader = new WAVHeader;
  memset (_wavheader, 0, sizeof (WAVHeader));

  // put some default value
  _wavheader->bitspersample = 16;
  _wavheader->blockalign = 4;
  _wavheader->byterate = 224000 / 8;
  _wavheader->channels = 2;

  _destroyable = 1;
  strcpy (_name, "dmx audio");
  demuxer->setPos (index[0].start, 0);
        //********************************
        // Should auto probe stream here
        //********************************
  _wavheader->encoding = WAV_MP2;

  _wavheader->frequency = 44100;
  _wavheader->byterate = 192000 / 8;

  demuxer->setPos (index[0].start, 0);
  _pos = 0;
  printf ("\n Mpeg audio initialized (%lu bytes)\n", _length);
  return 1;
}
// __________________________________________________________
// __________________________________________________________

uint8_t
dmxAudioStream::goTo (uint32_t offset)
{
uint8_t dummy[1024];
uint32_t left,right;
        // Search into the index to take the neareast one
        if(offset>=_length) return 0;
        for(uint32_t i=0;i<nbIndex-1;i++)
        {
                if(index[i].count>=offset && index[i+1].count<offset)
                {
                        demuxer->setPos(index[i].start,0);
                        _pos=index[i].count;
                        left=offset-_pos;
                        while(left)
                        {
                             if(left>1000) right=1000;
                                        else right=left;
                             right=demuxer->read(dummy,right);
                             if(!right) return 0;
                             ADM_assert(right<=left);
                             left-=right;
                             _pos+=right;
                        }
                        
                        return 1;
                
                }

        }
  return 0;
}

// __________________________________________________________
// __________________________________________________________

uint32_t
dmxAudioStream::read (uint32_t size, uint8_t * ptr)
{
uint32_t read;          
                
                        if(_pos+size>=_length) 
                        {       
                                printf("DMX_audio Going out of bound\n");
                                return 0;
                        }
                        if(!demuxer->read(ptr,size))
                        {
                                printf("DMX_audio Read failed\n");
                                 return 0;
                        }
                        _pos+=size;     
                        return size;
}

 //
