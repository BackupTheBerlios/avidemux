/***************************************************************************
                          ADM_3gpAudio.cpp  -  description
                             -------------------

	Provide access to the audio track embedded in 3gp file
	It can be either AMR NB/WB/ AAC
	The most common being AMR NB

 ***************************************************************************

    begin                : Tue Jul 23 2003
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
#include "config.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include "ADM_library/fourcc.h"
#include "ADM_3gp/ADM_3gp.h"
//_______________________________________________________
//
//
//_______________________________________________________

_3gpAudio::_3gpAudio(_3gpIndex *idx, uint32_t nbchunk, FILE * fd,WAVHeader *incoming)
{
	_nb_chunks=nbchunk;
	_fd=fd;
	_current_index=0;
	_abs_position=0;
	_rel_position=0;
	_pos=0;
	_index=idx;

	_wavheader=new WAVHeader;
	memset(_wavheader,0,sizeof(WAVHeader));
	// AMR like...
	if(incoming==NULL)
	{
		_wavheader->bitspersample=16;
		_wavheader->frequency=8000;
		_wavheader->channels=1;
		_wavheader->encoding=0xff00; //WAV_AMR_NB; // ??
		_wavheader->byterate=16000;
	}
	else // what we just found...
	{
		memcpy(_wavheader,incoming,sizeof(WAVHeader));
	}

	_destroyable=1;	
	strcpy(_name,"3gp audio");	
	// compute length
	_length=0;
	for(uint32_t i=0;i<_nb_chunks;i++)
		{
			_length+=_index[i].size;
		}
	printf("\n 3gp audio : %lu bytes (%lu chunks)\n",_length,_nb_chunks);

	printf("Byterate     :%d\n",_wavheader->byterate);
	printf("Frequency :%d\n",_wavheader->frequency);
	printf("Encoding   :%d\n",_wavheader->encoding);
	printf("Channels   :%d\n",_wavheader->channels);
    	goTo(0);


}
//_______________________________________________________
//
//
//_______________________________________________________
uint8_t _3gpAudio::goTo(uint32_t newoffset)
{
    uint32_t len;
    len = newoffset;
    if(len>=_length)
    	{
       	printf("\n Out of bound !\n");
     		return 0;
       }
    _current_index = 0;	// start at beginning
    do
      {
	  if (len >= _index[_current_index].size)	// skip whole subchunk
	    {
		len -= _index[_current_index].size;
		_current_index++;
  		if(_current_index>=_nb_chunks)
    			{
                  printf("\n idx : %lu max: %lu len:%lu\n",  _current_index,_nb_chunks,len);
                  return 0;
       		};
		_rel_position = 0;
	  } else		// we got the last one
	    {
		_rel_position = len;
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
//_______________________________________________________
//
//
//_______________________________________________________
uint32_t _3gpAudio::read(uint32_t len,uint8_t *buffer)
{
    uint32_t togo;
    uint32_t avail, rd;

    // just to be sure....

   fseeko(_fd,_abs_position+_rel_position,SEEK_SET);

    togo = len;

#ifdef VERBOSE_L3
    printf("\n ABS: %lu rel:%lu len:%lu", _abs_position, _rel_position,
	   len);
#endif

     do
     {
	  avail = _index[_current_index].size - _rel_position;	// how much available ?

	  if (avail > togo)	// we can grab all in one move
	    {
			if(togo!= fread( (uint8_t *) buffer,1,togo,_fd))
			{
				printf("\n ***WARNING : incomplete chunk ***\n");
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
		rd = fread( buffer,1,avail,_fd);
		if (rd != avail)
		  {
		      printf("\n Error : Expected :%lu bytes read :%lu \n",     rd, avail);
		      //ADM_assert(0);
		      return rd;

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
		      _abs_position =_index[0].offset ;
            		_rel_position = 0;
		      ADM_assert(len >= togo);
        	  	_pos+=len;
            		_pos-=togo;
		      return (len - togo);

		  }
    	else
     	{
#ifdef VERBOSE_L3
		printf("\n CONT: %lu rel:%lu len:%lu", _abs_position,
		       _rel_position, togo);
#endif
		_abs_position = _index[_current_index].offset;
		_rel_position = 0;
		//_riff->goTo(_abs_position);
		fseeko(_fd,_abs_position,SEEK_SET);
	    }
      }
    }
    while (togo);
    _pos+=len;
    return len;

}
//_______________________________________________________
//
//
//_______________________________________________________


_3gpAudio::~_3gpAudio()
{
	// nothing special to do...

}
//_______________________________________________________
uint8_t _3gpAudio::getNbChunk(uint32_t *ch)
{
	*ch=_nb_chunks;
	return 1;
}
