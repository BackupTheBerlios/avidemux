/***************************************************************************
                          ADM_vidNull.cpp  -  description
                             -------------------

	This filter asks an image from the editor and 
	clean-up the output
	It does compute the average quant possibly apply postprocessing
	
	

    begin                : Wed Mar 20 2002
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_filter/video_filters.h"

extern int DIA_getMPParams( int *pplevel, int *ppstrength,int *swap);

int filterDefaultPPType=7;
int filterDefaultPPStrength=5;
int filterDefaultPPSwap=0;
//
void	filterSetPostProc( void )
{
	if(DIA_getMPParams( &filterDefaultPPType, &filterDefaultPPStrength,&filterDefaultPPSwap))
	{
		getFirstVideoFilter();
	}

}
//_______________________________________________________________

AVDMVideoStreamNull::AVDMVideoStreamNull(ADM_Composer *in,uint32_t start, uint32_t nb)
{
aviInfo aviinf;
  	_in=in;	
  	
  	// now build infos
  	
  	ADM_assert(_in->getVideoInfo(&aviinf));
  	_info.width=aviinf.width;
  	_info.height=aviinf.height;
	if(start+nb==aviinf.nb_frames-1)  nb++; // take last frame
  	_info.nb_frames=nb;
	_start=start;
  	_info.encoding=0;
	_info.fps1000=aviinf.fps1000;
	_info.orgFrame=start;
//  	_uncompressed=(uint8_t *)malloc(3*aviinf.width*aviinf.height);
	_uncompressed=new ADMImage(aviinf.width,aviinf.height);

  	ADM_assert(_uncompressed);
	ADM_assert(start+nb<=aviinf.nb_frames);

	initPostProc(&_pp,_info.width,_info.height);
	_pp.postProcType=filterDefaultPPType;
	_pp.postProcStrength=filterDefaultPPStrength;
	_pp.forcedQuant=0;
	updatePostProc(&_pp);	    	
  	aprintf("\n Null stream initialized with start frame = %lu, nbframe=%lu \n",_start,nb);
  	
}
AVDMVideoStreamNull::~AVDMVideoStreamNull()
{

 	delete _uncompressed;	
	
}
//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoStreamNull::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
        UNUSED_ARG(len);
    	UNUSED_ARG(flags);
		if(!(frame<_info.nb_frames))
		{
				printf("\n going out of bounds! :%ld / %ld \n",frame,_info.nb_frames);
				return 0;
		}
		
			// read uncompressed frame

	// 1 get it
    	if(!_in->getUncompressedFrame(_start+frame,_uncompressed)     )
     			return 0;
	if(flags)
		*flags=_uncompressed->flags;
	// Quant table available ?
	if(_uncompressed->quant && _uncompressed->_qStride)
	{
		// Compute average quant
		// Optimize!
		int64_t sum=0;
		int type;
		
		for(uint32_t z=0;z<_uncompressed->_qSize;z++)
			sum+=_uncompressed->quant[z];
		sum+=(_uncompressed->_qSize-1);
		sum/=_uncompressed->_qSize;
		_uncompressed->_Qp=sum;
		//printf("Q:%d\n",_uncompressed->_Qp);
		#warning FIXME should be FF_I_TYPE/B/P
		if(_uncompressed->flags & AVI_KEY_FRAME) type=1;
			else if(_uncompressed->flags & AVI_B_FRAME) type=3;
				else type=2;
		if(_pp.postProcType && _pp.postProcStrength)
			{ 	// we do postproc !
				// keep
				uint8_t *oBuff[3],*iBuff[3];
				int	strideTab[3];
				int	strideTab2[3];

						iBuff[0]= YPLANE(_uncompressed);
		 				iBuff[1]= UPLANE(_uncompressed);
 		 				iBuff[2]= VPLANE(_uncompressed);
		 				oBuff[0]= YPLANE(data);
		 				oBuff[1]= UPLANE(data);
 		 				oBuff[2]= VPLANE(data);
        			
			            strideTab[0]=strideTab2[0]=_info.width;
				    strideTab[1]=strideTab2[1]=_info.width>>1;
				    strideTab[2]=strideTab2[2]=_info.width>>1;
            			
		 		   pp_postprocess(
		      			iBuff,
		        		strideTab,
		          		oBuff,
		         		strideTab2,
		      			_info.width,
		        		_info.height,
		          		(int8_t *)_uncompressed->quant,
		          		_uncompressed->_qStride,
		         		_pp.ppMode,
		          		_pp.ppContext,
		          		type);			// img type
				// update some infos
				data->copyInfo(_uncompressed);

			}
		else
			data->duplicate(_uncompressed);

		
	}
	else
	{
		data->duplicate(_uncompressed);
	}
        return 1;


}

uint8_t AVDMVideoStreamNull::configure( AVDMGenericVideoStream *instream){
	UNUSED_ARG(instream);
	return 0;
}
