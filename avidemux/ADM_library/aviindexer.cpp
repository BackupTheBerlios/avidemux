/***************************************************************************
                          aviindexer.cpp  -  description
                             -------------------
   This file convert a linked list into a regular array

    begin                : Tue Feb 19 2002
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
 #if 0
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
//#include <sstream>
#include <ADM_assert.h>
#include <math.h>
#include "avio.hxx"
#include "fourcc.h"
#include "subchunk.h"

// Convert DTS to PTS index
//
uint8_t aviHeader::reorder( void )
{
ShortIndex *index;
uint8_t ret=1;

	// already done..
	if( _reordered) return 1;
	ADM_assert(_videoindex);
	index=new ShortIndex[_nb_vchunk];
	// clear B frame flag for last frames
	_videoindex[_nb_vchunk-1].Flags &=~AVI_B_FRAME;

			//__________________________________________
			// the direct index is in DTS time (i.e. decoder time)
			// we will now do the PTS index, so that frame numbering is done
			// according to the frame # as they are seen by editor / user
			// I1 P0 B0 B1 P1 B2 B3 I2 B7 B8
			// xx I1 B0 B1 P0 B2 B3 P1 B7 B8
			//__________________________________________
			uint32_t forward=0;
			uint32_t curPTS=0;
			uint32_t dropped=0;

			for(uint32_t c=1;c<_nb_vchunk;c++)
			{
				if(!(_videoindex[c].Flags & AVI_B_FRAME))
					{
								memcpy(&index[curPTS],
										&_videoindex[forward],
										sizeof(ShortIndex));
								forward=c;
								curPTS++;
								dropped++;
					}
					else
					{			// we need  at lest 2 i/P frames to start decoding B frames
								if(dropped>=2)
								{
								memcpy(&index[curPTS],
										&_videoindex[c],
										sizeof(ShortIndex));
								curPTS++;
								}
					}
			}

			uint32_t last;


			// put back last I/P we had in store
			memcpy(&index[curPTS],
				&_videoindex[forward],
				sizeof(ShortIndex));
			last=curPTS;

			_videostream.dwLength= _mainaviheader.dwTotalFrames=_nb_vchunk=last+1;
			// last frame is always I

			delete [] _videoindex;

			_videoindex=index;;
			 _reordered=ret;
			return ret;

}

//---------------------------------------------------------------
uint8_t aviHeader::mergeIndex(uint32_t nbframe, SubChunk * First,
			      ShortIndex ** idx)
{
    ShortIndex *curidx;
    SubChunk *old;


    //*idx = (ShortIndex *) malloc(sizeof(ShortIndex) * nbframe);
    *idx=new ShortIndex[nbframe];
    ADM_assert(*idx);
    curidx = *idx;
    for (uint32_t i = 0; i < nbframe; i++)
      {
	  if(!First)
	  {
		printf("Error : %lu / %lu end of chain \n",i,nbframe);
		ADM_assert(0);

	  }
	  curidx->Offset = First->_offset;
	  curidx->Length = First->_len;
	  curidx->Flags = First->_flags;

//   		printf("\n at : %lu length : %lu flags :%08lX", curidx->Offset ,
//               curidx->Length ,    curidx->Flags);

	  curidx++;
	  // delete it
	  old = First;
	  First = First->_next;
	  delete old;
      }
    (*idx)->Flags|=AVI_KEY_FRAME;

    return 1;

}

// EOF
#endif
