//
// C++ Implementation: ADM_ogmAudio
//
// Description: 
//
// This implements the audio stream reader from ogm file format
// It is restricted to vorbis audio ATM
// It uses a slightly different api than other audio due to the
//  specificity of ogg (packet oriented, strongly)
//
//  The destructor may appear as incomplete, it is because a lot of arrays
//  are from the global demuxer, so they will be freed by it later on
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ADM_ogm/ADM_ogm.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_OGM_AUDIO
#include "ADM_toolkit/ADM_debug.h"

extern void mixDump(uint8_t *ptr,uint32_t len);

oggAudio::~ oggAudio()
{
	if(_demuxer)
	{
		delete _demuxer;
		_demuxer=NULL;
	}
	if(_extraData&&_extraLen)
	{
		delete [] _extraData;
		_extraData=NULL;
		_extraLen=0;
	
	}
	

}

oggAudio::oggAudio( char *name,OgAudioTrack *tracks,uint8_t trkidx )
{
	_tracks=tracks;
	_trackIndex=trkidx;
	
	_demuxer=new OGMDemuxer();
	
	assert(_demuxer->open(name));
	
	_wavheader=new   WAVHeader;
	memset(  _wavheader,0,sizeof( WAVHeader));
	_currentTrack=&_tracks[_trackIndex];		
	// put some default value
	_wavheader->bitspersample=16;
	_wavheader->blockalign = 4;
	_wavheader->byterate=_currentTrack->byterate;
	_wavheader->channels=_currentTrack->channels;
	_wavheader->frequency=_currentTrack->frequency;
	_wavheader->encoding=_currentTrack->encoding;
	
	_length=_tracks[_trackIndex].trackSize;
	
	_destroyable=1;
	_inBuffer=0;
	_lastFrag=NO_FRAG;
	strcpy(_name,"ogg audio");
	printf("Ogg audio created for track :%u\n",trkidx);
	_demuxer->setPos(0);
	_extraLen=0;
	_extraData=NULL;
	
	
	
	if(_wavheader->encoding==WAV_OGG)
	{
		// now take the 1st page as header
		uint8_t *tmp,*tmp1,*tmp2,*ptr;
		uint32_t size,size2,size1,flags;
	
		tmp=new uint8_t[64*1024];
		tmp1=new uint8_t[64*1024];
		tmp2=new uint8_t[64*1024];
	
		readPacket(&size,tmp,&flags);
		printf("1st header of %lu bytes stored as extra data (%lx flags)\n",size,flags);
	
		readPacket(&size1,tmp1,&flags);
		printf("Comment follow (%lx as flags)\n",flags);
	
		readPacket(&size2,tmp2,&flags);
		printf("cook book follow (%lx as flags)\n",flags);
	
		// 
		// We need all 3 packets to properly initialize it
		// 
		_extraLen=size+size1+size2+3*sizeof(uint32_t);
		_extraData=new uint8_t [_extraLen];
	
		ptr=_extraData+3*sizeof(uint32_t );
		memcpy(ptr,tmp,size);
		ptr+=size;
		memcpy(ptr,tmp1,size1);
		ptr+=size1;
		memcpy(ptr,tmp2,size2);
	
		delete [] tmp;
		delete [] tmp1;
		delete [] tmp2;
	
		uint32_t *idnx;
	
		idnx=(uint32_t *)_extraData;
		*idnx++=size;
		*idnx++=size1;
		*idnx++=size2;
		
		printf("Ogg audio init done\n");
	}	
	
}

// Linear interface
// DO NOT USE as much as possible
uint8_t oggAudio::goTo(uint32_t offset)
{
	// linear not supported
	// search the video frame referenced
	aprintf("Going to offset...%lu\n",offset);
	if(!offset)
	{
		_demuxer->setPos(0);
		_inBuffer=0;
		return 1;
	}
	for(uint32_t i=0;i<_currentTrack->nbAudioPacket;i++)
	{
		if(_currentTrack->index[i].dataSum>offset)
		{
			aprintf("matching frame %lu \n",i);
			// is i
			_demuxer->setPos(_currentTrack->index[i].pos);
			// search next packet			
			_inBuffer=0;
			return fillBuffer();
			
		
		}
	
	}
	return 0;
}
uint8_t oggAudio::fillBuffer( void )
{
uint32_t size,sizehdr,flags;
uint64_t f;

	while(_demuxer->readHeaderOfType(_currentTrack->audioTrack,&size,&flags,&f))
	{
		aprintf("\n page : %lu\n",f);		
		_demuxer->dumpHeaders(_buffer+_inBuffer,&sizehdr);
		_inBuffer+=sizehdr;
		_demuxer->readBytes(size,_buffer+_inBuffer);
		_inBuffer+=size;	
		assert(_inBuffer<64*1024);		
		aprintf("\n audio in buffer : %lu\n",_inBuffer);			
		return 1;
	}
	return 0;

}

