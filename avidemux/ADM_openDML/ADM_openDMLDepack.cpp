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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "math.h"
#include <ADM_assert.h>
#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_UNPACKER
#include "ADM_toolkit/ADM_debug.h"
#ifdef ADM_DEBUG
	//#define OPENDML_VERBOSE
#endif

typedef struct vopS
{
	uint32_t offset;
	uint32_t type;
}vopS;

static uint32_t searchVop(uint8_t *begin, uint8_t *end,uint32_t *nb, vopS *vop);

static const char *s_voptype[4]={"I frame","P frame","B frame","D frame"};
uint8_t OpenDMLHeader::unpackPacked( void )
{
	uint32_t len,nbFrame;
	uint8_t ret=0;
	uint32_t firstType, secondType,thirdType;
	uint32_t targetIndex=0,nbVop;
	uint32_t nbDuped=0;
	
	vopS	myVops[10]; // should be enough
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
	ADM_assert(newIndex);
	
	printf("Trying to unpack the stream\n");
	DIA_working *working=new DIA_working("Unpacking packed bitstream");
	
	uint32_t img=0;
	while(img<nbFrame)
	{
		working->update(img,nbFrame);
		if(!getFrameNoAlloc(img,buffer,&len))
			{
				printf("Error could not get frame %lu\n",img);
				goto _abortUnpack;
			}
		aprintf("--Frame:%lu/%lu, len %lu\n",img,nbFrame,len);
		
		
		if(len<9) // ??
		{
			if(nbDuped)
			{
				nbDuped--;
			}
			else
			{
				memcpy(&newIndex[targetIndex],&_idx[img],sizeof(_idx[0]));
				aprintf("TOO SMALL\n");
				targetIndex++;
			}
			img++;
			continue;
		}
		
		// now search vop header in this
		// Search first vop
		
		uint8_t *lastPos=buffer;
		uint8_t *endPos=buffer+len;
		aprintf("Frame :%lu",img);
		searchVop(buffer,endPos,&nbVop,myVops);
		
		
		if(!nbVop) goto _abortUnpack;
		
		if(nbVop==1) // only one vop
		{
			memcpy(&newIndex[targetIndex],&_idx[img],sizeof(_idx[0]));
			newIndex[targetIndex].intra=myVops[0].type;
			aprintf("Only one frame found\n");
			targetIndex++;
			img++;
			continue;
		
		}
		nbDuped++;
		// more than one vop, do up to the n-1th
		// the 1st image starts at 0
		myVops[0].offset=0;
		myVops[nbVop].offset=len;
				
		
		uint32_t place;
		for(uint32_t j=0;j<nbVop;j++)
		{

			place=targetIndex+j;
			 if(!j)
				newIndex[place].intra=myVops[j].type;
			else
				newIndex[place].intra=AVI_B_FRAME;
			newIndex[place].size=myVops[j+1].offset-myVops[j].offset;
			newIndex[place].offset=_idx[img].offset+myVops[j].offset;
				
		}				
		targetIndex+=nbVop;	
		img++;
		
	}
	newIndex[0].intra=AVI_KEY_FRAME; // Force
	ret=1;
_abortUnpack:
	delete [] buffer;
	delete working;
#if 0	
	for(uint32_t k=0;k<nbFrame;k++)
	{
		printf("%d old:%lu new: %lu \n",_idx[k].size,newIndex[k].size);
	}	
#endif	
	if(ret=1)
	{
		printf("Sucessfully unpacked the bitstream\n");
		
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
// and return also the vop type
// needed to update the index
uint32_t searchVop(uint8_t *begin, uint8_t *end,uint32_t *nb, vopS *vop)
{
	uint32_t startCode=0xffffffff;
	uint32_t off=0;
	uint32_t voptype;
	*nb=0;
	while(begin<end-3)
	{
		startCode=(startCode<<8)+*begin;
		if((startCode & 0xffffff00) == 0x100)
		{
			if(*begin==0xb6)
			{
				// Analyse a bit the vop header
				uint8_t coding_type=begin[1];
				coding_type>>=6;
				aprintf("\t %d Img type:%s\n",*nb,s_voptype[coding_type]);
				switch(coding_type)
				{
					case 0: voptype=AVI_KEY_FRAME;break;
					case 1: voptype=0;break;
					case 2: voptype=AVI_B_FRAME;break;
					case 3: printf("Glouglou\n");voptype=0;break;
				
				}
				vop[*nb].offset=off-3;
				vop[*nb].type=voptype;
				*nb=(*nb)+1;
			
			}	
		}
		off++;
		begin++;
	
	}

	return 0;
}
