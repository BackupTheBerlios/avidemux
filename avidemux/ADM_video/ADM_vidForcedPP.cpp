//
// C++ Implementation: ADM_vidForcedPP
//
// Description: 
//
//	Force postprocessing assuming constant quant & image type
//	Uselefull on some badly authored DVD for example
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <math.h>
#include <math.h>
#include <iconv.h>
#include "config.h"


#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"



#include "ADM_vidForcedPP.h"
#include "ADM_dialog/DIA_enter.h"

BUILD_CREATE(forcedpp_create,ADMVideoForcedPP);

extern int DIA_getMPParams( int *pplevel, int *ppstrength,int *swap);

uint8_t ADMVideoForcedPP::configure( AVDMGenericVideoStream *instream)
{
	_in=instream;
	int postproc, strength,uv;
	
		postproc=_param->postProcType;
		strength=_param->postProcStrength;
		uv=0;
		if( DIA_getMPParams( &postproc,&strength,&uv))
		{
				_param->postProcType=postproc;
				_param->postProcStrength=strength;				
				uv=_param->forcedQuant;
				if( DIA_GetIntegerValue(&uv,2,31, "Quant Value","Enter forced Q:"))
				{
					_param->forcedQuant=uv;
				}

				updatePostProc(&_postproc );				

		}
	return 1;	
 	
}
uint8_t	ADMVideoForcedPP::getCoupledConf( CONFcouple **couples)
{

			*couples=new CONFcouple(3);

			CSET(postProcType);
			CSET(postProcStrength);
			CSET(forcedQuant);

		return 1;	
}
char *ADMVideoForcedPP::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," Forced pp : %lu %lu",_param->postProcType,_param->postProcStrength);
        return buf;
}

ADMVideoForcedPP::~ADMVideoForcedPP()
{
	if(_uncompressed)
 		delete []_uncompressed;
	if(_postproc.ppContext)
		deletePostProc(&_postproc);

 	_uncompressed=NULL;
}


 ADMVideoForcedPP::ADMVideoForcedPP( AVDMGenericVideoStream *in,CONFcouple *couples)
{
		printf("Adding forced PP filter\n");
		_uncompressed=NULL;
		_in=in;
		assert(in);
		if(!couples)
		{
			
			_param=NEW(PP_CONF);
	    		_param->postProcType=5; // Bff=0 / 1=tff
	    		_param->postProcStrength=5;
			_param->forcedQuant=10;
			
	    	}
		else
		{
			_param=NEW(PP_CONF);
			GET(postProcType);
			GET(postProcStrength);
			GET(forcedQuant);
			
						
		}
		assert(in);
		
		memcpy(&_info,_in->getInfo(),sizeof(_info));	
		
	    	initPostProc(&_postproc,_info.width,_info.height);
		_postproc.postProcType=_param->postProcType;
		_postproc.postProcStrength=_param->postProcStrength;
		_postproc.forcedQuant=_param->forcedQuant;
		updatePostProc(&_postproc);	    	
	    	
	    	_uncompressed=new uint8_t[3*_info.width*_info.height];	
		

}
uint8_t ADMVideoForcedPP::getFrameNumberNoAlloc(uint32_t frame,
							uint32_t *len,
							uint8_t *data,
							uint32_t *flags)
{
		uint32_t page=_info.width*_info.height;
		
		*len=(page*3)>>1;
		if(frame>=_info.nb_frames) return 0;		
		
		if(_postproc.postProcType && _postproc.postProcStrength)
			{ 	// we do postproc !
				// keep
				uint8_t *iBuff[3],*oBuff[3];
				int strideTab[3],strideTab2[3];			
				
				
				if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags))
				 		return 0;

		 		oBuff[0]=data;
		 		oBuff[1]=data+page;
 		 		oBuff[2]=oBuff[1]+(page>>2);
				
				iBuff[0]=_uncompressed;
		 		iBuff[1]=_uncompressed+page;
 		 		iBuff[2]=iBuff[1]+(page>>2);
				
				
		            	strideTab[0]=strideTab2[0]=_info.width;
				strideTab[1]=strideTab2[1]=_info.width>>1;
				strideTab[2]=strideTab2[2]=_info.width>>1;
/*
void  pp_postprocess(uint8_t * src[3], int srcStride[3],
                 uint8_t * dst[3], int dstStride[3],
                 int horizontalSize, int verticalSize,
                 QP_STORE_T *QP_store,  int QP_stride,
		 pp_mode_t *mode, pp_context_t *ppContext, int pict_type);

*/				
		 		pp_postprocess(
		      			iBuff,
		        		strideTab,
		          		oBuff,
		         		strideTab2,
		      			_info.width,
		        		_info.height,
		          		NULL,
		          		0,
		         		_postproc.ppMode,
		          		_postproc.ppContext,
		          		1); // I ?
			  	
			}
		else
		{
			if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;
			return 1;
		
		}			
		
		
	return 1;
}

