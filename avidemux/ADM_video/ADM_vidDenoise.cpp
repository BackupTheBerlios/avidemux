/***************************************************************************
                          ADM_vidDenoise.cpp  -  description
                             -------------------
    begin                : Mon Nov 25 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Denoiser inspired from DNR in transcode
    Ported to YV12 and simplified
    
   Original code  Copyright (C) Gerhard Monzel - November 2001
 
    
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
#include "ADM_video/ADM_vidDenoise.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM denoiseParam={5,{"lumaLock","lumaThreshold","chromaLock","chromaThreshold",
					"sceneChange"}};


SCRIPT_CREATE(denoise_script,ADMVideoDenoise,denoiseParam);

uint8_t distMatrix[256][256];
uint32_t fixMul[16];

//static uint8_t matrixReady=0;
//static uint8_t doOnePix(uint8_t *in,uint8_t *out,uint8_t *lock,uint8_t *nb);

BUILD_CREATE(denoise_create,ADMVideoDenoise);
char *ADMVideoDenoise::printConf( void )
{
 	static char buf[50];

  assert(_param); 	
 	sprintf((char *)buf," Denoise : Lum :%02ld/:%02ld / Chm :%02ld/%02ld",
  								_param->lumaLock,
          				_param->lumaThreshold,
              		_param->chromaLock,
                	_param->chromaThreshold);
        return buf;
}
void buildDistMatrix( void );
void buildDistMatrix( void )
{
int d;	
	for(uint32_t y=255;y>0;y--)
	for(uint32_t x=255;x>0;x--)
	{
		  d=x-y;
		  if(d<0) d=-d;
		  distMatrix[x][y]=d;
		
	}

	 for(int i=1;i<16;i++)
                        {
                                        fixMul[i]=(1<<16)/i;
                        }

}

//_______________________________________________________________

ADMVideoDenoise::ADMVideoDenoise(
									AVDMGenericVideoStream *in,CONFcouple *couples)
{


  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  			 	
    uint32_t page;
    
  _info.encoding=1;
  
  page= 3*_in->getInfo()->width*_in->getInfo()->height;
  
  _uncompressed=new uint8_t [page];
  assert(_uncompressed);
  
  _locked=new uint8_t [page];
  assert(_locked);
 
	_lockcount=new uint8_t [page];
  assert(_lockcount);  
  
  memset(_lockcount,0,page);  
        
  _param=NULL;
  
  if(couples)
  	{
			_param=NEW(NOISE_PARAM);
			GET(lumaLock);
			GET(lumaThreshold);
			GET(chromaLock);
			GET(chromaThreshold);
			GET(sceneChange);
		 }
	else
		{
			  #define XXX 1
			  _param=NEW(NOISE_PARAM);
			  _param->lumaLock=  4*XXX;
			  _param->lumaThreshold= 10*XXX;
			  _param->chromaLock=  8*XXX;
			  _param->chromaThreshold= 16*XXX;
        _param->sceneChange=  30*XXX;
			}
  	  _lastFrame=0xfffffff0;	
}


uint8_t	ADMVideoDenoise::getCoupledConf( CONFcouple **couples)
{

			assert(_param);
			*couples=new CONFcouple(5);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(lumaLock);
	CSET(lumaThreshold);
	CSET(chromaLock);
	CSET(chromaThreshold);
	CSET(sceneChange);

	return 1;

}
ADMVideoDenoise::~ADMVideoDenoise()
{
 	
	delete [] _uncompressed;
 	delete [] _locked;
  delete [] _lockcount;
  DELETE(_param);
  
  _uncompressed=_locked=_lockcount=NULL;
}

//
//	Remove y and v just keep U and expand it
//
uint8_t ADMVideoDenoise::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
   //uint32_t x,w;
  	uint32_t page; 
   		assert(_param);
			assert(frame<_info.nb_frames);
								
			
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       			
                     
           //uint32_t sz=_info.width*_info.height;

          if((_lastFrame+1)!=frame) // async jump
          {
							// just copy it 
								memcpy(data,_uncompressed,*len);
								memcpy(_locked,_uncompressed,*len);
								_lastFrame=frame;
								return 1;
					}          
				_lastFrame=frame;
          
          // copy chroma for now
          page=  _info.width*_info.height>>2;
         
          
          //
          //uint32_t count=0;
          //uint32_t cell=page*4; // size of luma
          uint8_t *in,*out,*lock,*nb;
          uint8_t *uin,*uout,*ulock,*unb;
          uint8_t *vin,*vout,*vlock,*vnb;
          
          
          //uint32_t d;
          // init all
          
          // luma
          nb=_lockcount;
          lock=_locked;
          in=_uncompressed;
          out=data;
          // u
          unb=nb+page*4;
          ulock=lock+page*4;
          uin=in+page*4;
          uout=data+page*4;
          // v
          vnb=unb+page;
          vlock=ulock+page;
          vin=uin+page;
          vout=uout+page;
          
          
          uint32_t xx,yy/*,dl*/,du,dv;
          uint32_t locked=0;
          for(yy=_info.height>>1;yy>0;yy--)
          {
	          for(xx=_info.width>>1;xx>0;xx--)          
  	        {
								du=distMatrix[*uin][*ulock];														
								dv=distMatrix[*vin][*vlock];														
						
								// if chroma is locked , we try to lock luma
								if( (du<_param->chromaLock)
									 && (dv<_param->chromaLock))
									 {  
										 			*uout=*ulock;
 										 			*vout=*vlock;

#define PIX(z) 		doOnePix(in+z,out+z,lock+z,nb+z)									 
										 			locked+=PIX(0)+	PIX(1)+ 	PIX(_info.width)+
										     	PIX(_info.width+1);										      										 
										}
										else
								  // if chroma is blended, we blend luma
#undef PIX								  
#define PIX(z) 		doBlend(in+z,out+z,lock+z,nb+z)									 
									if( (du<_param->chromaThreshold)
									 && (dv<_param->chromaThreshold))
										{
											 		PIX(0);
										    	PIX(1);
										     	PIX(_info.width);
										     	PIX(_info.width+1);															
										      *uout=*ulock=(*uin+*uin)>>1;
 										 			*vout=*vlock=(*vin+*vin)>>1;
										}
#undef PIX											
										
										else
										{
#define PIX(z) *(out+z)=*(lock+z)=*(in+z);*(nb+z)=0											
											
													PIX(0);
										    	PIX(1);
										     	PIX(_info.width);
										     	PIX(_info.width+1);		
										 			*uout=*ulock=*uin;
 										 			*vout=*vlock=*vin;
											
#undef PIX											
										}
								  
											                        				                        
							uin++;uout++;ulock++;unb++;   
							vin++;vout++;vlock++;vnb++;   
							in++;out++;lock++;nb++;   
							in++;out++;lock++;nb++;   
							
						}
            // 
            in+=_info.width;
            out+=_info.width;
            lock+=_info.width;
            nb+=_info.width;            						
					};
          
          if(locked>page*3) // if more than 75% pixel not locked -> scene change
          {
						  	memcpy(data,_uncompressed,*len);
								memcpy(_locked,_uncompressed,*len);
					}
           
      return 1;
}

