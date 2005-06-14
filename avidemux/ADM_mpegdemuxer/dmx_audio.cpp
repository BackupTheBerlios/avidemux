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
#include "ADM_audio/ADM_mp3info.h"
#include "ADM_audio/ADM_a52info.h"
#define MAX_LINE 4096
#define PROBE_SIZE (4096*2)

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
  uint32_t aPid, vPid, aPes,mainAudio;
  uint32_t nbGop, nbFrame,nbAudioStream;

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
  sscanf (string, "Nb Gop   : %lu \n", &nbGop);	// width...

  fgets (string, MAX_LINE, file);
  sscanf (string, "Nb Images: %lu \n", &nbFrame);	// width...

  fgets (string, MAX_LINE, file);
  sscanf(string,"Nb Audio : %lu\n",&nbAudioStream); 

  fgets(string,MAX_LINE,file);
  sscanf(string,"Main aud : %lu\n",&mainAudio); 

  if(!nbAudioStream)
  {
_abrt:
         fclose (file);
        return 0;
  }
  fgets (string, MAX_LINE, file);

  char *needle,*hay;
  uint32_t i=0;

  hay=string;
  while(i!=mainAudio+1)
  {
        needle=strstr(hay," A");
        if(!needle) goto _abrt;
        sscanf(needle," A%x:%x",&aPid,&aPes);
        needle[0]='.';
        needle[1]='.';
        hay=needle;
        i++;
  }
  printf("Using Track :%x Pid:%x Es:%x for audio\n",mainAudio,aPid,aPes);
  // Build the streams
  myPes=aPes;
MPEG_TRACK track;
  switch (type)
    {
    case 'P':
                
                track.pes=aPes;
                track.pid=aPid;
                demuxer = new dmx_demuxerPS (1,&track);
                break;
    case 'T':
                track.pes=aPes;
                track.pid=aPid;
                demuxer = new dmx_demuxerTS (1,&track,0);
                break;
    default:
      ADM_assert (0);
    }
  if(!demuxer->open(realname))
        {
                printf("DMX audio : cannot open %s\n");
                
                fclose(file);                
                return 0;
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

      sscanf (string, "A %lu %"LLX, &img, &abs); //FIXME read all audio tracks and pick the one we want
        hay=strstr(string,":");
        ADM_assert(hay)
        i=0;
       while(i!=mainAudio+1)
        {
                needle=strstr(hay,":");
                if(!needle) ADM_assert(0);
                sscanf(needle,":%lx",&count);
                needle[0]='.';                
                hay=needle;
                i++;
        }



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
  
  _destroyable = 1;
  strcpy (_name, "dmx audio");
  demuxer->setPos (0, 0);

  if(!probeAudio()) return 0;
        printf("Probed audio\n");
        printf("Br:%lu\n",(_wavheader->byterate*8)/1000);
        printf("Fq:%lu\n",_wavheader->frequency);
        printf("Ch:%lu\n",_wavheader->channels);
  demuxer->setPos (0, 0);
  _pos = 0;
  printf ("\n DMX audio initialized (%lu bytes)\n", _length);
  printf ("With %lu sync point\n", nbIndex);
  return 1;
}
// __________________________________________________________
// __________________________________________________________

uint8_t
dmxAudioStream::goTo (uint32_t offset)
{
uint8_t dummy[1024];
uint32_t left,right;
int fnd=0;
        
        // Search into the index to take the neareast one
        if(offset>=_length) return 0;

        if(offset<index[0].count)
        {
                demuxer->setPos(0,0);
                _pos=0;
                fnd=1;
        }
        else
        {
                for(uint32_t i=0;i<nbIndex-1;i++)
                {
                        if(index[i].count<=offset && index[i+1].count>offset)
                        {
                                demuxer->setPos(index[i].start,0);
                                _pos=index[i].count;
                                fnd=1;
                                break;
                
                        }

                }
        }
        if(!fnd)
        {
         printf("DMX audio : failed!\n");
         return 0;
        }
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
                        if(!(size=demuxer->read(ptr,size)))
                        {
                                printf("DMX_audio Read failed\n");
                                 return 0;
                        }
                        _pos+=size;     
                        return size;
}
// __________________________________________________________
// __________________________________________________________

uint8_t dmxAudioStream::probeAudio (void)
{
uint32_t read,offset,offset2,fq,br,chan;          
uint8_t buffer[PROBE_SIZE];
MpegAudioInfo mpegInfo;
                
        if(PROBE_SIZE!=demuxer->read(buffer,PROBE_SIZE)) return 0;

        // Try mp2/3
        if(myPes>=0xC0 && myPes<0xC9)
        {
                if(getMpegFrameInfo(buffer,PROBE_SIZE,&mpegInfo,NULL,&offset))
                {
                        if(getMpegFrameInfo(buffer+offset,PROBE_SIZE-offset,&mpegInfo,NULL,&offset2))
                                if(!offset2)
                                {
                                        _wavheader->byterate=(1000*mpegInfo.bitrate)>>3;
                                        _wavheader->frequency=mpegInfo.samplerate;

                                        if(mpegInfo.mode!=3) _wavheader->channels=2;
                                        else _wavheader->channels=1;

                                        if(mpegInfo.layer==3) _wavheader->encoding=WAV_MP3;
                                        else _wavheader->encoding=WAV_MP2;
                                        return 1;
                                }
                }
        }
        // Try AC3
        if(myPes<9)
        {
                if(ADM_AC3GetInfo(buffer,PROBE_SIZE,&fq,&br,&chan,&offset))
                {
                        if(ADM_AC3GetInfo(buffer+offset,PROBE_SIZE-offset,&fq,&br,&chan,&offset2))
                        {
                                _wavheader->byterate=br; //(1000*br)>>3;
                                _wavheader->frequency=fq;                                
                                _wavheader->encoding=WAV_AC3;
                                _wavheader->channels=chan;
                                return 1;
                        }
                }
        }
        // Default 48khz stereo lpcm
        if(myPes>=0xA0 && myPes<0xA9)
        {
                _wavheader->byterate=(48000*4);
                _wavheader->frequency=48000;                                
                _wavheader->encoding=WAV_LPCM;
                _wavheader->channels=2;
                return 1;
        }
        return 0;
}
#if 0
void dmxAudioStream::checkAudio(void)
{
        lastImg=index[0].image;
        lastCount=index[0].count;
        for(uint32_t i=1;i<nbIndex-1;i++)
        {
                deltaImage=index[i].image-lastImg;
                deltaSound=index[i].count-lastImg;

        }

}
#endif
 //
