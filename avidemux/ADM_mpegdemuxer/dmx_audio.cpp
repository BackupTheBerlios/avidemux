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
  if (_index)
    delete[] _index;

  if (demuxer)
    delete demuxer;

  if(_tracks)
      delete [] _tracks;

  _index = NULL;
  _wavheader = NULL;
  demuxer = NULL;

}
dmxAudioStream::dmxAudioStream (void)
{
  _wavheader = NULL;
  demuxer = NULL;
  nbTrack = 0;
  _tracks=NULL;
  currentTrack=0;
  _index=NULL;
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
  int multi;

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

char *start;
  fgets (string, MAX_LINE, file);
  //sscanf (string, "File     : %s\n", realname);
  start=strstr(string,":");
  ADM_assert(start);
  strcpy(realname,start+2);

  int l=strlen(realname)-1;
  while(l&&(realname[l]==0x0a || realname[l]==0x0d))
  {
           realname[l]=0;
           l--;
  }


  fgets (string, MAX_LINE, file);
  sscanf(string,"Append   : %d\n",&multi);

  fgets (string, MAX_LINE, file);
  sscanf (string, "Image    : %c\n", &progressif);	// Progressive

  fgets (string, MAX_LINE, file);
  sscanf (string, "Picture  : %u x %u %u fps\n", &w, &h, &fps);	// width...

  fgets (string, MAX_LINE, file);
  sscanf (string, "Nb Gop   : %u \n", &nbGop);	// width...

  fgets (string, MAX_LINE, file);
  sscanf (string, "Nb Images: %u \n", &nbFrame);	// width...

  fgets (string, MAX_LINE, file);
  sscanf(string,"Nb Audio : %u\n",&nbAudioStream); 

  fgets(string,MAX_LINE,file);
  sscanf(string,"Main aud : %u\n",&mainAudio); 

  if(!nbAudioStream)
  {
_abrt:
         fclose (file);
        return 0;
  }
  _tracks=new dmxAudioTrack[nbAudioStream];
  nbTrack=nbAudioStream;
  _index=new dmxAudioIndex[nbGop+1];
  fgets (string, MAX_LINE, file);

  char *needle,*hay;
  uint32_t i=0;
/*
    Extrcat the PES/PID for each audio track from the line
        A(PES):(PID) A(PES):(PID) ....
*/
  hay=string;
  while(1)
  {
        needle=strstr(hay," A");
        if(!needle) goto _nnx;
        sscanf(needle," A%x:%x",&aPid,&aPes);
        ADM_assert(i<nbAudioStream);
        _tracks[i].myPid=aPid;
        _tracks[i].myPes=aPes;
        needle[0]='.';
        needle[1]='.';
        hay=needle;
        i++;
  }
_nnx:
  ADM_assert(i==nbAudioStream);
  currentTrack=mainAudio;
  // Now build the demuxers
  // We build only one demuxer, then we will use changePid
  // To go from one track to another
MPEG_TRACK track;
                track.pes=_tracks[mainAudio].myPes;
                track.pid=_tracks[mainAudio].myPid;
  switch (type)
    {
    case 'P':
                demuxer = new dmx_demuxerPS (1,&track,multi);
                break;
    case 'T':
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
 
  printf ("Building index with %u sync points\n", nbGop);

  uint32_t read = 0, img, count;
  uint64_t abs;

  
  while (read < nbGop)
    {
      if (!fgets (string, MAX_LINE, file))
	break;
      if (string[0] != 'A')
	continue;

      sscanf (string, "A %u %"LLX, &img, &abs); //FIXME read all audio tracks and pick the one we want
        hay=strstr(string,":");
        ADM_assert(hay)
        i=0;
       while(1)
        {
                needle=strstr(hay,":");
                if(!needle) goto _nxt;;
                sscanf(needle,":%lx",&count);
                needle[0]='.';                
                hay=needle;
                _index[read].count[i] = count;
                i++;
        }
_nxt:


      _index[read].img = img;
      _index[read].start = abs;
      
      read++;
    }
  fclose (file);
  nbIndex = read;
  if (!read)
    {
      printf ("No audio at all!\n");              
      return 0;
    }
printf("Filling audio header\n");
  // now fill in the header
  _length = _index[nbIndex - 1].count[mainAudio];
  for(int i=0;i<nbTrack;i++)
  {
    WAVHeader *hdr;
    hdr=&(_tracks[i].wavHeader);
    memset (hdr, 0, sizeof (WAVHeader));

    // put some default value
    hdr->bitspersample = 16;
    hdr->blockalign = 4;
  
    _destroyable = 1;
    strcpy (_name, "dmx audio");
    //demuxer->setPos (0, 0);
  }
  printf("Audio index loaded, probing...\n");
  if(!probeAudio()) return 0;
        
  demuxer->changePid(_tracks[currentTrack].myPid,_tracks[currentTrack].myPes);
  demuxer->setPos (0, 0);
  _pos = 0;
  printf ("\n DMX audio initialized (%lu bytes)\n", _length);
  printf ("With %lu sync point\n", nbIndex);
  changeAudioTrack(mainAudio);
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

        if(offset<_index[0].count[currentTrack])
        {
                demuxer->setPos(0,0);
                _pos=0;
                fnd=1;
        }
        else
        {
                for(uint32_t i=0;i<nbIndex-1;i++)
                {
                        if(_index[i].count[currentTrack]<=offset && _index[i+1].count[currentTrack]>offset)
                        {
                                demuxer->setPos(_index[i].start,0);
                                _pos=_index[i].count[currentTrack];
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
                            printf("DMX_audio Going out of bound (position : %u asked %u end%u)\n",_pos,size,_length);
                            size=_length-_pos;
                            if(_pos==_length) return 0;
                        }
                        if(!(size=demuxer->read(ptr,size)))
                        {
                            printf("DMX_audio Read failed (got:%u)\n",size);
                            _pos+=size;
                            return 0;
                        }
                        _pos+=size;     
                        return size;
}
// __________________________________________________________
// __________________________________________________________

uint8_t dmxAudioStream::probeAudio (void)
{
uint32_t read,offset,offset2,fq,br,chan,myPes,blocksize;          
uint8_t buffer[PROBE_SIZE];
MpegAudioInfo mpegInfo;
WAVHeader *hdr;

      for(int i=0;i<nbTrack;i++)
      {
        hdr=&(_tracks[i].wavHeader);
        // Change demuxer...

        demuxer->changePid(_tracks[i].myPid,_tracks[i].myPes);
        demuxer->setPos(0,0);
        printf("Probing track:%d, pid: %x pes:%x\n",i,_tracks[i].myPid,_tracks[i].myPes);
         //
        if(PROBE_SIZE!=(blocksize=demuxer->read(buffer,PROBE_SIZE)))
        {
           printf("DmxAudio: Reading for track %d failed (%u/%u)\n",i,blocksize,PROBE_SIZE);
           return 0;
        }
        myPes=_tracks[i].myPes;
        // Try mp2/3
        if(myPes>=0xC0 && myPes<=0xC9)
        {
                if(getMpegFrameInfo(buffer,PROBE_SIZE,&mpegInfo,NULL,&offset))
                {
                        if(getMpegFrameInfo(buffer+offset,PROBE_SIZE-offset,&mpegInfo,NULL,&offset2))
                                if(!offset2)
                                {
                                        hdr->byterate=(1000*mpegInfo.bitrate)>>3;
                                        hdr->frequency=mpegInfo.samplerate;

                                        if(mpegInfo.mode!=3) hdr->channels=2;
                                        else hdr->channels=1;

                                        if(mpegInfo.layer==3) hdr->encoding=WAV_MP3;
                                        else hdr->encoding=WAV_MP2;
                                        continue;
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
                                hdr->byterate=br; //(1000*br)>>3;
                                hdr->frequency=fq;                                
                                hdr->encoding=WAV_AC3;
                                hdr->channels=chan;
                                continue;
                        }
                }
        }
        // Default 48khz stereo lpcm
        if(myPes>=0xA0 && myPes<0xA9)
        {
                hdr->byterate=(48000*4);
                hdr->frequency=48000;                                
                hdr->encoding=WAV_LPCM;
                hdr->channels=2;
                continue;
        }
  }
  return 1;
}

uint8_t dmxAudioStream::changeAudioTrack(uint32_t newtrack)
{
      ADM_assert(newtrack<nbTrack);
      currentTrack=newtrack;

      _length=_index[nbIndex-1].count[currentTrack];
      _wavheader=&(_tracks[currentTrack].wavHeader);
      // Set demuxer...
      demuxer->changePid(_tracks[currentTrack].myPid,_tracks[currentTrack].myPes);
      //
      return 1;
}
uint8_t                 dmxAudioStream::getAudioStreamsInfo(uint32_t *nbStreams, uint32_t **infos)
{
    *nbStreams=0;
    *infos=NULL;
    if(!nbTrack) return 1;
    *nbStreams=nbTrack;
    *infos=new uint32_t [nbTrack];
    for(int i=0;i<nbTrack;i++)
        (*infos)[i]=_tracks[i].wavHeader.encoding;
    return 1;
}


 //
