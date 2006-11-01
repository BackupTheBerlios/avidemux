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
#include "default.h"
#include "ADM_editor/ADM_Video.h"


#include "fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_UNPACKER
#include "ADM_osSupport/ADM_debug.h"
#ifdef ADM_DEBUG
	//#define OPENDML_VERBOSE
#endif

typedef struct vopS
{
	uint32_t offset;
	uint32_t type;
}vopS;
#define MAX_VOP 10
static uint32_t searchVop(uint8_t *begin, uint8_t *end,uint32_t *nb, vopS *vop);
uint8_t ADM_findMpegStartCode(uint8_t *start, uint8_t *end,uint8_t *outstartcode,uint32_t *offset);

static const char *s_voptype[4]={"I frame","P frame","B frame","D frame"};
uint8_t OpenDMLHeader::unpackPacked( void )
{
	uint32_t len,nbFrame;
	uint8_t ret=0;
	uint32_t firstType, secondType,thirdType;
	uint32_t targetIndex=0,nbVop;
	uint32_t nbDuped=0;
	
	vopS	myVops[MAX_VOP]; // should be enough
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
	
	odmlIndex *newIndex=new odmlIndex[nbFrame+MAX_VOP]; // Due to the packed vop, we may end up with more images
							// Assume MAX_VOP Bframes maximum
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
                                aprintf(" skipped\n");
				nbDuped--;
                                aprintf("At %u, %d duped!\n",img,nbDuped);
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
		if(nbDuped)
                {
                        aprintf("At %u, %d duped!\n",img,nbDuped);
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
                        if(myVops[0].type==AVI_KEY_FRAME)
                        {
                                if(nbDuped)
                                {
                                        ADM_assert(targetIndex);
                                        if(newIndex[targetIndex-1].intra==AVI_B_FRAME)
                                        {
                                                printf("Trying to fix wrong vop packed\n");
                                                targetIndex--;
                                                nbDuped--;
                                        }
                                }
                        }
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
                if(nbVop>2)
                {
                        aprintf("At %u, %d vop!\n",img,nbVop);
                }
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
	printf("Initial # of images : %lu, now we have %lu \n",nbFrame,targetIndex);
	nbFrame=targetIndex;
	
	return ret;
}
// Search a start vop in it
// and return also the vop type
// needed to update the index
uint32_t searchVop(uint8_t *begin, uint8_t *end,uint32_t *nb, vopS *vop)
{
	
	uint32_t off=0;
	uint32_t globalOff=0;
	uint32_t voptype;
	uint8_t code;
	*nb=0;
	while(begin<end-3)
	{
    	if( ADM_findMpegStartCode(begin, end,&code,&off))
    	{
        	if(code==0xb6)
			{
				// Analyse a bit the vop header
				uint8_t coding_type=begin[off];
				coding_type>>=6;
				aprintf("\t at %u %d Img type:%s\n",off,*nb,s_voptype[coding_type]);
				switch(coding_type)
				{
					case 0: voptype=AVI_KEY_FRAME;break;
					case 1: voptype=0;break;
					case 2: voptype=AVI_B_FRAME;break;
					case 3: printf("Glouglou\n");voptype=0;break;
				
				}
        	    vop[*nb].offset=globalOff+off-4;
				vop[*nb].type=voptype;
				*nb=(*nb)+1;
				begin+=off+1;
				globalOff+=off+1;
				continue;
			
			}	
        	begin+=off;
        	globalOff+=off;
        	continue;
    	}
    	return 1; 
    }   
	return 1;
}
