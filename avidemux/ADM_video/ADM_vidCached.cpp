/***************************************************************************
                          ADM_vidCached.cpp  -  description
                             -------------------
    begin                : Tue Dec 31 2002
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
#include <assert.h>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCached.h"


		  
ADMVideoCached::ADMVideoCached(AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
uint32_t frame;
 	_in=in;		
 	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
  
  frame=3*_in->getInfo()->width*_in->getInfo()->height;					
												
  for(uint32_t u=0;u<3;u++)
  {
  		_buffer[u]=new uint8_t [frame];
    	assert(_buffer[u]);
  }
 	_bufnum[0]=0xefffffff;
 	_bufnum[1]=0xefffffff;
 	_bufnum[2]=0xefffffff;
  	
}
uint8_t ADMVideoCached::configure(AVDMGenericVideoStream *in)
{
      UNUSED_ARG(in);
	  return 0;
	
}
ADMVideoCached::~ADMVideoCached()
{
	for(uint32_t u=0;u<3;u++)
  {
			delete [] _buffer[u];
	}
}

char	*ADMVideoCached::printConf( void) 
{
	assert(0);
	return NULL;
}
uint8_t ADMVideoCached::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
    UNUSED_ARG(frame);
    UNUSED_ARG(len);
    UNUSED_ARG(data);
    UNUSED_ARG(flags);
	assert(0);
	return 0;
}
int32_t ADMVideoCached::searchIndex(uint32_t frame)
{
		for(int32_t i=0;i<3;i++)
		{
				if(_bufnum[i]==frame) return i;
		}
		return -1;
	
}
//
//	0 -> Error
//  1 -> ok but abort now
//  2 -> ok continue
uint8_t  ADMVideoCached::fillCache(uint32_t frame)
{
uint32_t dflags,dlen;

			index_c=searchIndex(frame);
			index_p=searchIndex(frame-1);
			
			
			
			// case 1 : we got both of them
			if(index_c>=0 && index_p>=0)
			{	// search next one
					uint8_t mask=2+4+8;
					mask^=(2<<index_c)+(2<<index_p);
					switch(mask)
						{
							case 2: index_n=0;break;
							case 4: index_n=1;break;
							case 8: index_n=2;break;
							default:
											assert(0);														
						}
						_bufnum[index_n]=frame+1;
						if(!_in->getFrameNumberNoAlloc(frame+1, &dlen,_buffer[index_n],
															&dflags))
						{
								
								return 1;
						}		
						return 2;							
			}
		 	
		if(index_p>=0)   // we got only the previous
			{
						index_c=index_p+1;
						index_n=index_p+2;
						index_c=index_c%3;
						index_n=index_n%3;						
						// we got only previous
						// fetch cur & next
			      if(!_in->getFrameNumberNoAlloc(frame, &dlen,_buffer[index_c],
															&dflags))
						{
								return 0;
						}				
						_bufnum[index_c]=frame;				
						if(!_in->getFrameNumberNoAlloc(frame+1, &dlen,_buffer[index_n],
															&dflags))
						{
								
								return 1;
						}								
            _bufnum[index_n]=frame+1;		
            return 2;
			}
			
		// we got none, fetch them
		// first we try the previous for speed up reasons
		index_p=0;
		index_c=1;
		index_n=2;
		
		 if(!_in->getFrameNumberNoAlloc(frame-1, &dlen,_buffer[index_p],
														&dflags))
				{
						return 0;
				}		
		 _bufnum[index_p]=frame-1;
						 	
			if(!_in->getFrameNumberNoAlloc(frame, &dlen,_buffer[index_c],
															&dflags))
					{
						return 0;
					}		
				_bufnum[index_c]=frame;
		
						
			if(!_in->getFrameNumberNoAlloc(frame+1, &dlen,_buffer[index_n],
															&dflags))
						{
								return 1;
						}		
						_bufnum[index_n]=frame+1;
				       
	  return 2;
}
//


