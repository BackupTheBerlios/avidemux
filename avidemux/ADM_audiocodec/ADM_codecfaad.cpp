//
// C++ Implementation: ADM_codecfaad
//
// Description: class ADM_faad, interface to libfaad2 .
//
// The init can be done 2 ways
// 1- some info are used as extradata (esds atom ot whatever) 
// 2- the init is done when decoding the actual raw stream
//
// The big drawback if that in some case you can eat up a lot of the stream
// before finding a packet start code to enable correct init of the codec
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#ifdef USE_FAAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "faad.h"

static uint32_t xin,xout;

ADM_faad::ADM_faad( uint32_t fourcc ,WAVHeader *info,uint32_t l,uint8_t *d) :   ADM_Audiocodec(fourcc)
{
faacDecConfigurationPtr conf;
unsigned long srate;
unsigned char chan;
		_inited=0;
		_instance=NULL;
		_inbuffer=0;
		_instance = faacDecOpen();
		conf=faacDecGetCurrentConfiguration(_instance);
		// Update the field we know about
		conf->outputFormat=FAAD_FMT_16BIT;
		conf->defSampleRate=info->frequency;
		conf->defObjectType =LC;
		faacDecSetConfiguration(_instance, conf);
		// if we have some extra data, it means we can init it from it
		if(l)
		{
			_inited = 1;
			faacDecInit2(_instance, d,l, &srate,&chan);
			printf("Found :%d rate %d channels\n",srate,chan);		
		}
		else // we will init it later on...
		{
			_inited=0;
			printf("No conf header, will try to init later\n");
			
		}
		printf("Faad decoder created\n");
}
ADM_faad::~ADM_faad()
{
	if(_instance)
		faacDecClose(_instance);
	_instance=NULL;
	printf("Faad decoder closed\n");
}

void ADM_faad::purge(void)
{
	_inbuffer=0;
	 faacDecPostSeekReset(_instance, 0);
}
uint8_t ADM_faad:: beginDecompress( void )
{
	_inbuffer=0;
}
uint8_t ADM_faad::endDecompress( void ) 
{
	_inbuffer=0;
	 faacDecPostSeekReset(_instance, 0);
}
uint8_t ADM_faad::run( uint8_t * ptr, uint32_t nbIn, 
			uint8_t * outptr,   uint32_t * nbOut)
{
long int res;
void *outbuf;
faacDecFrameInfo info;
int max=0;
unsigned long srate=0;
unsigned char chan=0;
uint8_t first=0;


		assert(_instance);
		*nbOut=0;
		if(!_inited) // we still have'nt initialized the codec
		{
			// Try
			printf("Trying with %d bytes\n",nbIn);
			res=faacDecInit(_instance,ptr,nbIn,&srate,&chan);
			if(res>=0)
			{
				printf("Faad Inited : rate:%d chan:%d off:%ld\n",srate,chan,res);
				_inited=1;
				first=1;
				_inbuffer=0;
				ptr+=res;
				nbIn-=res;
				
			}
		}
		if(!_inited)
		{
			printf("No dice...\n");
			return 1;	
		}
		// The codec is initialized, feed him
		do
		{
			max=FAAD_BUFFER-_inbuffer;
			if(nbIn<max) max=nbIn;
			memcpy(_buffer+_inbuffer,ptr,max);
			ptr+=max;
			nbIn-=max;
			_inbuffer+=max;
			/*
			if(_inbuffer<FAAD_MIN_STREAMSIZE*2)
			{
				return 1;
			}*/
			memset(&info,0,sizeof(info));
			//printf("%x %x\n",_buffer[0],_buffer[1]);
		 	outbuf = faacDecDecode(_instance, &info, _buffer, _inbuffer);
			if(info.error)
			{
				printf("Faad: Error %d :%s\n",info.error,
					faacDecGetErrorMessage(info.error));
					/*
				xin=info.bytesconsumed ;
				if(!xin) xin=1;
				memmove(_buffer,_buffer+xin,_inbuffer-xin);
				_inbuffer-=xin;
				return 1;*/
				return 0;
			}
			if(first)
			{
				printf("Channels : %d\n",info.channels);
				printf("Frequency: %d\n",info.samplerate);
				printf("SBR      : %d\n",info.sbr);
				
			
			}
			xin=info.bytesconsumed ;
			if(xin>_inbuffer) xin=0;
			
			xout=info.samples*2;
			
			//printf("in:%d out:%d\n",xin,xout);
			
			memmove(_buffer,_buffer+xin,_inbuffer-xin);
			_inbuffer-=xin;
			if(xout)
			{
				memcpy(outptr,outbuf,xout);
				*nbOut+=xout;
				outptr+=xout;
			}
		}while(nbIn);
		return 1;
}
                 
          
#endif
