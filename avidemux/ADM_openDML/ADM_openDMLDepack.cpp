//
// C++ Implementation: ADM_openDMLDepack
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ODML
#include "ADM_toolkit/ADM_debug.h"
#ifdef ADM_DEBUG
	//#define OPENDML_VERBOSE
#endif
static uint32_t searchVop(uint8_t *begin, uint8_t *end);


uint8_t OpenDMLHeader::unpackPacked( void )
{
	uint32_t len,nbFrame;
	uint8_t ret=0;
	
	// here we got the vidHeader to get the file easily
	// we only deal with avi now, so cast it to its proper type (i.e. avi)
		
	
	// now we are ready to rumble
	// First get a unpack buffe
	uint8_t *buffer=new uint8_t [2*getWidth()*getHeight()];

	// For each frame we lookup 2 times the VOP header
	// the first one become frame n, the second one becomes frame N+1
	// Image contaning only VOP header are royally ignored
	// We should get about the same number of in/out frame
	
	nbFrame=getMainHeader()->dwTotalFrames;
	
	odmlIndex *newIndex=new odmlIndex[nbFrame];
	assert(newIndex);
	uint32_t newTotal=0;
	printf("Trying to unpack the stream\n");
	
	for(uint32_t img=0;img<nbFrame;img++)
	{
		
		if(!getFrameNoAlloc(img,buffer,&len))
			{
				printf("Error could not get frame %lu\n",img);
				goto _abortUnpack;
			}
		aprintf("--Frame:%lu/%lu, len %lu\n",img,nbFrame,len);			
		if(len<7)
			{
				aprintf("    Dummy frame, skipped\n");
				continue;
			}
		// now search vop header in this
		// Search first vop
		
		uint8_t *lastPos=buffer;
		uint8_t *endPos=buffer+len;
		
		uint32_t firstVopOffset=0,secondVopOffset=0;
		
		firstVopOffset=searchVop(buffer,endPos);
		secondVopOffset=searchVop(buffer+firstVopOffset,endPos);
		
		if(!firstVopOffset)
		{
			aprintf("Did not find a single vop....aborting\n");
			goto _abortUnpack;			
		}
		// If there is nno secondVop, take the whole frame
		firstVopOffset-=4;
		aprintf("Frame 1 dump %lu :\n",firstVopOffset);
		//mixDump(buffer+firstVopOffset,8);
		aprintf("\n");
		
		if(!secondVopOffset)
		{
			memcpy(&newIndex[newTotal],&_idx[img],sizeof(_idx[0]));
			newTotal++;
			aprintf("Only one frame found\n");
		}
		else
		{
			
			aprintf("Frame 2 dump %lu :\n",secondVopOffset);
			//mixDump(buffer+secondVopOffset,8);
			aprintf("\n");
			
			// split it
			memcpy(&newIndex[newTotal],&_idx[img],sizeof(_idx[0]));
			newIndex[newTotal].size=secondVopOffset;
			newTotal++;
			aprintf(" one frame found:%lu\n",secondVopOffset);
			// second part, most probably a  B Frame
			newIndex[newTotal].offset=_idx[img].offset+secondVopOffset;
			newIndex[newTotal].size=len-secondVopOffset;
			newIndex[newTotal].intra=AVI_B_FRAME;
			newTotal++;
			aprintf(" two frame found:%lu\n",len-secondVopOffset);
		}
		
	}
	ret=1;
_abortUnpack:
	delete [] buffer;
	if(ret=1)
	{
		printf("Sucessfully unpacked the bitstream\n");
		printf("Originally %lu frames, now %lu frames\n",nbFrame,newTotal);
		delete [] _idx;
		_idx=newIndex;
	}
	else
	{
		delete [] newIndex;
		printf("Could not unpack this...\n");
	}
	return ret;
}
// Search a start vop in it
uint32_t searchVop(uint8_t *begin, uint8_t *end)
{
	uint32_t startCode=0xffffffff;
	uint32_t off=0;
	while(begin<end-3)
	{
		startCode=(startCode<<8)+*begin;
		if((startCode & 0xffffff00) == 0x100)
		{
			aprintf("Startcode : %d\n",*begin);
			if(*begin==0xb6)
			{
				
				return off+1;
			
			}	
		}
		off++;
		begin++;
	
	}

	return 0;
}
