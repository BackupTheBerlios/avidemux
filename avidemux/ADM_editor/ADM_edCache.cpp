//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "default.h"
#include "ADM_image.h"
#include "ADM_editor/ADM_edCache.h"
#include "ADM_assert.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_EDITOR
#include "ADM_osSupport/ADM_debug.h"
	
EditorCache::EditorCache(uint32_t size,uint32_t w, uint32_t h)
{
	_elem=new cacheElem[size];
	for(uint32_t i=0;i<size;i++)
	{
		_elem[i].image=new ADMImage(w,h);
		
// 		_elem[i].image->_qSize= ((w+15)>>4)*((h+15)>>4);
// 		_elem[i].image->quant=new uint8_t[_elem[i].image->_qSize];
// 		_elem[i].image->_qStride=(w+15)>>4;
		
		_elem[i].frameNum=0x0fffffff;
		_elem[i].lastUse=0x0ffffff;
	}
	_counter=0;
	_nbImage=size;
}
EditorCache::~EditorCache(void)
{
	for(uint32_t i=0;i<_nbImage;i++)
	{
		delete _elem[i].image;
	}
	delete[] _elem;

}
ADMImage 	*EditorCache::getImage(uint32_t no)
{
	for(uint32_t i=0;i<_nbImage;i++)
	{
		if(_elem[i].frameNum==no)
		{
			aprintf("EdCache: %lu  in cache %lu\n",no,i);
			_elem[i].lastUse=_counter;
			_counter++;
			 return _elem[i].image;
		}
	}
// 	aprintf("EdCache: %lu not in cache\n",no);
	return NULL;
}
// Get the LRUed iimage in the cache
// the cache is big enough to be immune
// to reuse in the same go
ADMImage	*EditorCache::getFreeImage(void)
{
	uint32_t min=0;
	uint32_t target=0;
	uint64_t  delta=0xffffff;
	for(uint32_t i=0;i<_nbImage;i++)
	{
		delta=abs((int)_counter-(int)_elem[i].lastUse);
		if(delta>min)
		{
			min=delta;
			target=i;
			
		}
	}
	_elem[target].lastUse=_counter+1000;;
	_elem[target].frameNum=0xffffffff;
	
	return _elem[target].image;

}
uint8_t		EditorCache::updateFrameNum(ADMImage *image,uint32_t frameno)
{
	for(uint32_t i=0;i<_nbImage;i++)
	{
		if(_elem[i].image==image)
		{
			ADM_assert(_elem[i].frameNum==0xffffffff);
			_elem[i].frameNum=frameno;
			_elem[i].lastUse=_counter;
			_counter++;
			return 1;
		
		}
	
	}
	ADM_assert(0);

}
void EditorCache::dump( void)
{
	for(int i=0;i<_nbImage;i++)
	{
		aprintf("Edcache content:%d %lu\n",i,_elem[i].frameNum);
	}
}
