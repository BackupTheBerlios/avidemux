/***************************************************************************
                          ADM_ogm.cpp  -  description
                             -------------------

		Simple "OGM" demuxer, audio part.
		This demuxer is not too smart and uses brute force approach in
		non sequential access.
		It is also only able to parse video.
		An index is built.

		For OGM all frames are not easily seekable.
		Some of them span across 2 pages or are between 2 other ones
			in the same page
			
		To kiss it, we only take the position of keyframe in ogm way,
		i.e. they start a fresh page.
		
		Othere will be forwarded from there or from the current image.
		
		
		
    begin                : Tue Apr 28 2003
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
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef __FreeBSD__
          #include <sys/types.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include "math.h"

#include "ADM_library/default.h"
#include "ADM_library/fourcc.h"
#include "ADM_ogm.h"
#include "ADM_ogmpages.h"
#include "ADM_dialog/DIA_working.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_OGM
#include "ADM_toolkit/ADM_debug.h"



oggHeader::oggHeader(void)
{
	_fd=NULL;
	_filesize=0;
	_videoTrack=0xff;
	_demux=NULL;
	_index=NULL;	
	_lastImage=0xffff;
	_lastFrag=0x0;	
	_audio=NULL;
	
	memset(&_audioTracks[0],0,sizeof(OgAudioTrack));
	memset(&_audioTracks[1],0,sizeof(OgAudioTrack));
	
	_audioTracks[0].audioTrack=_audioTracks[1].audioTrack=0xff;
	
}
oggHeader::~oggHeader()
{

	if(_demux)
		{
			delete _demux;
			_demux=NULL;
		}
	if(_index)
		{
			delete [] _index;
			_index=NULL;
		}
	for(uint32_t i=0;i<2;i++)
	{
		if(_audioTracks[i].index)
		{
			delete [] _audioTracks[i].index;
			_audioTracks[i].index=NULL;	
		}
	}
	
	
}

void oggHeader::Dump(void)
{

}
void oggHeader::_dump(void)
{

}

uint8_t oggHeader::close(void)
{

	return 1;
}


WAVHeader *oggHeader::getAudioInfo(void)
{
	if(!_audio)
		return NULL;
	return _audio->getInfo();
}
uint8_t  oggHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
	if(!_audio)
	{
	*audio=NULL;
	return 0;
	}
	*audio=_audio;
	return 1;
}
uint32_t  oggHeader::getNbStream(void)
{
	return 1;

}
uint8_t  oggHeader::needDecompress( void )
{
	return 1;
}

uint8_t  oggHeader:: getFrameSize(uint32_t frame,uint32_t *size)
{
	*size=0;
	if(!_index) return 0;
	if(frame>(uint32_t)(_videostream.dwLength-1)) return 0;
	*size= _index[frame].size;
	return 1;

}
uint8_t  oggHeader::setFlag(uint32_t frame,uint32_t flags)
{
	if(!_index) return 0;
	if(frame>(uint32_t)(_videostream.dwLength-1)) return 0;
	_index[frame].flags=flags;
	return 1;

}
uint32_t  oggHeader::getFlags(uint32_t frame,uint32_t *flags)
{
	if(!_index) return 0;
	if(frame>(uint32_t)(_videostream.dwLength-1)) return 0;
	*flags=_index[frame].flags;
	return 1;
}

uint8_t  oggHeader::  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
					uint32_t *flags)
{
	getFlags(framenum,flags);
	return getFrameNoAlloc(framenum,ptr,framelen);

}


uint8_t oggHeader::open(char *name)
{
	OGMDemuxer 	*demux;
	uint8_t  	buffer[64*1024];
	stream_header	*header;
	uint32_t 	fourcc=0;
	uint32_t 	size,flag;
	uint8_t		firstByte;
	uint64_t	framenum;
	uint8_t		id;
	uint32_t	nbAudioFrame;


	header=(stream_header *)buffer;

	/**
		We open up to 10 tracks an see what's inside
	*/

	demux=new OGMDemuxer();
	assert(demux->open(name));
	for(uint32_t track=0;track<10;track++)
	{
		if(!demux->readHeader(&size,&flag,&framenum,&id)) break;
		demux->readBytes(1,&firstByte);
		if(firstByte!=1) // not a header ?
			{
				printf("\n First packet : not a header %x?\n",firstByte);
				break;
			}
		// Grab headers

		demux->readPayload(buffer);
		// we got a first page with track id track
		// What is it ?
		char str[20];
#define OINFO(x,y) memcpy(str,header->x,y); str[y]=0;printf(#x"\t:%s \n",str);

			OINFO(streamtype,8);
			str[5]=0;

			// if it is a video track we keep it handy
			if(!strcmp(str,"video"))
			{
				_videoTrack=id;
				fourcc=fourCC::get((uint8_t *)&(header->subtype[0]))   ;
			}
			if(!strcmp(str,"vorbi"))
			{
				if( _audioTracks[0].audioTrack==0xff)
				{
					_audioTracks[0].audioTrack=id;
					_audioTracks[0].encoding=WAV_OGG;
					_audioTracks[0].channels=header->audio.channels;
					_audioTracks[0].byterate=header->audio.avgbytespersec;
					printf("Taking that track as audio track 1\n");
				}
				else
				if(_audioTracks[1].audioTrack==0xff)
				{
					_audioTracks[1].audioTrack=id;
					_audioTracks[1].encoding=WAV_OGG;
					_audioTracks[1].channels=header->audio.channels;
					_audioTracks[1].byterate=header->audio.avgbytespersec;
					printf("Taking that track as audio track 2\n");
				}
			}
			if(!strcmp(str,"audio"))
			{
				uint32_t codec;
				OINFO(subtype,4);
				sscanf(str,"%x",&codec);
				printf("Audio fourcc:%d (%x)\n",codec,codec);
				if( _audioTracks[0].audioTrack==0xff)
				{
					_audioTracks[0].audioTrack=id;
					_audioTracks[0].encoding=codec;
					_audioTracks[0].channels=header->audio.channels;
					_audioTracks[0].byterate=header->audio.avgbytespersec;
					printf("Taking that track as audio track 1\n");
				}
				else
				if(_audioTracks[1].audioTrack==0xff)
				{
					_audioTracks[1].audioTrack=id;
					_audioTracks[1].encoding=codec;
					_audioTracks[1].channels=header->audio.channels;
					_audioTracks[1].byterate=header->audio.avgbytespersec;
					printf("Taking that track as audio track 2\n");
				}
			}
			OINFO(subtype,4); // fourcc
	}
	delete demux;
	// no audio track found...
	if(_videoTrack==0xff) return 0;

	
	// Ok, we found the video track
	// time to gather some informations

	_demux=new OGMDemuxer();
	assert(_demux->open(name));
	_demux->readHeaderOfType(_videoTrack,&size,&flag,&framenum);
	_demux->readBytes(1,&firstByte);
	_demux->readPayload(buffer);


