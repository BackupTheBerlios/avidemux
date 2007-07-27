/***************************************************************************
      Front end for bitmap sources (vobsub, ts, sup,...)
      (C) 2007 Mean
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
 #include "ADM_assert.h"
 #include "default.h"

 #include "ADM_editor/ADM_edit.hxx"
 #include "ADM_video/ADM_genvideo.hxx"

 #include "ADM_ocr.h"
 #include "ADM_ocrInternal.h"
 #include "ADM_toolkit/toolkit.hxx"
 #include "ADM_userInterfaces/ADM_commonUI/DIA_enter.h"
 #include "ADM_videoFilter/ADM_vidDvbSub.h"

/*  *****************************
 * 	**** Case 1 : Vobsub ********
 *  *****************************
 */

class ADM_BitmapSource_VobSub : public ADM_BitmapSource
{
protected:
			ADMVideoVobSub *_vobsub;
			uint32_t		_nbPics;
public: 
								ADM_BitmapSource_VobSub(void);
			virtual uint8_t     init(ADM_OCR_SOURCE *source);
			virtual 			~ADM_BitmapSource_VobSub();
			virtual uint32_t 	getNbImages(void);
			virtual vobSubBitmap *getBitmap(uint32_t nb,uint32_t *start, uint32_t *end,uint32_t *first,uint32_t *last);
};

ADM_BitmapSource_VobSub::ADM_BitmapSource_VobSub(void)
{
	_vobsub=NULL;
	_nbPics=0;
}
ADM_BitmapSource_VobSub::~ADM_BitmapSource_VobSub()
{
	if(_vobsub) delete _vobsub;
	_vobsub=NULL;
}
uint8_t     ADM_BitmapSource_VobSub::init(ADM_OCR_SOURCE *source)
{
	_vobsub=new ADMVideoVobSub(source->subparam->subname,source->subparam->index);
	if(!_vobsub)
	{
		printf("[BitmapSource_VS] Cannot open source\n");
		return 0;
	}
	_nbPics=_vobsub->getNbImage();
	return 1;
}
uint32_t 	ADM_BitmapSource_VobSub::getNbImages(void)
{
	return _nbPics;
}
/**
 * 		\fn getBitmap
 * 		\brief returns a bitmap in black & white (luma+alpha)
 * */
vobSubBitmap *ADM_BitmapSource_VobSub::getBitmap(uint32_t nb,uint32_t *start, uint32_t *end,uint32_t *first,uint32_t *last)
{
	return _vobsub->getBitmap(nb,start, end,first,last);
}
/*  *****************************
 * 	**** Case 2 : TsSub  ********
 *  *****************************
 */
#define TS_WIDTH  720
#define TS_HEIGHT 576
class ADM_BitmapSource_TsSub : public ADM_BitmapSource
{
protected:
			uint32_t		_nbPics;
			ADMVideoSubDVB  *_dvb;
			vobSubBitmap    *_bitmap;
public: 
								ADM_BitmapSource_TsSub(void);
			virtual uint8_t     init(ADM_OCR_SOURCE *source);
			virtual 			~ADM_BitmapSource_TsSub();
			virtual uint32_t 	getNbImages(void);
			virtual vobSubBitmap *getBitmap(uint32_t nb,uint32_t *start, uint32_t *end,uint32_t *first,uint32_t *last);
};

ADM_BitmapSource_TsSub::ADM_BitmapSource_TsSub(void)
{
	_nbPics=0x7FFFF;
	_dvb=NULL;
	_bitmap=new vobSubBitmap(TS_WIDTH,TS_HEIGHT);
}
ADM_BitmapSource_TsSub::~ADM_BitmapSource_TsSub()
{
	if(_dvb) delete _dvb;
	_dvb=NULL;
	if(_bitmap) delete _bitmap;
	_bitmap=NULL;
}
uint8_t     ADM_BitmapSource_TsSub::init(ADM_OCR_SOURCE *source)
{
	_dvb=new ADMVideoSubDVB("/capture/grey/Grey_anatomy_2007_05_22_Avec_Le_Temp.mpg",0x96,TS_WIDTH,TS_HEIGHT);
	
	ADM_assert(_dvb);
	return _dvb->init("/capture/grey/Grey_anatomy_2007_05_22_Avec_Le_Temp.mpg");
}
uint32_t 	ADM_BitmapSource_TsSub::getNbImages(void)
{
	return _nbPics;
}
/**
 * 		\fn getBitmap
 * 		\brief returns a bitmap in black & white (luma+alpha)
 *      @param nb : unused
 *      @start : Timecode of start (ms)
 * 		@end   : Timecode of end (ms)
 * 		@first : First non empty line
 * 		@last  : Last non empty line
 * */
vobSubBitmap *ADM_BitmapSource_TsSub::getBitmap(uint32_t nb,uint32_t *start, uint32_t *end,uint32_t *first,uint32_t *last)
{
	uint32_t pts;
	// First get our bitmap
	  if(! _dvb->getNextBitmap(_bitmap,&pts))
	  {
		  printf("[BitmapSource] cannot get next bitmap\n");
		  return NULL;
	  }
	  // Now build it
	  // First & last are the 1st and last non empty lines
	  *first=0;
	  *last=TS_HEIGHT-1;
	  
	  // Lookup the actual beginning /end
	  uint32_t top=0;
	   while(top<TS_HEIGHT && !_bitmap->isDirty(top) ) top++;
	        
	        if(top==TS_HEIGHT)
	        {
	                printf("[BitmapSource]Empty bitmap\n");
	                return NULL; 
	        }
	  // 
	        *first=top;
	        
	  // Now bottom
	        uint32_t bottom=TS_HEIGHT-1;
	        while(bottom>top && !_bitmap->isDirty(bottom) ) bottom--;
	        if(bottom==top)
	        {
	        	   printf("[BitmapSource]Empty bitmap\n");
	        	   return NULL; 
	        }
	        *last=bottom;
	        
	  // start & end are timecodes
	  *start=pts/90;
	  *end=*start+1000;
			  
	  // Set alpha as luma
	  uint8_t *in,*out;
	  in=_bitmap->_bitmap;
	  out=_bitmap->_alphaMask;
	  for(uint32_t yy=0;yy<TS_HEIGHT;yy++)
		  for(uint32_t xx=0;xx<TS_WIDTH;xx++)
		  {
			  if(*in) *out=255;
			  else *out=0;
			  
			  in++;
			  out++;
			  
		  }
		  
	return _bitmap;
}
//**********************************************************
//**********************************************************
//**********************************************************
/**
 * 	\fn ADM_buildBitmapSource
 * \brief Instantiate the correct bitmap source
*/
ADM_BitmapSource *ADM_buildBitmapSource(ADM_OCR_SOURCE *source)
{
	switch(source->type)
	{
			case ADM_OCR_TYPE_VOBSUB: 	return new ADM_BitmapSource_VobSub();
			case ADM_OCR_TYPE_TS: 		return new ADM_BitmapSource_TsSub();
			default:
						printf("[BitmapSource] Unknown source type %d\n",source->type);
						ADM_assert(0);
	}
	return NULL;
}
//EOF
