/***************************************************************************
                          ADM_vidTempSmooth.cpp  -  description
                             -------------------

	Temporal smoother
	Adapted from avisynth
	
	For each pixel we check toward radius previous image if
			u,v are within chroma_threshold
			y   is within luma_threshold

	if so, we blend them with the average value
	
	If more than 50% of pixels differs, it is probably a scene change
	we refill the "previous" screens with present screen and send it as is.

	That avoid blending when there is fade to black or whatever

	For the moment, only u & v are used. Y will follow later.
	A radius of 6 seems good.


    begin                : Sun Jul 28 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/
#if 0
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
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidTempSmooth.h"
BUILD_CREATE(tempsmooth_create,AVDMVideoTempSmooth);
 char *AVDMVideoTempSmooth::printConf(void)
{
static char str[100];
		sprintf(str,"Temp Smooth : R = %02lu, chroma=%01lu,luma %01lu",_param->radius,
					_param->luma_threshold,_param->chroma_threshold);
 		return str;
}

uint8_t AVDMVideoTempSmooth::configure( AVDMGenericVideoStream *instream)
{
    UNUSED_ARG(instream);
 	return 0;
}

//_______________________________________________________________

AVDMVideoTempSmooth::AVDMVideoTempSmooth(
									AVDMGenericVideoStream *in,CONFcouple *couples)
{

   	 _ptr_to_screen=NULL;
	_screen=NULL;
  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
	if(couples)
	{
		_param=NEW(TEMPSMOOTH_PARAMS);
		GET(radius);
		GET(chroma_threshold);
		GET(luma_threshold);
	}
    else
    		{
         			_param=NEW( TEMPSMOOTH_PARAMS);
            		_param->radius=7;
              	_param->luma_threshold=16;
              	_param->chroma_threshold=12;


        }

					
  _info.encoding=1;

  	  	
}

uint8_t	AVDMVideoTempSmooth::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(3);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(radius);
	CSET(chroma_threshold);
	CSET(luma_threshold);

			return 1;

}
AVDMVideoTempSmooth::~AVDMVideoTempSmooth()
{
DELETE(_param);
	if(  _ptr_to_screen) free( _ptr_to_screen);
	if(_screen) delete [] _screen;
 	
}

uint8_t AVDMVideoTempSmooth::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
	uint32_t one_screen;
	uint8_t *p;


		one_screen=  _info.width*_info.height;
		one_screen+=one_screen>>1;

		if(!_screen) // first time -> allocate and fill the whole stuff
			{

         			// allocate ptr
				//_ptr_to_screen=new *uint8_t[_param->_radius];
				_ptr_to_screen=(uint8_t **)malloc(_param->radius*sizeof(uint8_t *));
				_screen=new uint8_t[_param->radius*_info.width*_info.height*2];

				p=_screen;

				for(uint32_t i=0;i<_param->radius;i++)
				{
                          	
					_ptr_to_screen[i]=p;
					p+=one_screen;
				}
	        		if(!_in->getFrameNumberNoAlloc(frame, len,_screen,flags)) return 0;
				// copy it to next ones
				for(uint32_t i=1;i<_param->radius;i++)
				{
					memcpy( _ptr_to_screen[i],_screen,one_screen);
				}

				// ready !
				memcpy(data,_screen,one_screen);
				*len=one_screen;
				return 1;							
			}
       	// now is the generic case
	// first round rubbing the screens (0 being the most recent...)
			uint8_t *o;
			o=_ptr_to_screen[_param->radius-1];
    			for(uint32_t i=1;i<_param->radius;i++)	
			{
			   	_ptr_to_screen[i]=_ptr_to_screen[i-1];
            		}
			_ptr_to_screen[0]=o;
  			if(!_in->getFrameNumberNoAlloc(frame, len,o,flags)) return 0;
	
    		// now copy Y  will be dealt with later
			memcpy(data ,o,     _info.width*_info.height);
		
			uint8_t cont1;
			uint8_t u,v,nu,nv;
//			uint8_t y1,y2,y3,y4,ny;
//			uint8_t ny1,ny2,ny3,ny4;


			uint32_t  same,diff;
			int32_t delta,ru,rv;
//			int32_t ry1,ry2,ry3,ry4;

			uint8_t *out,*outu,*outv,*in,*inu,*inv;
			uint32_t thres_chroma=		_param->chroma_threshold*_param->chroma_threshold;	
//          	uint32_t thres_luma=		_param->luma_threshold*_param->luma_threshold;	

			uint8_t **ptr  = (uint8_t **)malloc(_param->radius * sizeof(uint8_t *));
			uint8_t **ptru = (uint8_t **)malloc(_param->radius * sizeof(uint8_t *));
			uint8_t **ptrv = (uint8_t **)malloc(_param->radius * sizeof(uint8_t *));
						ADM_assert(ptr);
						ADM_assert(ptru);
						ADM_assert(ptrv);

						for(uint8_t j=1;j<_param->radius;j++)
						{
                      		ptr[j]=_ptr_to_screen[j];
								ptru[j]=ptr[j]+_info.width*_info.height;
								ptrv[j]=ptru[j]+(_info.width*_info.height>>2);

						}


	       same=diff=0;

			out=data;
			outu= out+_info.width*_info.height;
			outv=outu+(_info.width*_info.height>>2);;

			inu= o+_info.width*_info.height;
			inv=inu+(_info.width*_info.height>>2);;

			in=o;

			

       	for(uint32_t z=(_info.height>>1);z>0;z--)
			{
       	for(uint32_t x=(_info.width>>1);x>0;x--)
			{
	             		cont1=1;						

						ru=u=*inu++;
						rv=v=*inv++;
#define IFF(w,t)  delta=n##w-w;delta*=delta; 	if((int32_t)delta<(int32_t)t)
						for(uint8_t j=1;j<_param->radius;j++)
						{
								nu= *(ptru[j]++);
								nv= *(ptrv[j]++);

								IFF(u,thres_chroma)								
									{                      	
                            					IFF(v,thres_chroma)						
													{
#define IFFY(x) IFF(y##x,thres_luma) { ry##x## +=ny##x##;cont##x##++;ru+=nu;rv+=nv;}
														
															ru+=nu;
															rv+=nv;
															cont1++;														
                                   			}              // end if chroma
									
									}                           // end if chroma

                           } // end loop throught radius

					// now check result
					if(cont1!=1) 	same++; // we got a match!

					if(cont1>1)
					{					
						*outu++= ( ru+(cont1>>1))/cont1;			
						*outv++= ( rv+(cont1>>1))/cont1;
					}
					else
					{
                   	*outu++=128;          // hig light moving sections
                   	*outv++=0;

					}

			} // end 140x
		} // end y
		*len=one_screen;
		*flags=0;

		 diff=(_info.width*_info.height>>2)-same;
		if(diff>same) // scene change !
		{
        	printf("scene change detected\n");
							for(uint32_t i=1;i<_param->radius;i++)
							{
								memcpy( _ptr_to_screen[i],o,one_screen);
							}
							memcpy(data,o,one_screen);


		}
		printf("same : %08lu diff: %08lu  total %08lu\n",same,diff,same+diff);
	
	free(ptr);
	free(ptru);
	free(ptrv);
        return 1;
}
#endif
