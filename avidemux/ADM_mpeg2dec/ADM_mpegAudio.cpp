/***************************************************************************
                          ADM_mpegAudio.cpp  -  description
                             -------------------
    begin                : Sun Oct 27 2002
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
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include <ADM_assert.h>
#include "ADM_mpeg2dec/ADM_mpegscan.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_TS.h"
#include "ADM_mpeg2dec/ADM_mpegAudio.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG_DEMUX
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_audio/ADM_a52info.h"

//___________________________________________________																
//___________________________________________________																

//___________________________________________________																
//___________________________________________________																
AVDMMpeg2decAudioStream::~AVDMMpeg2decAudioStream()
{
	delete [] _sync;	
	delete _wavheader;
	delete demuxer;
	
}					
AVDMMpeg2decAudioStream::AVDMMpeg2decAudioStream(char *name,uint32_t nb_sync)
{
	 	FILE *file;	
		int type=0,id;
		uint8_t ctype=0;
		uint64_t abs;
		uint32_t dummy;//,audio=0;
		char string[1025]; //,str[1024];;
	
		_sync= NULL;
		_wavheader=NULL;
		
		printf("\n  opening d2v file for audio track : %s\n",name);
		file=fopen(name,"rt");
		if(!file) 
			{
				printf("\n Error !!");
				ADM_assert(0);
			}
		
		fgets(string,1023,file);   	// File header
		sscanf(string,"IDXM %c %x",(char*)&ctype,&id);
		type=ctype;
		fgets(string,1023,file);   	// # of  frames	 		
		fgets(string,1023,file);   	// nb file
		fgets(string,1023,file);   	// File header
		string[strlen(string)-1]=0; // remove CR
		switch(type)
		{
		case 'P':	demuxer=new ADM_mpegDemuxerProgramStream(id,0xe0); 
				// we take the 0xc0 streamid->audio
				break;
		case 'T':
			{
				uint32_t opid=0, ovid=0;
				ADM_matchPid(string, id, &ovid, &opid);
				printf("Audio Stream id : %x\n",opid);
				demuxer=new ADM_mpegDemuxerTransportStream(opid,0xff); 
				// we take the 0xc0 streamid->audio
				break;
			}
		default:
				ADM_assert(0);
		}
		
						
		if(!demuxer->open(string))
		{
				printf("\n cannot open mpeg >%s<\n",string);
				delete demuxer;
				fclose(file);
				ADM_assert(0);

		}
		
		_syncPoints=nb_sync;
		_sync=new  SYNC_[_syncPoints+1]	; 	
		ADM_assert(_sync);		

		fgets(string,1023,file);   	// Size x*y

		// Now I Offset + nb B/P frames		
		uint8_t t;
		uint64_t of;
		uint32_t sz;		
		printf("\n looking up %lu sync points\n",nb_sync);
		for(uint32_t y=0;y<nb_sync;)
		{
			     fgets(string,1023,file);     
			     if(string[0]=='I')
		        {
#ifndef CYG_MANGLING			
				if(5!=sscanf(string,"%c %llX %llX %lX %lu",
#else
				if(5!=sscanf(string,"%c %I64X %I64X %lX %lu",
#endif				
					&t,&of, &abs,&dummy,&sz))
				  {
					printf("\n not enought infos : %s", string);
				}

				_sync[y].absolute=abs;
				_sync[y].relative=dummy;
				//printf(" %llX %llX\n", _sync[y].absolute,_sync[y].relative);
				y++;	
			 }

		}    
		
		fclose(file);  
			
			// now fill in the header
			_length=_sync[nb_sync-1].relative;
			_wavheader=new   WAVHeader;
			memset(  _wavheader,0,sizeof( WAVHeader));
			
			// put some default value
			_wavheader->bitspersample=16;
			_wavheader->blockalign = 4;
			_wavheader->byterate=224000/8;
			_wavheader->channels=2;

			_destroyable=1;
			strcpy(_name,"mpg audio");
			switch(id)
			{
				case 0xA0: //LPCM
				case 0xA1:
				case 0xA2:
				case 0xA3:
				case 0xA4:
				case 0xA5:
				case 0xA6:
				case 0xA7:
			// mp
						_wavheader->frequency=48000;
						_wavheader->channels=2;
						_wavheader->byterate=48000*4;
						_wavheader->encoding=WAV_LPCM;
						break;
				case 0xC0:
				case 0xC1:
				case 0xC2:
				case 0xC3:
				case 0xC4:
				case 0xC5:
				case 0xC6:
				case 0xC7:
			// mp2 audio
					_wavheader->encoding=WAV_MP2;
					demuxer->goTo(0);
					_pos=0;
					if(!getAudioSpec())
					{
						printf("\n Could not get audio information from mpeg stream -> zeroing audio\n");
						_length=0;
					}
					break;
				case 0: // AC3
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:

						_wavheader->encoding=WAV_AC3;
						demuxer->goTo(0);
						_pos=0;
						if(!getAC3AudioSpec())
						{
							printf("\n Could not get audio information from AC3 stream -> zeroing audio, libA52 not installed ??\n");
							_length=0;
						}
						break;
				default:
						ADM_assert(0);

			}
			demuxer->goTo(0);
			_pos=0;
			printf("\n Mpeg audio initialized (%lu bytes)\n",_length);						
}															
// __________________________________________________________
// __________________________________________________________

uint8_t 			AVDMMpeg2decAudioStream::goTo(uint32_t offset)
{	
		// special cases
		if(offset>=_length) 
		{
			printf("mpeg A: Out of bound \n");
			return 0;
		}
		if(offset==0)
		{
			_pos=0;	 
			 return demuxer->goTo(0);
		}
	
	// roughly go to needed point
	// find the matching point
	ADM_mpegDemuxerProgramStream *dem;
	dem=(ADM_mpegDemuxerProgramStream*)demuxer;
	
	// Border line case
	if(offset<_sync[0].relative)
	{
		aprintf("Audio: too low value, going back to 0\n");
		_pos=0;
		return demuxer->goTo(0);
	}
#if 0	
	if(_offset>_sync[_syncPoints-1].relative)
	{
		aprintf("Audio: too hi value, going to the end\n");
		_pos=0;
		return demuxer->goTo(0);
	}
#endif
	
	for(uint32_t y=0;y<_syncPoints-1;y++)
	{
			if((offset>=_sync[y].relative) && (offset<_sync[y+1].relative))
			{
			//	printf(" found %d : %llX %llX  -- %llX %llX \n",y, _sync[y].absolute,_sync[y].relative,
			//							_sync[y+1].absolute,_sync[y+1].relative);
				if(!dem->_asyncJump2(_sync[y].relative,_sync[y].absolute))
				{
						printf("\nAudio async jump failed\n");
						aprintf("\n offset : %lu relative:%lu absolute :%lu\n",
										offset,_sync[y].relative,_sync[y].relative);
						return 0;
				}
				_pos=offset;					
				return demuxer->goTo(offset);										
			}		
	}
	printf("\n Audio : Duh!\n");
	printf("We tried to sync : Offset :%d, first :%d last :%d\n",
				offset,_sync[0].relative,_sync[_syncPoints-1]);
	return 0;
}

// __________________________________________________________
// __________________________________________________________

uint32_t 			AVDMMpeg2decAudioStream::read(uint32_t size,uint8_t *ptr)
{
	uint32_t read;		
	
	//		demuxer->dump();
			if(_pos+size>=_length) 
			{	
					size=_length-_pos-1;
			}
			read= demuxer->read(size,ptr);	
				demuxer->dump();
	//			printf("\n read :%d\n",read);
			if(read!=size)
			{
				printf("\n**** Warning not enough bytes***");	
			}
			_pos+=read;	
			return read;
}
// __________________________________________________________
// Code present 3 times -> factorize it...
//
//	Detect mpeg features
// __________________________________________________________


uint8_t 			AVDMMpeg2decAudioStream::getAudioSpec(void)
{
#define SEARCH	2*1024 
   uint8_t buffer[SEARCH],*ptr;
   uint32_t rd;
   
   
	ptr=buffer;
	if(!(rd=read(SEARCH,buffer)))
	{
		printf("\n read failed\n");
		return 0;	
	}
	return  mpegAudioIdentify(ptr, SEARCH, _wavheader,_mpegSync); 
 }
 //-----------------------------------------------------------------------
 #undef SEARCH
 #define SEARCH	8*1024 
 uint8_t 			AVDMMpeg2decAudioStream::getAC3AudioSpec(void)
{

   uint8_t buffer[SEARCH],*ptr;
   uint32_t rd,fq,br,ch,syncoff;
   
   
   
	ptr=buffer;
	if(!(rd=read(SEARCH,buffer)))
	{
		printf("\n read failed\n");
		return 0;	
	}
	if(! ADM_AC3GetInfo(ptr, SEARCH, &fq, &br,&ch,&syncoff)) return 0;
	_wavheader->frequency=fq;
	_wavheader->byterate=br;
        return 1;
 }

 //