//
//	0 copy
//  1 lock
//  2 threshold
//
uint8_t ADMVideoDenoise::doOnePix(uint8_t *in,uint8_t *out,uint8_t *lock,uint8_t *nb)
{
unsigned int d;
							d=distMatrix[*(in)][*(lock)]; 
							if(d<_param->lumaLock)         
							{								                
								if(*(nb)>30)  // out of scope -> copy new                   
								{  						// too much copy ->                              
										*(nb)=0;                       
										*(out)=(*(in)+*(lock))>>1;
										*(lock)=*(out);    										
										return DN_COPY;      
								}                                 
								else                               
								{                                   
									*(out)=*(lock);		
									*nb += 1; // *(nb)++;	
									return DN_LOCK;		
								}                  
							}                     
							else if(d< _param->lumaThreshold) 
							{                                  
								 *(nb)=0;                           
									*(out)=(*(in)+*(lock))>>1;	
									return DN_BLEND;							
							}
							else   // too big delta
							{    
								 *(nb)=0; 
									*(out)=*(in);	  
									*(lock)=*(in);    
									return DN_COPY;
							}                     
					                           
							assert(0);
							return 0;

}
uint8_t ADMVideoDenoise::doBlend(uint8_t *in,uint8_t *out,uint8_t *lock,uint8_t *nb)
{
unsigned int d;
		   d=distMatrix[*(in)][*(lock)]; 
		   *nb=0;
		   
			if(d<_param->lumaThreshold)         
			{
					*(out)=(*(in)+*(lock))>>1;					
			}
			else
			*out=*in;
			return 0;
	
}

// EOF