#define CLR(x)              memset(& x,0,sizeof(  x));

               CLR( _videostream);
               CLR(  _mainaviheader);

    		_videostream.dwScale=1000;
                _videostream.dwRate=25000;

#ifdef ADM_BIG_ENDIAN
	#define SWAP64(x) { uint64_t y=x;x=R32((y>>16)>>16)+((R32(y&0x0ffffffff)<<16)<<16);}
	#define SWAP32(x) x=R32(x)
	#define SWAP16(x) x=R16(x)
#else
	#define SWAP64(x) ;
	#define SWAP32(x) ;
	#define SWAP16(x) ;
#endif

		SWAP32(header->video.width);
		SWAP32(header->video.height);
		SWAP64(header->time_unit);

              _mainaviheader.dwMicroSecPerFrame=header->time_unit;;     // assuming per sample=1

	      if(!header->time_unit) header->time_unit=40000;
double fps;
		fps= header->time_unit;
		fps=1/fps;
		fps=fps*10000000*1000;
		  _videostream.dwRate=(uint32_t )floor(fps);;


              _videostream.fccType=fourCC::get((uint8_t *)"vids");
              memset( &_video_bih,0,sizeof(_video_bih));
               _video_bih.biBitCount=24;

              _videostream.fccHandler=fourcc;
              _video_bih.biCompression=0;

              _videostream.dwLength= _mainaviheader.dwTotalFrames=1; // ??
               _videostream.dwInitialFrames= 0;
               _videostream.dwStart= 0;
               _video_bih.biWidth=_mainaviheader.dwWidth=header->video.width ;
               _video_bih.biHeight=_mainaviheader.dwHeight=header->video.height;
               _video_bih.biPlanes= 24;

		_isaudiopresent=0;
		_isvideopresent=1;


		uint32_t nbFrames;
		if(!buildIndex(&nbFrames))
			{
				printf("\n could not build index..\n");
				return 0;
			}
		// Take audio 1 atm
		if(_audioTracks[1].audioTrack!=0xff || _audioTracks[0].audioTrack!=0xff)
		{
			_isaudiopresent=1;
			if(_audioTracks[1].audioTrack!=0xff
				&&_audioTracks[1].nbAudioPacket 
				&& GUI_Question("Take second track ?"))
			{
				_audio=new oggAudio( name, &_audioTracks[0],1 );
			}
			else
			{
				_audio=new oggAudio( name, &_audioTracks[0],0  );
			}
		
		
		}
		else
		{
			printf("No audio found\n");
		}
 		_videostream.dwLength= _mainaviheader.dwTotalFrames=nbFrames;
		return 1;

}