uint32_t oggAudio::read(uint32_t size, uint8_t *data)
{
uint32_t i;

	if(_wavheader->encoding==WAV_OGG)
	{

		if(!_inBuffer)
			readPacket(&_inBuffer,_buffer,&i);
		if(!_inBuffer) return 0;
		if(size<_inBuffer)
		{
			memcpy(data,_buffer,size);
			memmove(_buffer,_buffer+size,_inBuffer-size);
			_inBuffer-=size;
			aprintf("Ogg:This round read %lu bytes from buffer (asked) \n",size);
			return size;
		}
		memcpy(data,_buffer,_inBuffer);
		i=_inBuffer;
		_inBuffer=0;
		aprintf("This round read %lu bytes asked %lu \n",i,size);
		return i;
	}
	// It is not vorbis audio
	// We have to skip 1 byte header in case of fresh packet
	// Ask the full page
	uint32_t cursize=0;
	uint32_t flags=0;
	uint64_t stamp=0;
	uint8_t *frags,frag;
	uint32_t ssize=0;
	while(_demuxer->readHeaderOfType(_currentTrack->audioTrack,&cursize,&flags,&stamp))
		{
			_demuxer->getLace(&frag,&frags);
			for(uint32_t i=0;i<frag;i++)
			{
				cursize=frags[i];
				if(!i && !(flags&1)) // fresh packet -> skip one byte
				{
					_demuxer->readBytes(1,data);
					assert(cursize);
					cursize--;
					_demuxer->readBytes(cursize,data);
				}
				else
				{
					_demuxer->readBytes(cursize,data);
				}
				data+=cursize;
				_lastFrag=i;
				ssize+=cursize;	
				if(frags[i]!=0xff )
				{
					if(i==frag-1) _lastFrag=NO_FRAG;
					return ssize;
				}
			}
			// Else skip it, should not happen
			printf("OGM : Failed reading non vorbis\n");
			return 0;
			
		
		}
	printf("OGM : Failed reading non vorbis\n");
	return 0;
	


}
//
//	Get the next packet and strip the header if any
//	Used for VBR interface, use it use it use it
//	It is a bit recursive due to excessive laziness

uint32_t oggAudio::readPacket(uint32_t *size, uint8_t *data,uint32_t *flags)
{
uint32_t i;
uint8_t *frags,frag;
uint32_t cursize;
uint64_t f;
uint32_t fl;


	*size=0;
	*flags=0;
		// if _lastFrag=0 it means there is some data left
		_demuxer->getLace(&frag,&frags);
		if(_lastFrag!=NO_FRAG)
		{
			for(uint32_t i=_lastFrag+1;i<frag;i++)
			{
				aprintf("\t cont :%lu -> %02d \n",i,frags[i]);
				cursize=frags[i];
				// get the frame ?
				_demuxer->readBytes(cursize,data);
				data+=cursize;
				_lastFrag=i;
				*size+=cursize;			
				if(frags[i]!=0xff )
				{
					if(i==frag-1) _lastFrag=NO_FRAG;
					return *size;
				}
			}
			_lastFrag=NO_FRAG;
			readPacket(&cursize,data,flags);
			*size+=cursize;
			return *size;
		}

		// We need a fresh packet
		while(_demuxer->readHeaderOfType(_currentTrack->audioTrack,&cursize,flags,&f))
		{
			_demuxer->getLace(&frag,&frags);
			for(uint32_t i=0;i<frag;i++)
			{
				aprintf("\t cont :%lu -> %02d\n",i,frags[i]);
				cursize=frags[i];
				// get the frame ?
				_demuxer->readBytes(cursize,data);
				data+=cursize;
				_lastFrag=i;
				*size+=cursize;			
				if(frags[i]!=0xff )
				{
					if(i==frag-1) _lastFrag=NO_FRAG;
					return *size;
				}
			}
			_lastFrag=NO_FRAG;
			readPacket(&cursize,data,flags);
			*size+=cursize;
			return *size;	
		
		}
		printf("End of ogg stream\n");
		return 0;

}
uint32_t	oggAudio::readDecompress( uint32_t size,uint8_t *ptr )
{
uint32_t flags,nbout=0;
	// Read packet
	printf("oggAudio:\n");
	if(!readPacket(&size,internalBuffer,&flags))
	{
		printf("Cannot read packet\n");
		 return 0;
	}
	if(!_codec)
	{
		printf("No codec\n");
		return 0;
	}
	if(!_codec->run( internalBuffer, size, ptr,  &nbout)) 
	{
		printf("Error decoding\n");
		return 0;
	}
	return nbout;

}
// 1/16 sec is close enough
#define CLOSE_ENOUGH (44100>>16)
// we will use the pcm equ to find where to jump
uint8_t	 oggAudio::goToTime(uint32_t mstime)
{
uint64_t val,cur,f;
uint32_t flags,cursize;
OgAudioIndex *idx;

	val=mstime;
	val*=_wavheader->frequency;
	val/=1000; // in seconds
	aprintf("OGM:Looking for %lu ms\n",mstime);
	aprintf("Looking for %llu sample\n",val);
	
	
	//
	idx=_currentTrack->index;
	for(uint32_t i=0;i<_currentTrack->nbAudioPacket;i++)
	{		
		cur=idx[i].sampleCount;
		if(cur>val)
		{
			aprintf("Gotcha at frame %lu\n",i);
			_demuxer->setPos(idx[i].pos);
			_inBuffer=0;
			_lastFrag=NO_FRAG;
			// Now we forward till the next header is > value
			while(_demuxer->readHeaderOfType(_currentTrack->audioTrack,&cursize,&flags,&f))
			{
				if(f>val || abs(f-val)<CLOSE_ENOUGH)
				{
					aprintf("Wanted %llu",val);
					aprintf(" got %llu\n",f);					
					_lastFrag=0;
					// Clear the cross page packet if any
					if((flags & OG_CONTINUE_PACKET))
						readPacket(&cursize,_buffer,&flags);
					return 1;
				
				}
			
			}
			aprintf("Could no sync to a header close enough\n");
			return 0;
		}
		aprintf("%lu: Current %llu target %llu\n",i,idx[i].sampleCount,val);		
	}
	printf("**Failed to seek to %lu ms!**\n",mstime);
	return 0;

}
