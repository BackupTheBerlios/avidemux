/***************************************************************************
                          ADM_vidPartial.cpp  -  description
                             -------------------
    begin                : Mon Dec 30 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This filter is special
    It uses internally another filter from _start to _end
    and output copy of input else 
    
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
#include <assert.h>
#include <assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"

extern AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,CONFcouple *couple, AVDMGenericVideoStream *in);

BUILD_CREATE(partial_create,ADMVideoPartial);
//___________________________________________________
char 						*ADMVideoPartial::printConf(void)
{
	static char string[500];
		 			sprintf(string,"Partial from %ld to %ld : %s",
		    						_param->_start,
		          			_param->_end,
		             		_son->printConf());
		    return string;
}

ADMVideoPartial::ADMVideoPartial(   AVDMGenericVideoStream *in,
							CONFcouple		*couples)
{
		_param=NEW( PARTIAL_CONFIG);
		_param->_start=0;
		_param->_end=0;
		_in=in;

			GET(_start);
			GET(_end);
			GET(_tag);
			// we share the same parameters
			_son= filterCreateFromTag((VF_FILTERS)_param->_tag,couples,_in);
		 	memcpy(&_info,_in->getInfo(),sizeof(_info));
}

ADMVideoPartial::ADMVideoPartial(  AVDMGenericVideoStream *in,VF_FILTERS tag,CONFcouple *setup)
{
		_param=NEW( PARTIAL_CONFIG);
		_param->_start=0;
		_param->_end=0;
		_param->_tag=tag;
		_in=in;
		// we share the same parameters
		_son= filterCreateFromTag(tag,setup,_in);
		memcpy(&_info,_in->getInfo(),sizeof(_info));
}

/**/
uint8_t	ADMVideoPartial::getCoupledConf( CONFcouple **couples)
{
			uint32_t nbParam=0;

			assert(_param);

			// first we ask the child its config
			CONFcouple *child;
			if(!_son->getCoupledConf(&child))
			{
				// no param for child
#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
				*couples=new CONFcouple(3);
				CSET(_start);
				CSET(_end);
				CSET(_tag);
				return 1;
			}

			// grab child param
			nbParam=child->getNumber();

			*couples=new CONFcouple(3+nbParam);
			CSET(_start);
			CSET(_end);
			CSET(_tag);

	// then set the child ones
	char *nm,*vl;

	for(uint32_t i=0;i<nbParam;i++)
	{
		child->getEntry(i,&nm, &vl);
		  (*couples)->setCouple(nm,vl);
	}
	// delete child
	delete child;

	return 1;

}

//
ADMVideoPartial::~ADMVideoPartial()
{
		if(_son) delete _son;
		DELETE(_param);

}
//
uint8_t ADMVideoPartial::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
		assert(_son);
		if(frame>=_param->_start && frame<=_param->_end) // 
		{
				return _son->getFrameNumberNoAlloc(frame,len,data,flags);
		}
		return _in->getFrameNumberNoAlloc(frame,len,data,flags);	                           
}	      
//
uint8_t ADMVideoPartial::configure( AVDMGenericVideoStream *instream)
{
        UNUSED_ARG(instream);
		//uint8_t  GUI_getIntegerValue(int *valye, int min, int max, char *title)
		GUI_getIntegerValue((int *)&_param->_start, 0, 0xffff, "Start frame ?")    ;
		GUI_getIntegerValue((int *)&_param->_end, 0, 0xffff, "End frame ?") ;
		if(_param->_end<_param->_start) _param->_end=_param->_start;

	 return 1;
}  

 		                                                                         
#endif

