
/***************************************************************************
                        Simple equalizer or linear luma/chroma filter
    

   (C) 2004/2005 Mean fixounet@free.fr
   GPL version 2

 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"
#include "math.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include"ADM_video/ADM_vidField.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"



#include "ADM_vidEqualizer.h"
#include "ADM_filter/video_filters.h"

static FILTER_PARAM equalizer_template={8,{"0","1","2","3"
					,"4","5","6","7"}};
BUILD_CREATE(equalizer_create,vidEqualizer);
SCRIPT_CREATE(equalizer_script,vidEqualizer,equalizer_template);

extern uint8_t DIA_getEqualizer(EqualizerParam *param, AVDMGenericVideoStream *incoming);

uint8_t vidEqualizer::configure(AVDMGenericVideoStream *in)
{
ADMImage *video1;
uint32_t l,f;
uint8_t r;

	_in=in;		
	r= DIA_getEqualizer(_param,in);
	return r;
	
}

char *vidEqualizer::printConf( void )
{
 	static char buf[50];
 	sprintf((char *)buf," Equalizer");
        return buf;
}

static const int cross[8]= {0,36,73,109,
			146,182,219,255};
			
//_____________________________________________			
// Interpolate
// Through the points to build the full range
// conversion table
//_____________________________________________
#if 1 // Linear
uint8_t equalizerBuildScaler(int32_t *p,uint32_t *s)
{
double alpha,r;
int32_t deltax,deltay;

	for(int i=0;i<7;i++)
	{
		// line that goes from i to i+1
		deltax=cross[i+1]-cross[i];
		deltay=p[i+1]-p[i];
		
		if(deltay==0) // flat line
		{
			for(uint32_t x=cross[i];x<cross[i+1];x++)
                        {
                                if(p[i]>=0)
				    s[x]=p[i];		
                                else
                                    s[x]=0;
                        }
		}	
		else
		{
			alpha=deltay;
			alpha/=deltax;
			for(int32_t x=cross[i];x<cross[i+1];x++)
			{
				r=(x-cross[i])*alpha;
				r+=p[i];
                                if(r<0) r=0;
				
				s[x]=(uint32_t)floor(r+0.49);			
			}
		}
	}
	for(int i=0;i<256;i++)
        {
                                if(s[i]<0) s[i]=0;
                                if(s[i]>255) s[i]=255;
        
        }
	
		
}	
#else // X3+X+Y
uint8_t equalizerBuildScaler(int32_t *p,uint32_t *s)
{
double r;
int32_t deltax,deltay;
double alpha,beta;
double y1,y2;
	for(int i=0;i<7;i++)
	{
		if(i>0 && i<6)
		{
			y2=p[i+1]-p[i-1];
			y1=p[i]-p[i-1];
			alpha=y2-2*y1;
		}
		else alpha=0;
		if(alpha==0 ) // Linear
		{
			deltax=cross[i+1]-cross[i];
			deltay=p[i+1]-p[i];
			if(deltay==0) // flat line
			{
				for(uint32_t x=cross[i];x<cross[i+1];x++)
					s[x]=p[i];		
			}	
			else
			{
				alpha=deltay;
				alpha/=deltax;
				for(int32_t x=cross[i];x<cross[i+1];x++)
				{
					r=(x-cross[i])*alpha;
					r+=p[i];
					if(r<0) r=0;
					s[x]=(uint32_t)floor(r+0.49);			
				}
			}
		}
		else // spline
		{
			double ss=36.429;
			double xx;
			alpha=alpha/(6*ss*ss*ss);
			beta=y1-alpha*ss*ss*ss;
			beta/=ss;
			
			for(int32_t x=cross[i];x<cross[i+1];x++)
				{
					xx=x;
					xx-=cross[i-1];
					r=(beta+xx*xx*alpha);
					r*=xx;
					r+=p[i-1];
					if(r<0) r=0;
                                        if(r>255) r=255;
					s[x]=(uint32_t)floor(r+0.49);			
				}
			
		
		
		
		}
	}
}	

#endif
vidEqualizer::vidEqualizer(AVDMGenericVideoStream *in,CONFcouple *couples) 
{


		
   		
		_in=in;		
   		memcpy(&_info,_in->getInfo(),sizeof(_info));    
  		_info.encoding=1;
		_uncompressed=NULL;
		
  		_info.encoding=1;
		_uncompressed=new ADMImage(_info.width,_info.height);
		
		
		_param=NEW(EqualizerParam);
		if(couples)
		{
		
#undef GET
#define GET(x) couples->getCouple((char *)"p"#x,&(_param->_points[x]))		
			GET(0);
			GET(1);
			GET(2);
			GET(3);
			GET(4);
			GET(5);
			GET(6);
			GET(7);
		}
		else // Default
  		{
			 	_param->_points[0]=0;
				_param->_points[1]=36;
				_param->_points[2]=73;
				_param->_points[3]=109;
				_param->_points[4]=146;
				_param->_points[5]=182;
				_param->_points[6]=219;
				_param->_points[7]=255;

		}
		equalizerBuildScaler((int32_t *)_param->_points,(uint32_t *)_param->_scaler); 
				
		

		
}
//____________________________________________________________________
vidEqualizer::~vidEqualizer()
{
		
	delete _uncompressed;
	delete _param;
	_param=NULL;
	_uncompressed=NULL;
		
		
}

//______________________________________________________________
uint8_t vidEqualizer::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

	if(!_in->getFrameNumberNoAlloc(frame,len,_uncompressed,flags)) return 0;
	
	uint8_t *src,*dst;
	src=_uncompressed->data;
	dst=data->data;
	for(uint32_t y=_info.height;y>0;y--)
	for(uint32_t x=_info.width;x>0;x--)
		*(dst++)=_param->_scaler[*(src++)];

	uint32_t square=_info.width*_info.height;
	square>>=2;
	// copy u & v too
	memcpy(data->data+4*square,_uncompressed->data+4*square,2*square);
	return 1;
}


uint8_t	vidEqualizer::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(8);
#undef CSET
#define CSET(x)  (*couples)->setCouple((char *)"p"#x,(_param->_points[x]))

	CSET(0);
	CSET(1);
	CSET(2);
	CSET(3);
	CSET(4);
	CSET(5);
	CSET(6);
	CSET(7);
	
	return 1;
}

// EOF
