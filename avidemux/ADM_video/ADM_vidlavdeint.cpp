//
//
// C++ Implementation: ADM_vidlavdeint
//
// Description: 
//		Wrapper for libavcode deinterlacer
//
// Author: mean <fixounet@free.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include "ADM_lavcodec.h"

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidlavdeint.h"
//static void decimate(uint8_t *src,uint8_t *target, uint32_t linessrc, uint32_t width);
#include "ADM_filter/video_filters.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(lavdeint_script,AVDMVideoLavDeint,nullParam);
BUILD_CREATE(lavdeint_create,AVDMVideoLavDeint);


char *AVDMVideoLavDeint::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Lavcodec deinterlacer");
        return buf;
}

//_______________________________________________________________
AVDMVideoLavDeint::AVDMVideoLavDeint(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_uncompressed=new uint8_t[3*_info.width*_info.height];


}

// ___ destructor_____________
AVDMVideoLavDeint::~AVDMVideoLavDeint()
{
 	delete [] _uncompressed;

}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoLavDeint::getFrameNumberNoAlloc(uint32_t frame,
																		uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
//static Image in,out;
			if(frame>=_info.nb_frames) return 0;


			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

		// if not %4 -> skip
		if((_info.width&03) || (_info.height & 3))
		{
			printf("\n WIDTH & HEIGHT must be multiple of 4!!\n");
			memcpy(data,_uncompressed,(_info.width*_info.height*3)>>1);
			return 1;
		}
		// and deinterlace it
		AVPicture src;
  		AVPicture dest;
		uint32_t page=_info.width*_info.height;
		
		src.data[0]=_uncompressed;
		src.data[1]=_uncompressed+page;
		src.data[2]=_uncompressed+((page*5)>>2);
  
		dest.data[0]=data;
		dest.data[1]=data+page;
		dest.data[2]=data+((page*5)>>2);
		
		src.linesize[0]=dest.linesize[0]=_info.width;
		src.linesize[1]=dest.linesize[1]=_info.width>>1;
		src.linesize[2]=dest.linesize[2]=_info.width>>1;
  
		if (avpicture_deinterlace(&dest,&src,PIX_FMT_YUV420P,_info.width,_info.height)<0)
		{
			printf("Error in avpicture deinterlace!\n");
			return 0;
		} 		

      return 1;
}


