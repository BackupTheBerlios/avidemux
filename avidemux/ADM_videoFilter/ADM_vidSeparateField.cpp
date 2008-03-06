/***************************************************************************
                          Separate Fields.cpp  -  description
                             -------------------
Convert a x*y * f fps video into -> x*(y/2)*fps/2 video

Same idea as for avisynth separatefield


    begin                : Thu Mar 21 2002
    copyright            : (C) 2002 by mean
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

#include "ADM_default.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFieldUtil.h"
#include "ADM_vidSeparateField.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM swapParam={0,{""}};

SCRIPT_CREATE(separatefield_script,AVDMVideoSeparateField,swapParam);
SCRIPT_CREATE(mergefield_script,AVDMVideoMergeField,swapParam);
SCRIPT_CREATE(stackfield_script,AVDMVideoStackField,swapParam);
SCRIPT_CREATE(hzstackfield_script,AVDMVideoHzStackField,swapParam);
SCRIPT_CREATE(unstackfield_script,AVDMVideoUnStackField,swapParam);

BUILD_CREATE(separatefield_create,AVDMVideoSeparateField);
BUILD_CREATE(mergefield_create,AVDMVideoMergeField);
BUILD_CREATE(hzstackfield_create,AVDMVideoHzStackField);
BUILD_CREATE(stackfield_create,AVDMVideoStackField);
BUILD_CREATE(unstackfield_create,AVDMVideoUnStackField);

char *AVDMVideoSeparateField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Separate Fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoSeparateField::AVDMVideoSeparateField(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	

	_info.height>>=1;
	_info.fps1000*=2;
	_info.nb_frames*=2;
	vidCache=new VideoCache(2,_in);

}

// ___ destructor_____________
AVDMVideoSeparateField::~AVDMVideoSeparateField()
{
 	
	delete vidCache;
	vidCache=NULL;
}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoSeparateField::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t ref;
ADMImage *ptr;
		if(frame>=_info.nb_frames) return 0;
		ref=frame>>1;
		
		ptr=vidCache->getImage(ref);
		if(!ptr) return 0;

		ADM_assert(ptr->data);
		ADM_assert(data->data);
		if(frame&1) // odd image
			 vidFieldKeepOdd(_info.width,_info.height,ptr->data,data->data);
		else
			 vidFieldKeepEven(_info.width,_info.height,ptr->data,data->data);
		data->copyInfo(ptr);	
		vidCache->unlockAll();
      return 1;
}
//------------------ and merge them ------------------


char *AVDMVideoMergeField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Merge fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoMergeField::AVDMVideoMergeField(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));	
	vidCache=new VideoCache(4,_in);
	

	_info.height<<=1;
	_info.fps1000>>=1;
	_info.nb_frames>>=1;


}

// ___ destructor_____________
AVDMVideoMergeField::~AVDMVideoMergeField()
{
 		delete vidCache;
		vidCache=NULL;
}

/**
	Interleave frame*2 and frame*2+1
*/
uint8_t AVDMVideoMergeField::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t ref,ref2;
ADMImage *ptr1,*ptr2;
		if(frame>=_info.nb_frames) return 0;

		ref=frame<<1;
		ref2=ref+1;
		ptr1=vidCache->getImage(ref);
		ptr2=vidCache->getImage(ref+1);
		
		if(!ptr1 || !ptr2)
		{
			printf("Merge field : cannot read\n");
			vidCache->unlockAll();
		 	return 0;
		}
		 vidFieldMerge(_info.width,_info.height,ptr1->data,ptr2->data,data->data);
		 vidCache->unlockAll();
		
      return 1;
}
//_______________________Stack Fields_______________________

char *AVDMVideoStackField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Stack fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoStackField::AVDMVideoStackField(	AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));	
	_uncompressed=new ADMImage(_info.width,_info.height);	

}

// ___ destructor_____________
AVDMVideoStackField::~AVDMVideoStackField()
{
 		delete _uncompressed;
		_uncompressed=NULL;
}

/**
	Interleave frame*2 and frame*2+1
*/
uint8_t AVDMVideoStackField::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t ref,ref2;
ADMImage *ptr1,*ptr2;
		if(frame>=_info.nb_frames) return 0;

		 if(!_in->getFrameNumberNoAlloc(frame, len, _uncompressed, flags)) return 0;
		 
		  vidFielStack(_info.width ,_info.height,YPLANE(_uncompressed),YPLANE(data));
		data->copyInfo(_uncompressed);	
      return 1;
}

/****/

//*************************************************************
//_______________________Hz Stack Fields_______________________

char *AVDMVideoHzStackField::printConf( void )
{
        static char buf[50];
        sprintf((char *)buf," Hz Stack fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoHzStackField::AVDMVideoHzStackField(	AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
        _in=in;
        memcpy(&_info,_in->getInfo(),sizeof(_info));
        _info.width<<=1;
        _info.height>>=1;
        _uncompressed=new ADMImage(in->getInfo()->width,in->getInfo()->height);	
}

// ___ destructor_____________
AVDMVideoHzStackField::~AVDMVideoHzStackField()
{
        delete _uncompressed;
        _uncompressed=NULL;
}

/*
        Put fields side by side	
*/
static void fCopy(uint8_t *d, uint8_t *sr, uint32_t w, uint32_t sstride,uint32_t dstride,uint32_t h)
{

}
uint8_t AVDMVideoHzStackField::getFrameNumberNoAlloc(uint32_t frame,
                            uint32_t *len,
                            ADMImage *data,
                            uint32_t *flags)
{
uint32_t ref,ref2;
ADMImage *ptr1,*ptr2;
        if(frame>=_info.nb_frames) return 0;
        if(!_in->getFrameNumberNoAlloc(frame, len, _uncompressed, flags)) return 0;

        uint32_t pg=_info.width*_info.height;
        // Duplicate _uncompressed
        memcpy(YPLANE(data),YPLANE(_uncompressed),pg);
        memcpy(UPLANE(data),UPLANE(_uncompressed),pg>>2);
        memcpy(VPLANE(data),VPLANE(_uncompressed),pg>>2);


        return 1;
}

/****/
//_______________________UnStack Fields_______________________

char *AVDMVideoUnStackField::printConf( void )
{
        static char buf[50];

        sprintf((char *)buf," UnStack fields");
        return buf;
}

//______________________Unstack Fields_________________________________________
AVDMVideoUnStackField::AVDMVideoUnStackField(       AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
        _in=in;
        memcpy(&_info,_in->getInfo(),sizeof(_info));    
        _uncompressed=new ADMImage(_info.width,_info.height);   

}

// ___ destructor_____________
AVDMVideoUnStackField::~AVDMVideoUnStackField()
{
                delete _uncompressed;
                _uncompressed=NULL;
}
//*************************************************************
/**
        Interleave frame*2 and frame*2+1
*/
uint8_t AVDMVideoUnStackField::getFrameNumberNoAlloc(uint32_t frame,
                                uint32_t *len,
                                ADMImage *data,
                                uint32_t *flags)
{
uint32_t ref,ref2;
ADMImage *ptr1,*ptr2;
                if(frame>=_info.nb_frames) return 0;

                 if(!_in->getFrameNumberNoAlloc(frame, len, _uncompressed, flags)) return 0;
                 
                  vidFielUnStack(_info.width ,_info.height,YPLANE(_uncompressed),YPLANE(data));
                data->copyInfo(_uncompressed);  
      return 1;
}
