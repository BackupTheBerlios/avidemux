/***************************************************************************
                          audiotimeline.cpp  -  description
                             -------------------
    Try to build a timeline for audio track
    Very useful for VBR audio
    the bitrate*time does not work in that case

    begin                : Fri May 3 2002
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
//#include <stream.h>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "aviaudio.hxx"

#ifdef USE_MP3
#include "ADM_audiocodec/avdm_mad.h"
#endif
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"



#define ATOM 100 //1152>>1

uint8_t AVDMGenericAudioStream::buildAudioTimeLine(void)
{
#ifndef USE_MP3
     return 0;
#else
uint32_t in,d=0,rd=0,ms10,offset=0,index=0; //,left=0,len=0;;
uint8_t *ptr;
		// we do nothing in case of WAV or unhandler stream
  		// that left only MP3 for now ...
     if (_wavheader->encoding != WAV_MP3 &&_wavheader->encoding != WAV_MP2 )
      {	
	  return 0;
      }
      // in case of mpeg3 we will build a pseudo memory map
      // one tick is 10 ms
      // 100*3600*2*4= 360*8*1000=3000 000= 3 meg
      // for a 2 hour movie
      // should be acceptable


      goTo(0);  //rewind
      printf("\n scanning timeline\n");
      // ms10 is the raw length of 10 ms of uncompressed audio
      ms10=_wavheader->channels*_wavheader->frequency*2;
      ms10=ms10/100; // 16 bits per sample
      printf("\n 10 ms is %lu\n",ms10);
      ptr=(uint8_t *)malloc(2*64*1024); // should be enough
		_audioMap=new ST_point[100*3600*4]; // 4 hours / 6 Megs
      assert(ptr);
      assert(_audioMap);

      // Initialize MAD decoding engine
      AVDM_MadInit();

      DIA_working *work;

      work=new DIA_working("Building VBR map");

      goTo(0);
      while(offset<_length)
      	{
              			work->update(offset,_length);
				if(!work->isAlive())
				{
					delete work;
					work=new DIA_working("Building VBR map");
					work->update(offset,_length);
				}
                
                		// read a big chunk
						if (!(in = read(ATOM,internalBuffer)))	// FIXME
		  				{
					      printf(" read failed, end of stream ? \n");
					      goto end;
		      			}		      					         	   
			        	offset+=ATOM;
			      		AVDM_MadRun(internalBuffer, in, ptr, &d);
						if (d)
					  		{
			    		  	rd += d;
      						_audioMap[index].foffset=offset;
      						_audioMap[index++].woffset=rd;
            			//	printf("\n At : %lu , w: %lu",offset,rd);
			  				}
         				

         }

end:         
      _nbMap=index;
      printf("\n Nb entries in timeline : %lu\n",_nbMap);
      delete work;
      AVDM_MadEnd();

      free(ptr);    
     return 1;
#endif
}

//
//	Read one byte
//
//
uint8_t  AVDMGenericAudioStream::readc( uint8_t *c)
{
 	uint32_t status;
  			status=read(1,c);
     		return status;

}

