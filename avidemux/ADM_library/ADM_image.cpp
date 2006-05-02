/***************************************************************************
    copyright            : (C) 2003-2005 by mean
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
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ADM_assert.h"
#include "ADM_library/default.h"

#include "ADM_library/ADM_image.h"
#include "ADM_toolkit/ADM_cpuCap.h"
//#include "ADM_assert.h"
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
void    ADMImage::commonInit(uint32_t w,uint32_t h)
{
        _width=w;        
        _height=h;
        quant=NULL;
        _qStride=0;
        _Qp=2;
        flags=0;
        _qSize=0;
        _aspect=ADM_ASPECT_1_1;
        
        imgCurNb++;
        _planes[0]=_planes[1]=_planes[2]=NULL;
        _noPicture=0;
        _colorspace=ADM_COLOR_YV12;

}
ADMImage::ADMImage(uint32_t width, uint32_t height)
{
        commonInit(width,height);
        _isRef=0;
        data=new uint8_t [ ((width+15)&0xffffff0)*((height+15)&0xfffffff0)*2];
        ADM_assert(data);
        imgCurMem+=(width*height*3)>>1;
        if(imgCurMem>imgMaxMem) imgMaxMem=imgCurMem;
        if(imgCurNb>imgMaxNb) imgMaxNb=imgCurNb;

};
// Create a fake image with external datas
ADMImage::ADMImage(uint32_t width, uint32_t height,uint32_t dummy)
{       
        commonInit(width,height); 
        _isRef=1;
        data=NULL;                
                
};
//
//	Deallocate
//
ADMImage::~ADMImage()
{
        if(!_isRef)
        {
	       if(quant) delete [] quant;
	       quant=NULL;
	       if(data) delete [] data;
	       data=NULL;
               imgCurMem-=(_width*_height*3)>>1;
	}
	imgCurNb--;
	
}

//
//	Duplicate
//
uint8_t ADMImage::duplicateMacro(ADMImage *src,uint32_t swap)
{
        // Sanity check
        ADM_assert(src->_width==_width);
        ADM_assert(src->_height==_height);

        ADM_assert(!_isRef); // could not duplicate to a linked data image

        // cleanup if needed
        if(quant) delete [] quant;
        quant=NULL;
 
       _qStride=0;      
        _qSize=0;
        
        copyInfo(src);
        if(!src->_isRef)
        {
               memcpy(YPLANE(this),YPLANE(src),_width*_height);
                if(swap)
                {
                        memcpy(UPLANE(this),VPLANE(src),(_width*_height)>>2);
                        memcpy(VPLANE(this),UPLANE(src),(_width*_height)>>2);
                }
                else
                {
                        memcpy(UPLANE(this),UPLANE(src),(_width*_height)>>2);
                        memcpy(VPLANE(this),VPLANE(src),(_width*_height)>>2);
                }
        }
        else
        {
                if(src->_noPicture)
                {
                        // no pic available, blacken it                        
                        memset(YPLANE(this),0,_width*_height);
                        memset(UPLANE(this),128,(_width*_height)>>2);
                        memset(VPLANE(this),128,(_width*_height)>>2);
                        return 1;
                        
                }
               // The source is a reference
                // We have to use the alternate informations
                // to copy & compact at the same time
                //

                ADM_assert(src->_planeStride[0]);
                ADM_assert(src->_planeStride[1]);
                ADM_assert(src->_planeStride[2]);

                ADM_assert(src->_planes[0]);
                ADM_assert(src->_planes[1]);
                ADM_assert(src->_planes[2]);

                uint8_t *in,*out;
                uint32_t w,h,stride;

                w=src->_width;
                h=src->_height;
                stride=src->_planeStride[0];
                out=YPLANE(this);
                in=src->_planes[0];
#define PLANE_CPY(h) \
                for(uint32_t y=0;y<h;y++)  \
                { \
                        memcpy(out,in,w); \
                        in+=stride;       \
                        out+=w;           \
                } 
                PLANE_CPY(h);

                w>>=1;
                h>>=1;
                in=src->_planes[1];
                if(swap)
                        out=VPLANE(this);
                else
                        out=UPLANE(this);
                stride=src->_planeStride[1];
                PLANE_CPY(h);
        
                in=src->_planes[2];
                if(swap)
                        out=UPLANE(this);
                else
                        out=VPLANE(this);
                stride=src->_planeStride[2];
                PLANE_CPY(h);


        }
        return 1;
}
uint8_t ADMImage::duplicate(ADMImage *src)
{
	return duplicateMacro(src,0);
}
uint8_t ADMImage::duplicateSwapUV(ADMImage *src)
{
        return duplicateMacro(src,0);
}

uint8_t ADMImage::duplicateFull(ADMImage *src)
{
	// Sanity check
	ADM_assert(src->_width==_width);
	ADM_assert(src->_height==_height);
	
	
	copyInfo(src);
        ADM_assert(!_isRef);

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
uint8_t ADMImage::blacken(void)
{
        memset(YPLANE(this),0,_width*_height);
        memset(UPLANE(this),128,(_width*_height)>>2);
        memset(VPLANE(this),128,(_width*_height)>>2);
        return 1;

}
/*
    Copy "this" image into dest image at x,y position

*/
uint8_t ADMImage::copyTo(ADMImage *dest, uint32_t x,uint32_t y)
{
      
    uint32_t box_w=_width, box_h=_height;
    // Clip if needed
    if(y>dest->_height)
    {
        printf("Y out : %u %u\n",y,dest->_height);
         return 1;
    }
    if(x>dest->_width) 
    {
        printf("X out : %u %u\n",x,dest->_width);
         return 1;
    }
    
    if(x+box_w>dest->_width) box_w=dest->_width-x;
    if(y+box_h>dest->_height) box_h=dest->_height-y;

    // do y
    BitBlit(YPLANE(dest)+x+dest->_width*y,dest->_width,
            data,_width,
            box_w,box_h);
    // Do u
    BitBlit(UPLANE(dest)+x/2+(dest->_width*y)/4,dest->_width/2,
            UPLANE(this),_width>>1,
            box_w>>1,box_h>>1);

    BitBlit(VPLANE(dest)+x/2+(dest->_width*y)/4,dest->_width/2,
            VPLANE(this),_width>>1,
            box_w>>1,box_h>>1);


    return 1;

}
uint8_t BitBlit(uint8_t *dst, uint32_t pitchDst,uint8_t *src,uint32_t pitchSrc,uint32_t width, uint32_t height)
{
    
    for(int y=0;y<height;y++)
    {
        memcpy(dst,src,width);
        src+=pitchSrc;
        dst+=pitchDst;   
    }
    return 1;
}
//EOF