/**
		Build an index of video and store for each video frames
			- its position in file (i.e position of its first page)
			- its size
			- its flags (=0/B_FRAME/KEY_FRAME)
		Build first a temp index for 3h 30 fps movie and shrink it back afterward


*/
uint8_t oggHeader::buildIndex(uint32_t  *nb)
{
uint32_t size,flags;
uint8_t  id;
uint8_t  bte;
uint32_t cursize=0;
uint64_t curpos=0;
uint32_t frame=0;
uint64_t frame2;
uint32_t frameFlag;

uint8_t frag, *frags;

OgAudioTrack *track1,*track2;

DIA_working *work=new DIA_working("Scanning OGM");

	track1=&_audioTracks[0];
	track2=&_audioTracks[1];


OgAudioIndex *tmpaudio[2];
#define MAXINDEX (30*3600*3)

	_index=new OgIndex[MAXINDEX];
	memset(_index,0,sizeof(OgIndex)*MAXINDEX);
	printf("Temporary index created of %lu megaBytes\n",(sizeof(OgIndex)*MAXINDEX)>>20);
	
	tmpaudio[0]=new OgAudioIndex[MAXINDEX];
	tmpaudio[1]=new OgAudioIndex[MAXINDEX];
	
	printf("Audio, Temporary index created of 2*%lu megaBytes\n",(sizeof(OgAudioIndex)*MAXINDEX)>>20);
		
	frameFlag=0;
	curpos=0;
	
	while(_demux->readHeader(&size,&flags,&frame2,&id))
	{
		//printf("Id:%d\n",id);
		if( id!=_videoTrack)
		{
			uint32_t psize=size;
			if(id==track1->audioTrack ) 
				{
					OgAudioIndex *idx=&tmpaudio[0][track1->nbAudioPacket++];	
					idx->pos=_demux->getPos();
					idx->sampleCount=frame2;
					idx->dataSum=track1->trackSize;
					track1->trackSize+=size;
					
				}
			if(id==track2->audioTrack ) 
				{
					OgAudioIndex *idx=&tmpaudio[1][track2->nbAudioPacket++];	
					idx->pos=_demux->getPos();
					idx->sampleCount=frame2;
					idx->dataSum=track2->trackSize;
					track2->trackSize+=size;
					
				}
			_demux->skipBytes(psize);
			continue;
		}
		work->update(_demux->getPos()>>10,_demux->getFileSize()>>10);
		_demux->getLace(&frag,&frags);
  		// is it a fresh packed
		if(!(flags & OG_CONTINUE_PACKET))
		{
			// nope a new one
			// could be header/comment or keyframe (?)
			// read the first byte to see
			_demux->readBytes(1,&bte);
			// not a data packet
			if(bte&1) continue;
			if(bte & OG_KEYFRAME) frameFlag=AVI_KEY_FRAME;
			// skip one more bite
		//	_demux->readBytes(1,&bte);
			curpos=_demux->getPos();
   			// we can easily retrieve that one
   			_index[frame].pos=curpos;
			
			aprintf("frame : %lu,",frame);
			aprintf("Pos:%llu\n",curpos);

		}
		// get laces to see if there is stuff in it

		for(uint32_t i=0;i<frag;i++)
		{
			if(!i && (!(flags & OG_CONTINUE_PACKET)))
			{
				// remove 2 header bytes
				cursize+=frags[i]-1;
			}
			else
			cursize+=frags[i];
			// it means end of a frame
			if(frags[i]!=0xff)
			{
				_index[frame].size=cursize;
				_index[frame].flags=frameFlag;
				aprintf("Detected frame : %lu,size %lu  page count : %lu\n",
					frame,cursize,frame2);
				frame++;
				cursize=0;
				frameFlag=0;
			}

		} 
		//if(frame>6000) break;
	}
	// Collect last bits of audio
	
	delete work;
	*nb=frame;
	printf("\n Found %lu Frame\n",frame);
	if(!frame) return 0;

	// shrink index
	OgIndex			*tmp;
	tmp=new OgIndex[frame];
	memcpy(tmp,_index,sizeof(OgIndex)*frame);
	printf("Final index created of %lu megaBytes\n",(sizeof(OgIndex)*frame)>>20);
	
	// shrink audio indexes
	track1->index=new OgAudioIndex[ track1->nbAudioPacket ];
	track2->index=new OgAudioIndex[ track2->nbAudioPacket ];
	memcpy(track1->index,tmpaudio[0],sizeof(OgAudioIndex)*track1->nbAudioPacket);
	memcpy(track2->index,tmpaudio[1],sizeof(OgAudioIndex)*track2->nbAudioPacket);
	
	delete [] tmpaudio[0];
	delete [] tmpaudio[1];
	
	printf("Final index for audio track1 of %lu megaBytes\n",
			(sizeof(OgAudioIndex)*track1->nbAudioPacket)>>20);
	printf("Final index for audio track2 of %lu megaBytes\n",
			(sizeof(OgAudioIndex)*track2->nbAudioPacket)>>20);
	
	
	printf("Audio track1: %lu bytes seen, Audio Track2 : %lu bytes seen\n",
		track1->trackSize,track2->trackSize);	
	delete [] _index;
	_index=tmp;

	return 1;
}
/**
		For OGM file we can either read sequentially
		or rewind to a fresh page a forward from then

*/
uint8_t  oggHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
	uint64_t f;
	
	if(!_index) return 0;
	if(framenum>(uint32_t)(_videostream.dwLength-1)) return 0;

	uint32_t size,len,flags,seek,cursize=0;
	uint8_t bte,frag,*frags;


	aprintf("****************** frame %lu requested **************\n",framenum);
	len=*framelen=_index[framenum].size;
	// continue the previously read image ?
	if((_lastImage+1)==framenum)
	{
		seek=framenum;
		// read last frags
		_demux->getLace(&frag,&frags);
		if(_lastFrag>=frag)
		{
			printf("\n Either end of stream or a bug... %lu %lu \n",_lastFrag,_lastImage);
			_lastImage=0xfffffff;
			_lastFrag=0xfffff;
			return 0;
		};
		//

		for(uint32_t i=_lastFrag+1;i<frag;i++)
		{
			aprintf("\t cont :%lu -> %02d ( %lu to go)\n",i,frags[i],len-frags[i]);
			cursize=frags[i];
			// get the frame ?
			_demux->readBytes(cursize,ptr);
			len-=cursize;
			ptr+=cursize;
			_lastFrag=i;

			aprintf(" Last frag : %lu\n",_lastFrag);
			if(frags[i]!=0xff )
			{

				aprintf("--short seeking (%d)  \n",i);
				if( len==0)
				{
					_lastImage=framenum;
					aprintf("\n Got short frame\n");
					return 1;
				}
				else
				{
					printf("\n Short Inconsistency : When requesting frame "
					"%lu we got len to go %lu and reached a new frame, lastFrag=%lu/%u\n",
										framenum,len,_lastFrag,frag);
										
					printf("For information the frame len is %lu\n",*framelen);
					assert(0);
				}

			}
		}
	}
	else
	{
		// look last seekable frame
		seek=framenum;
		while(!_index[seek].pos) seek--;

		aprintf("last seekable : %lu\n",seek);

		// Jump to seek
		assert(_demux->setPos(_index[seek].pos));
		// and now read forward
	}

	while(_demux->readHeaderOfType(_videoTrack,&size,&flags,&f))
	{
		aprintf("\n page : %lu\n",f);
		_demux->getLace(&frag,&frags);
  		// is it a fresh packed
		if(!(flags & OG_CONTINUE_PACKET))
		{
			// nope a new one
			// could be header/comment or keyframe (?)
			// read the first byte to see
			_demux->readBytes(1,&bte);
			// not a data packet
			if(bte&1) continue;
			// skip one more bite
			//_demux->readBytes(1,&bte);
		}
		// get laces to see if there is stuff in it
		aprintf("__ frags__ : %u \n",frag);
		for(uint32_t i=0;i<frag;i++)
		{
			aprintf("\t %lu -> %02d ( %lu to go)\n",i,frags[i],len-frags[i]);
			if(!i && (!(flags & OG_CONTINUE_PACKET)))
			{
				// remove 2 header bytes
				cursize=frags[i]-1;
			}
			else
				cursize=frags[i];
			// get the frame ?
			if(seek==framenum)
			{
				_demux->readBytes(cursize,ptr);
				len-=cursize;
				ptr+=cursize;
			}else
			{
				_demux->skipBytes(cursize);
			}
			_lastFrag=i;
			aprintf(" Last frag : %lu\n",_lastFrag);
			// got it all ?

			if(seek>framenum)
			{
				printf("\n When requesting frame %lu we got len to go %lu and reached a new frame, lastFrag=%lu/%u\n",
										framenum,len,_lastFrag,frag);
				assert(0);
			}

			// it means end of a frame
			if(frags[i]!=0xff )
			{

				aprintf("--seeking (%ld) got : %lu \n",i,seek);
				if(seek==framenum && len==0)
				{
					_lastImage=framenum;
					return 1;
				}
				else if(seek==framenum)
				{
					printf("\n Inconsistency : When requesting frame %lu we got len to go %lu and reached a new frame, lastFrag=%lu/%u\n",
										framenum,len,_lastFrag,frag);
					printf("For information the frame len is %lu\n",*framelen);
					assert(0);
				}
				seek++;
			}

		}

	}
	_lastImage=0xffffff;
	_lastFrag=0xffffff;
	return 0;
}
