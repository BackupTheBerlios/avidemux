/***************************************************************************
                          ADM_OpenDML.cpp  -  description
                             -------------------
	
		OpenDML redordering, convert PTS<->DTS order
		
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

#include "default.h"
#include "ADM_editor/ADM_Video.h"


#include "fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_ODML
#include "ADM_osSupport/ADM_debug.h"

#define OPENDML_VERBOSE
uint8_t			OpenDMLHeader::isReordered( void )
{ 
 	return _reordered;
}

uint8_t OpenDMLHeader::reorder( void )
{
odmlIndex *index;
uint8_t ret=1;
uint32_t nbFrame= _videostream.dwLength;
	// already done..
	if( _reordered) return 1;
	ADM_assert(_idx);
	
	index=new odmlIndex[nbFrame];
	// clear B frame flag for last frames
	_idx[nbFrame-1].intra &=~AVI_B_FRAME;

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

			for(uint32_t c=1;c<nbFrame;c++)
			{
				if(!(_idx[c].intra & AVI_B_FRAME))
					{
								memcpy(&index[curPTS],
										&_idx[forward],
										sizeof(odmlIndex));
								forward=c;
								curPTS++;
								dropped++;
					}
					else
					{// we need  at lest 2 i/P frames to start decoding B frames
						if(dropped>=1)
						{
							memcpy(&index[curPTS],
								&_idx[c],
								sizeof(odmlIndex));
							curPTS++;
						}
						else
						{
						printf("We dropped a frame (%d/%d).\n",dropped,c);
						}
					}
			}

			uint32_t last;


			// put back last I/P we had in store
			memcpy(&index[curPTS],
				&_idx[forward],
				sizeof(odmlIndex));
			last=curPTS;

			_videostream.dwLength= _mainaviheader.dwTotalFrames=nbFrame=last+1;
			// last frame is always I

			delete [] _idx;

			_idx=index;;
			// last frame cannot be B frame
			_idx[last].intra&=~AVI_B_FRAME;
			 _reordered=ret;
			return ret;

}
