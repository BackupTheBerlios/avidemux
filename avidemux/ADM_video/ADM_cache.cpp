//
// C++ Implementation: ADM_cache
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
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_cache.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"


VideoCache::VideoCache(uint32_t nb,AVDMGenericVideoStream *in)
{
uint32_t sz;
	nbEntry=nb;
	incoming=in;
	memcpy(&info,in->getInfo(),sizeof(info));
	// Ready buffers
	frameNum=new uint32_t[nbEntry];
	frameLock=new uint8_t[nbEntry];
	frameBuffer=new ADMImage *[nbEntry]; //(uint8_t **)malloc( nbEntry*sizeof(uint8_t *));
	sz=(info.width*info.height*3)>>1;
	for(uint32_t i=0;i<nbEntry;i++)
	{
		frameNum[i]=0xffff0000;
		frameLock[i]=0;
		frameBuffer[i]=new ADMImage(info.width,info.height);	
	}
	lastUsed=0;
}
//_____________________________________________
VideoCache::~ VideoCache()
{
	for(uint32_t i=0;i<nbEntry;i++)
	{
		delete  frameBuffer[i];
	}
	delete [] frameBuffer;
	delete [] frameLock;
	delete [] frameNum;
	
}
//_____________________________________________
int32_t VideoCache::searchFrame( uint32_t frame)
{
	for(uint32_t i=0;i<nbEntry;i++)
	{
		if(frameNum[i]==frame) return i;
	}
	return -1;
}
//_____________________________________________
int32_t 	 VideoCache::searchPtr( ADMImage *ptr)
{
	for(uint32_t i=0;i<nbEntry;i++)
	{
		if(frameBuffer[i]==ptr) return i;
	}
	return -1;
}
//_____________________________________________
uint8_t  VideoCache::unlockAll(void)
{
	for(uint32_t i=0;i<nbEntry;i++)
	{		
		frameLock[i]=0;		
	}
	return 1;
}
//_____________________________________________
uint8_t  VideoCache::unlock(ADMImage *frame)
{
int32_t k;
	k=searchPtr(frame) ;
	ADM_assert(k>=0);
	frameLock[k]--;
	return 1;	
}
//_____________________________________________
uint8_t  VideoCache::purge(void)
{
	for(uint32_t i=0;i<nbEntry;i++)
	{		
		frameLock[i]=0;
		frameNum[i]=0xffff0000;	
	}
	return 1;

}
//_____________________________________________
ADMImage *VideoCache::getImage(uint32_t frame)
{
int32_t i;
uint32_t tryz=nbEntry;
uint32_t len,flags;
	// Already there ?
	if((i=searchFrame(frame))>=0)
	{
		aprintf("Cache : Cache hit %d buffer %d\n",frame,i);
		frameLock[i]++;
		return frameBuffer[i];	
	}
	// Else get it!
	
	// First elect a new buffer, we do it by 
	// using a simple scheme
	uint32_t count=0;
	aprintf("Cache : Cache miss %d\n",frame);
	//for(uint32_t i=0;i<nbEntry;i++) printf("%d(%d) ",frameNum[i],frameLock[i]);printf("\n");
	while(tryz)
	{
		if(!frameLock[(lastUsed+count)%nbEntry])
		{
			// for a candidate
			ADMImage *ptr;
			uint32_t target;
			
			target=(lastUsed+count)%nbEntry;
			ptr=frameBuffer[target];
			
			if(!incoming->getFrameNumberNoAlloc(frame,&len,ptr,&flags)) return NULL;
			lastUsed=(lastUsed+count+1)%nbEntry;
			frameLock[target]++;
			frameNum[target]=frame;			
			return ptr;
		}
		tryz--;
		count++;		
	}
	printf("Could not find empty slot in cache\n");
	for(uint32_t i=0;i<nbEntry;i++) printf("%d(%d) ",frameNum[i],frameLock[i]);printf("\n");
	ADM_assert(0);
}
