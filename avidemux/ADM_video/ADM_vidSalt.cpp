/***************************************************************************
                          ADM_vidSalt.cpp  -  description
                             -------------------
    begin                : Wed Jan 1 2003
    copyright            : (C) 2003 by mean
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
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidSalt.h"

extern uint8_t distMatrix[256][256];

BUILD_CREATE(salt_create,ADMVideoSalt);
//_______________________________________________________________


 char 								*ADMVideoSalt::printConf(void)
 {
	  	static char buf[50];
 	
 				sprintf((char *)buf," Stabilize :%ld",*_param);
        return buf;
	}
uint8_t  GUI_getIntegerValue(int *valye, int min, int max, char *title);	
uint8_t ADMVideoSalt::configure(AVDMGenericVideoStream *instream)
{
int i;
			_in=instream;
			i=(int)*_param;
			if (GUI_getIntegerValue(&i,1,255,"Stabilize Threshold"))
			{
					*_param=(uint32_t)i;
					return 1;
			}
			return 0;

}
//--------------------------------------------------------	
ADMVideoSalt::ADMVideoSalt(
									AVDMGenericVideoStream *in,CONFcouple *couples)
                  :ADMVideoCached(in,couples)
{
  //uint32_t frame;
  _uncompressed=NULL;
   
 
  _info.encoding=1;
  if(couples)
  {
			_param=NEW( uint32_t);
			couples->getCouple((char *)"threshol",_param);
	}
	else
	{
			_param=NEW( uint32_t);
			*_param=30;
	}
 
}

uint8_t 	ADMVideoSalt::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(1);


 		(*couples)->setCouple((char *)"threshold",*_param);

		return 1;

}
//                     1
//		Get in range in 121 + coeff matrix
//                     1
//
// If the value is too far away we ignore it
// else we blend

uint8_t ADMVideoSalt::getFrameNumberNoAlloc(uint32_t frame,
																	uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
UNUSED_ARG(flags);
uint32_t uvlen;
uint32_t dlen,dflags;

uint8_t										*_next;
uint8_t										*_previous;  	
uint8_t										*_current;
	//		printf("\n Stabilize : %lu\n",frame);

		
					
			
			uvlen=    _info.width*_info.height;
			*len=uvlen+(uvlen>>1);
		  if(frame> _info.nb_frames-1) return 0;
			if(!frame || (frame==_info.nb_frames-1))
			{
				 if(!_in->getFrameNumberNoAlloc(frame, &dlen,_buffer[0],&dflags))
				 {
					 	return 0;
					}
				 _bufnum[0]=frame;
					memcpy(data,_buffer[0],*len);
					return 1;
			}	 
				ADM_assert(frame<_info.nb_frames);   
			// read uncompressed frame
			switch(fillCache(frame))
			{
					case 0: return 0;
				  case 1: memcpy(data,_buffer[index_c],*len);return 1;break;
				  case 2: break;
				  default: ADM_assert(0); break;				
			}
   		_previous=_buffer[index_p];		
   		_current=_buffer[index_c];
   		_next=_buffer[index_n];          
           // for u & v , no action -> copy it as is
           memcpy(data+uvlen,_current+uvlen,uvlen>>1);
               							   				
           uint8_t *inprev,*innext,*incur,*zout;
              
              inprev=_previous+1+_info.width;
              innext=_next+1+_info.width;
              incur =_current+1+_info.width;
              
              zout=data+_info.width+1;
              
             
           
              
              uint16_t c,coeff;
              uint32_t x,total;
              for(uint32_t y= _info.height-1;y>1;y--)
              	{
								 							   
								   
				            for(x= _info.width-1;x>1;x--)
        			      	{
	          			       	c=*incur*4;;
	                    		coeff=4;
	                      	// first we average 4 points around
	                       total=(*innext+*inprev)>>1;
#define PONDERATE(x,p) if(distMatrix[*incur][x]<*_param)	                      \
{ c+=x;coeff++;}
													// means close enought
													*zout=*incur;
													if(distMatrix[*innext][*inprev]<16)
													{
															if(distMatrix[total][*incur]>*_param)
															{
																*zout=250; //total;
															}
													}
															
											
													//*zout=(uint8_t)floor(0.49+(c/coeff));													
													zout++;
													incur++;
													innext++;
													inprev++;	
																																		                  			  
											}
											zout+=2;
											incur+=2;
											innext+=2;
											inprev+=2;
								}
							return 1;
						
}
ADMVideoSalt::~ADMVideoSalt()
{
 
 	DELETE(_param);
}


#endif
