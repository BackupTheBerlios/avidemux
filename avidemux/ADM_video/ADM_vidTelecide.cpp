/***************************************************************************
                          ADM_vidTelecide.cpp  -  description
                             -------------------
	Strongly inspired by Donal Graft deinterlacer (decomb)

***************************************************************************
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
//#define EVEN_INTERLEAVE	

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include"ADM_video/ADM_vidDeinterlace.h"
#include"ADM_video/ADM_vidTelecide.h"

#define MATCH_THRESH 100
#define ASM_DEINT
#define ASM_BLEND

extern  int32_t _l_w,_l_h;
extern uint8_t *_l_p,*_l_c,*_l_n;
extern uint8_t *_l_e,*_l_e2;

BUILD_CREATE(telecide_create,ADMVideoTelecide);


char *ADMVideoTelecide::printConf(void)
{
 		return (char *)"Pal Smart";;
}

//_______________________________________________________________


ADMVideoTelecide::ADMVideoTelecide(	AVDMGenericVideoStream *in,CONFcouple *setup)
										: ADMVideoFields(in,setup)
{


	_next=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
  assert(_next);
  _instock=0xFFFF0000;
  	  	
}
ADMVideoTelecide::~ADMVideoTelecide()
{
 	
	delete []_next;	
	
}
/*
  	Interleave _uncompressed with in2
   		even line from in2 odd=0
      odd  line          odd=1
*/
uint8_t ADMVideoTelecide::interleave(	uint8_t *in2,uint8_t odd)
{
 	uint32_t w=_info.width;
	uint8_t  	*out=_uncompressed;

if(odd)
{
	in2+=w;
	out+=w;
}
	for(uint32_t y=(_info.height>>1);y>0;y--)
		{
        		memcpy(out,in2,w);						
				in2+=w<<1;
				out+=w<<1;
		}
	return 1;
}

//
//		Try to march fields of a frame with previous  / next until it is not interlaced	
//
//

uint8_t ADMVideoTelecide::getFrameNumberNoAlloc(uint32_t frame,
																	uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
uint32_t uvlen;
uint32_t dummylen;
uint8_t motion;

uint32_t 	cmatch,nmatch,n2match;

			

			assert(frame<_info.nb_frames);
			assert(_uncompressed);					
			
			uvlen=    _info.width*_info.height;
			*len=uvlen+(uvlen>>1);
			
			if(_instock!=frame)
			{																	
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, &dummylen,_uncompressed,flags)) return 0;         	
   		}
     	else
      {
					memcpy(_uncompressed,_next,*len);
			}

           // for u & v , no action -> copy it as is
           memcpy(data,_uncompressed,(uvlen*3)>>1);
	
    	     if(frame==_info.nb_frames-1) return 1;

           // No interleaving detected
           if(!(motion=hasMotion()) )
           	{
									printf("\n Not interlaced !\n");
      						return 1; // over !					
             }
            cmatch=getMatch();

    /*	------------------------------------------------------------------------------------
								Try to complete with next frame  fields
	------------------------------------------------------------------------------------
					*/
            if(!_in->getFrameNumberNoAlloc(frame+1, &dummylen,_next,flags)) return 0;
            _instock=frame+1;
						interleave(_next,0);

						nmatch=getMatch();
						
					  memcpy(_uncompressed,data,_info.width*_info.height);
	          interleave(_next,1);
	          
 						n2match=getMatch();

						printf(" Cur  : %lu \n",cmatch);
						printf(" Next : %lu \n",nmatch);
						printf(" NextP: %lu \n",n2match);

						if((cmatch<nmatch)&&(cmatch<n2match))
						{
			   				printf("\n __ pure interlaced __\n");
			          memcpy(_uncompressed,data,_info.width*_info.height);
								hasMotion();
	  		        doBlend(data);					
								return 1;
						}
						if( nmatch > n2match)
						{
							printf("\n -------Shifted-P is better \n");
							if(hasMotion())
							{
								 doBlend(data);
								 printf(" but there is still motion \n");
							}
						}
						else
						{
							printf("\n -------Shifted-O is better \n");
							memcpy(_uncompressed,data,_info.width*_info.height);
	          	interleave(_next,0);
							if(hasMotion())
							{
								 doBlend(data);
								 printf(" but there is still motion \n");
							}
						}							  		    			
						// which chroma is better ? from current or from next ?
						// search for a transition and see if there is also one ?
						
						return 1;						
}

/*
   	Returns the number of difference (interlacing) found


*/
uint32_t      ADMVideoTelecide::getMatch( void )
{

			uint32_t m=0,x,y;

			uint8_t *p,*n,*c;

			c=_uncompressed + _info.width;
			n=_uncompressed + _info.width+_info.width;
			p=_uncompressed ;


			for(y=_info.height>>2;  y >2 ; y--)
					{
           for(x=_info.width;x>0;x--)
								{
                             		if(  (*c-*p)*(*c-*n) >MATCH_THRESH) m++;
										p++;c++;n++;
								}
								p+=3*_info.width;
								c+=3*_info.width;
								n+=3*_info.width;

					}

                 return m;
}



#endif






