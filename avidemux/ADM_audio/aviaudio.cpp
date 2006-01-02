/***************************************************************************
                          aviaudio.cpp  -  description
                             -------------------
    begin                : Fri Nov 23 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr

This class deals with a chunked / not chunked stream
It is an fopen/fwrite lookalike interface to chunks



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
#include <math.h>
#ifdef CYG_MANGLING
#include <io.h>
#endif

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"


//___________________________________
//
//___________________________________
AVDMAviAudioStream::AVDMAviAudioStream(		odmlIndex *idx,
						uint32_t nbchunk,
						FILE  	*fd,
				       		WAVHeader * wav, 
						uint32_t preload,
						uint32_t extraLen,
						uint8_t  *extraData
						
						)      : AVDMGenericAudioStream()
{
    UNUSED_ARG(preload);

   
    _extraLen=extraLen;
    _extraData=extraData;
    if(_extraLen)
    	{
		printf(" we have %lu bytes of extra data in wavheader\n",_extraLen);
	}
    _pos=0;
    _index=idx;
    _destroyable = 0;
    _nb_chunks=nbchunk;
    strcpy(_name, "FILE:AVI");

     _codec=getAudioCodec(wav->encoding,wav,_extraLen,_extraData);
     ADM_assert(_codec);

    _current_index = 0;

    printf("\n Audio streamer initialized");
    _abs_position = 0;
    _rel_position = 0;
    _fd=fd;
    _wavheader = wav;
    _length = 0;

    for(uint32_t i=0;i<_nb_chunks;i++)
    	{
        	_length+=_index[i].size;
       }
	
    printf("\n Total audio length : %lu",_length);
}
//#define VERBOSE_L3
uint8_t	AVDMAviAudioStream::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
	if(isPaketizable())
		return AVDMGenericAudioStream::getPacket(dest,len,samples);
	// it is not packetizable, assume it is correctly muxed
	// else welcome to segfault land
	if(_current_index>=_nb_chunks)
	{
		*len=0;
		*samples=0;
		return 0;	
	}
	fseeko(_fd,_index[_current_index].offset,SEEK_SET);
	fread(dest,1,_index[_current_index].size,_fd);
	*len=_index[_current_index].size;
	*samples=1024; // Common value
        // A bit ugly..
        if(_wavheader->encoding==WAV_IMAADPCM)
        {       
                // how many block ?
                int count=*len/_wavheader->blockalign;
                //nb sample in one block ?
                *samples=2*(_wavheader->blockalign-4*_wavheader->channels)/(_wavheader->channels);
                *samples*=count;
        }
	_current_index++;
	return 1;
}
//___________________________________
//
//___________________________________
uint32_t AVDMAviAudioStream::read(uint32_t len, uint8_t * buffer)
{
    uint32_t togo;
    uint32_t avail, rd;
//    uint32_t askedlen=len;

	if (_current_index>=_nb_chunks) return 0;
    // just to be sure....  
    fseeko(_fd,_abs_position+_rel_position,SEEK_SET);
    togo = len;

#ifdef VERBOSE_L3
    printf("\n ABS: %lu rel:%lu len:%lu", _abs_position, _rel_position,
	   len);
#endif

     do
     {
     	  if(_rel_position>_index[_current_index].size )
	  {
	  	avail=0;
	  }
	  else
	  {
	  	avail = _index[_current_index].size - _rel_position;	// how much available ?
	   }

	  if (avail > togo)	// we can grab all in one move
	    {

		if(!(togo == fread( buffer,1,togo,_fd)))
		{
			printf("\n Badly indexed file, trying to continue anyway...\n");
			 return 0;
		}
#ifdef VERBOSE_L3

		printf("\n REL: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif

		_rel_position += togo;
#ifdef VERBOSE_L3

		printf("\n FINISH: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif

		buffer += togo;
		togo = 0;
	  } else		// read the whole subchunk and go to next one
	    {
#ifdef VERBOSE_L3

		printf("\n CONT: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif

		rd = fread(buffer,1,avail,_fd);
		if (rd != avail)
		  {
		      printf("\n Error : Expected :%lu bytes read :%lu \n",     rd, avail);
		      ADM_assert(0);

		  }
		buffer += avail;
		togo -= avail;

		_current_index++;
		if (_current_index>=_nb_chunks)
		  {
#ifdef VERBOSE_L3

		printf("\n OVR: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif
#if 0
		    _abs_position =_index[0].offset ;
            	     _rel_position = 0;
		     _current_index=0;
		      ADM_assert(len >= togo);
        	      _pos+=len;
                      _pos-=togo;
		      return (len - togo);
#endif
			return 0;	      

		  }
    	else
     	{
#ifdef VERBOSE_L3
		printf("\n CONT: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif
		_abs_position = _index[_current_index].offset;
		_rel_position = 0;
		fseeko(_fd,_abs_position,SEEK_SET);
	    }
      }
    }
    while (togo);
    _pos+=len;
    return len;

}

//_______________________________________________
//
//_______________________________________________
uint8_t AVDMAviAudioStream::goTo(uint32_t newoffset)
{
    uint32_t len;
    len = newoffset;
    if(len>=_length)
    	{
       	printf("\n AudioGoto Out of bound !\n");
     		return 0;
       }
    _current_index = 0;	// start at beginning
#ifdef VERBOSE_L3
    printf("\n Stream offset : %lu\n", newoffset);
#endif
    do
      {
	  if (len >= _index[_current_index].size)	// skip whole subchunk
	    {
		len -= _index[_current_index].size;
		_current_index++;
  		if(_current_index>=_nb_chunks)
    			{
                  printf("\n idx : %lu max: %lu len:%lu\n",  _current_index,_nb_chunks,len);
		  _current_index=0;
                  //ADM_assert(0);
                  //pos=0;
                  return 0;
       		};
		_rel_position = 0;
	  } else		// we got the last one
	    {
	    	_rel_position = len;
		fseeko(_fd,_index[_current_index].offset+len,SEEK_SET);
		len = 0;

	    }
	  //printf("\n %lu len bytes to go",len);
      }
    while (len);
    _abs_position = _index[_current_index].offset;
  ADM_assert(_current_index<_nb_chunks);
    _pos=newoffset;
    return 1;
}



//___________________
