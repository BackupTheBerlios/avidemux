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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ADM_library/default.h"

#include "ADM_library/ADM_image.h"
#include "ADM_assert.h"
void ADMImage_stat( void );

static uint32_t imgMaxMem=0;
static uint32_t imgCurMem=0;
static uint32_t imgMaxNb=0;
static uint32_t imgCurNb=0;

void ADMImage_stat( void )
{
	printf("Images stat:");
	printf("___________");
	printf("Max memory consumed (MB)     : %lu\n",imgMaxMem>>10);
	printf("Current memory consumed (MB) : %lu\n",imgCurMem>>10);
	printf("Max image used               : %lu\n",imgMaxNb);
	printf("Cur image used               : %lu\n",imgCurNb);

}
//
//	Allocate and initialize everything to default values
//
ADMImage::ADMImage(uint32_t width, uint32_t height)
{
	_width=width;
	_height=height;
	data=new uint8_t [ ((width+15)&0xffffff0)*((height+15)&0xfffffff0)*2];
	ADM_assert(data);
	quant=NULL;
	_qStride=0;
	_Qp=2;
	flags=0;
	_qSize=0;
	_aspect=ADM_ASPECT_1_1;
	imgCurMem+=(width*height*3)>>1;
	imgCurNb++;
	
	if(imgCurMem>imgMaxMem) imgMaxMem=imgCurMem;
	if(imgCurNb>imgMaxNb) imgMaxNb=imgCurNb;
};
//
//	Deallocate
//
ADMImage::~ADMImage()
{
	if(quant) delete [] quant;
	quant=NULL;
	if(data) delete [] data;
	data=NULL;
	
	imgCurNb--;
	imgCurMem-=(_width*_height*3)>>1;
}

//
//	Duplicate
//
uint8_t ADMImage::duplicate(ADMImage *src)
{
	// Sanity check
	ADM_assert(src->_width==_width);
	ADM_assert(src->_height==_height);
	// cleanup if needed
	if(quant) delete [] quant;
	quant=NULL;
	_qStride=0;	
	_qSize=0;
	
	copyInfo(src);
	memcpy(YPLANE(this),YPLANE(src),_width*_height);
	memcpy(UPLANE(this),UPLANE(src),(_width*_height)>>2);
	memcpy(VPLANE(this),VPLANE(src),(_width*_height)>>2);
	return 1;
}
uint8_t ADMImage::duplicateFull(ADMImage *src)
{
	// Sanity check
	ADM_assert(src->_width==_width);
	ADM_assert(src->_height==_height);
	
	
	copyInfo(src);
	memcpy(YPLANE(this),YPLANE(src),_width*_height);
	memcpy(UPLANE(this),UPLANE(src),(_width*_height)>>2);
	memcpy(VPLANE(this),VPLANE(src),(_width*_height)>>2);
	copyQuantInfo(src);
	
	return 1;
}
uint8_t ADMImage::copyInfo(ADMImage *src)
{
	_Qp=src->_Qp;
	flags=src->flags;
	_aspect=src->_aspect;
}
//
//	By design the reallocation of quant should happen at startup
//	but not in processing itself (reuse already allocated quant)
//	It may seem shoddy, but it adds flexibility
//
uint8_t ADMImage::copyQuantInfo(ADMImage *src)
{
	if(!src->_qStride || !src->_qSize)  // No (usable) quant
	{
		_qStride=0;
		return 1;
	}
	// Reuse Quant memory ?
	if(!quant)
	{	// need a new quant
		quant=new uint8_t[src->_qSize];
		_qSize=src->_qSize;
	
	}
	
	// Same size ?
	ADM_assert(_qSize==src->_qSize);		
	_qStride=src->_qStride;		
	memcpy(quant,src->quant,_qSize);	
	
	return 1;
}





