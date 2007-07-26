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

class ADM_BitmapSource_TsSub : public ADM_BitmapSource
{
protected:
			uint32_t		_nbPics;
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
}
ADM_BitmapSource_TsSub::~ADM_BitmapSource_TsSub()
{

}
uint8_t     ADM_BitmapSource_TsSub::init(ADM_OCR_SOURCE *source)
{
	
		return 0;
}
uint32_t 	ADM_BitmapSource_TsSub::getNbImages(void)
{
	return _nbPics;
}
/**
 * 		\fn getBitmap
 * 		\brief returns a bitmap in black & white (luma+alpha)
 * */
vobSubBitmap *ADM_BitmapSource_TsSub::getBitmap(uint32_t nb,uint32_t *start, uint32_t *end,uint32_t *first,uint32_t *last)
{
	return NULL;
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
