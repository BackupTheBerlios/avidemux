//
//
// C++ Implementation: ADM_odml_regular
//
// Description: This file tries to read a type 1 avi index, building both audio and video 
//			index all along
//
//
// Author: mean <fixounet@free.fr>, (C) 2003
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
#ifdef ADM_DEBUG
	//#define ODML_INDEX_VERBOSE
#endif

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ODML
#include "ADM_toolkit/ADM_debug.h"

uint8_t		OpenDMLHeader::indexRegular(uint32_t vidTrack,uint32_t audTrack,uint32_t audioTrackNumber)
{
uint32_t fcc,flags,offset,len;
uint32_t count,total=0;
uint32_t trackId,ccType;
uint32_t audioCount[3]={0,0,0};
int64_t startOfData;
  
	printf("Trying avi type 1 index\n");
	if(!_regularIndex.offset)
	{
		printf("No regular index\n");
		return 0;
	}
	fseeko(_fd,_regularIndex.offset,SEEK_SET);
	
	// first pass : count how much
	count=_regularIndex.size >> 4;
	while(count)
	{
		fcc=len=0;
		fcc=read32();
		flags=read32();
		offset=read32();
		len=read32();	
		trackId=((fcc>>8) & 0xFF) -'0';;
		if(trackId>3) trackId=0;
	  	ccType=fcc >>16;
		switch(ccType)
		{
		
		case MKFCC('d','c',0,0):	// video tracks
		case MKFCC('d','b',0,0):
			total++;
			break;
		case MKFCC('w','b',0,0):					
			audioCount[trackId]++;
			break;	
		default:
			printf("Unknown fcc:");fourCC::print(fcc);printf("\n");
		}				
		count--;
		
	}
	aprintf("Audio found : %lu %lu %lu\n",audioCount[0],audioCount[1],audioCount[2]);
	 
	 uint32_t audioTrack=0xff;
	 if(audTrack!=0xff ) // we want a track !
	 {
	 	if(audioTrackNumber) // take 2nd track, we clear the 1 st one
		{
			for(uint32_t i=0;i<3;i++)
			{
				if(audioCount[i])  // clear 1st track
				{
					audioCount[i]=0;
					aprintf("Clearing track %lu\n",i);
					break;				
				}
			}
		}
		// search the first valid track we hace found
	 	if(audioCount[0]) audioTrack=0;
	 	else if(audioCount[1]) audioTrack=1;
	 	else if(audioCount[2]) audioTrack=2;
		
	 	if(audioTrack!=0xff)
	 	{
	 		_audioIdx=new odmlIndex[audioCount[audioTrack]];
			memset(_audioIdx,0,sizeof(odmlIndex)*audioCount[audioTrack]);	 
	 	}
	}
	printf("Found %lu videos chunk\n",total);
	fseeko(_fd,_regularIndex.offset,SEEK_SET);
	_idx=new odmlIndex[total];
	memset(_idx,0,sizeof(odmlIndex)*total);
	count=0;
	uint32_t audiocount=0;
	uint32_t audiototal=0;
	uint32_t audioSize=0;
	
	if(audioTrack!=0xff)  audiototal=audioCount[audioTrack];
	while(count<total || audiocount<audiototal)
	{
		fcc=len=0;
		fcc=read32();
		flags=read32();
		offset=read32();
		len=read32();	
		trackId=((fcc>>8) & 0xFF) -'0';;
		if(trackId>3) trackId=0;
	  	ccType=fcc >>16;
		if(count==0 && audiocount==0) // first chunk met
		{
			// the first one is at movie_offset-4+8
			// Its offset + startoff data should be there
			startOfData=_movi.offset+8;
			startOfData-=offset;
		
		}
		switch(ccType)
		{
		
		case MKFCC('d','c',0,0):	// video tracks
		case MKFCC('d','b',0,0):
			_idx[count].offset=offset;
			_idx[count].offset+=startOfData;
			_idx[count].size=len;
			_idx[count].intra=flags;
#ifdef ODML_INDEX_VERBOSE			
			printf("Vid:at %llx size %lu (%lu/%lu) |",_idx[count].offset,len,count,total);fourCC::print(fcc);printf("|\n");
			if(offset & 1) printf("!!!! ODD !!!");	
#endif			
			count++;
			break;
		case MKFCC('w','b',0,0):
			if(audioTrack==trackId)								
			{
				_audioIdx[audiocount].offset=offset;
				_audioIdx[audiocount].offset+=startOfData;
				_audioIdx[audiocount].size=len;
				_audioIdx[audiocount].intra=flags;
				audioSize+=len;
#ifdef ODML_INDEX_VERBOSE							
				printf("Aud:at %llx size %lu (%lu/%lu)|",
					_audioIdx[audiocount].offset,len,audiocount,audiototal);fourCC::print(fcc);printf("|\n");	
#endif				
				if(offset & 1) printf("!!!! ODD !!!");	
				audiocount++;	
			}
			break;	
		default:
			printf("Unknown fcc:");fourCC::print(fcc);printf("\n");
		}		
		
	}
	 _videostream.dwLength= _mainaviheader.dwTotalFrames=total;
	 if(total)
	 	_idx[0].intra=AVI_KEY_FRAME;
	 _nbAudioChunk=audiototal;
	 printf("Found %lu video and %lu audio chunks\n",total,audiototal);
	 printf("Total bytes for audio : %lu bytes\n",audioSize);
	 _audiostream.dwLength=audioSize;	
	 return 1;
}
