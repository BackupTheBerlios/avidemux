/***************************************************************************
                          ADM_OpenDML.cpp  -  description
                             -------------------
	
		OpenDML Demuxer
		
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
#include <assert.h>
#include <string.h>
#include "config.h"
#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ODML
#include "ADM_toolkit/ADM_debug.h"
#ifdef ADM_DEBUG
	//#define OPENDML_VERBOSE
#endif


uint8_t OpenDMLHeader::setFlag(uint32_t frame,uint32_t flags)
{
    UNUSED_ARG(frame);
    UNUSED_ARG(flags);
    
	_idx[frame].intra=flags;    
	return 1;
}

uint32_t OpenDMLHeader::getFlags(uint32_t frame,uint32_t *flags)
{
*flags=0;
	if(frame>= (uint32_t)_videostream.dwLength) return 0;

	if(fourCC::check(_videostream.fccHandler,(uint8_t *)"MJPG"))
	{
		*flags=AVI_KEY_FRAME;
	}
	else
	{
		if(_idx[frame].intra & AVI_KEY_FRAME) 		*flags=AVI_KEY_FRAME;
		else if(_idx[frame].intra & AVI_B_FRAME)	*flags=AVI_B_FRAME;
	}
	if(!frame) *flags=AVI_KEY_FRAME;
	return 1;
}
 uint8_t  OpenDMLHeader::getFrameSize(uint32_t frame,uint32_t *size) 
{
	*size=0;
	if(frame>= (uint32_t)_videostream.dwLength) return 0;
	*size=_idx[frame].size;
	return 1;
}

uint8_t OpenDMLHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
	if(_videoExtraLen)
	{
		*len=_videoExtraLen;
		*data=_videoExtraData;
		return 1;
	}
	*len=0;
	*data=NULL;
	return 0;

}

uint8_t  OpenDMLHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags)
            {
		    	getFlags(framenum,flags);
                	return getFrameNoAlloc(  framenum,ptr,framelen);
              }
uint8_t  OpenDMLHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
uint64_t offset=_idx[framenum].offset; //+_mdatOffset;
	 		

 	fseeko(_fd,offset,SEEK_SET);
 	fread(ptr, _idx[framenum].size, 1, _fd);
  	*framelen=_idx[framenum].size;
	aprintf("Size: %lu\n",_idx[framenum].size);
	if(offset & 1) printf("odd!\n");
 	return 1;
}
OpenDMLHeader::~OpenDMLHeader()
{
	close();

}
uint8_t    OpenDMLHeader::close( void )
{
	if(_fd)
 		{
               	fclose(_fd);
             	}
              _fd=NULL;
	if(_idx)
	{
		delete [] _idx;
		_idx=NULL;
	}
	if(_audioIdx)
	{
		delete [] _audioIdx;
		_audioIdx=NULL;
	}	
	if(_videoExtraData)
	{
		delete [] _videoExtraData;
		_videoExtraData=NULL;
	}
	if(_audioExtraData)
	{
		delete [] _audioExtraData;
		_audioExtraData=NULL;
	}
	if(_audioTrack)
	{
		delete _audioTrack;
		_audioTrack=NULL;
	}
 	return 1;
}
//
//	Set default save value
//

OpenDMLHeader::OpenDMLHeader(void)
{
	_fd=NULL;
	_idx=NULL;	
	_audioIdx=NULL;
	_nbAudioChunk=0;
	_nbTrack=0;
	memset(&(_Tracks[0]),0,sizeof(_Tracks));
	memset(&_regularIndex,0,sizeof(_regularIndex));
	_videoExtraData=_audioExtraData=NULL;
	_videoExtraLen=_audioExtraLen=0;
	_nbAudioChunk=0;
	_audioTrack=NULL;
	_reordered=0;

	

}
uint8_t	OpenDMLHeader::getAudioStream(AVDMGenericAudioStream **audio)
{  	
	if(_isaudiopresent)
	{
		assert(_audioTrack);
		*audio=_audioTrack;
		return 1;
	}
	*audio=NULL;
	return 0;	
};
WAVHeader 	*OpenDMLHeader::getAudioInfo(void )
{ 	
	
	if(_isaudiopresent)
		return &_wavHeader; 
	else
		return NULL;
	
} ;
uint32_t OpenDMLHeader::getNbStream(void)
{ 
		if(_isaudiopresent)
			return 2;
		else
			return 1;
};
//______________________________________
//
// Open and get the headears/index built
// along way
//______________________________________
uint8_t    OpenDMLHeader::open(char *name)
{
uint8_t badAvi=0;
uint32_t rd;

	printf("** opening OpenDML files **");	
	_fd=fopen(name,"rb");
	if(!_fd)
	{
		printf("\n cannot open %s \n",name);
		return 0;
	}
#define CLR(x)              memset(& x,0,sizeof(  x));

              CLR( _videostream);
              CLR( _mainaviheader);
	      _isvideopresent=1;
	      _isaudiopresent=0;    	     	      	 	      
	      
		_nbTrack=0;
		riffParser *parser=new riffParser(name);
		
		if(MKFCC('R','I','F','F')!=(rd=parser->read32()))
			{
				printf("Not riff\n");badAvi=1;
				printf("%lx != %lx\n",rd,MKFCC('R','I','F','F'));
			}
		parser->read32();
		if(MKFCC('A','V','I',' ')!=parser->read32())
			{
				printf("Not Avi\n");badAvi=1;
			}
		
		if(!badAvi)
			{
				walk(parser);	
			
			}					
		delete parser;
		aprintf("Found %d tracks\n:-----------\n",_nbTrack);
		// check if it looks like a correct avi
		if(!_nbTrack) badAvi=1;
		
		// if we are up to here -> good avi :)
		if(badAvi)
		{
			printf("FAIL\n");
			return 0;
		}
		// now read up each parts...
		//____________________________
		
#define DUMP_TRACK(i) aprintf(" at %llu (%llx) size : %llu (%llx)\n", \
				_Tracks[i].strh.offset,\
				_Tracks[i].strh.offset,\
				_Tracks[i].strh.size,\
				_Tracks[i].strh.size);
								
		for(uint32_t i=0;i<_nbTrack;i++)
		{
			DUMP_TRACK(i);		
		}		
		
		uint32_t vidTrack=0xff;
		// search wich track is the video one
		// and load it to _videoheader
		
		for(uint32_t i=0;i<_nbTrack;i++)
		{
			fseeko(_fd,_Tracks[i].strh.offset,SEEK_SET);
			if(_Tracks[i].strh.size!=sizeof(_videostream))
			{
				printf("Mmm(1) we have a bogey here, size mismatch : %lu \n",_Tracks[i].strh.size);
				printf("expected %d\n",sizeof(_videostream));
				if(_Tracks[i].strh.size<sizeof(_videostream)-8) // RECT is not mandatory
				{
					GUI_Alert("Maformed header!");
					return 0;
				}		
				printf("Trying to continue anyway\n");			
			}
			fread(&_videostream,sizeof(_videostream),1,_fd);
#ifdef ADM_BIG_ENDIAN
				Endian_AviStreamHeader(&_videostream);
#endif
			if(_videostream.fccType==MKFCC('v','i','d','s'))
				{
					vidTrack=i;
					printf("Video track is %ld\n",i);
					break;
				}		
		}
		if(0xff==vidTrack)
		{
			printf("Could not identify video track!");
			return 0;
		}
		
		// STOP HERE -> Alex <-
		//return 0;
		// STOP HERE -> Alex <-
		
		
		
									

		// then bih stuff
		int32_t extra;
		_fd=fopen(name,"rb");
		
		fseeko(_fd,_Tracks[vidTrack].strf.offset,SEEK_SET);		
		extra=_Tracks[vidTrack].strf.size-sizeof(_video_bih);
		if(extra<0)
		{	
			printf("bih is not big enough (%lu/%lu)!\n",_Tracks[vidTrack].strf.size,sizeof(_video_bih));
			return 0;
		}
		fread(&_video_bih,sizeof(_video_bih),1,_fd);
#ifdef ADM_BIG_ENDIAN
		Endian_BitMapInfo(&_video_bih);
#endif
		if(extra>0)
		{				
			_videoExtraLen=extra;		
			_videoExtraData=new uint8_t [extra];
			fread(_videoExtraData,extra,1,_fd);
		}
		_isvideopresent=1;
		//--------------------------------------------------
		//	Read audio trak info, select if there is
		//	several
		//--------------------------------------------------
		// and audio track
		uint32_t audioTrack=0xff;	
		uint32_t audioTrackNumber=0;
		if(_mainaviheader.dwStreams>=2)
		{
			// which one is the audio track, is there several ?
			switch(countAudioTrack())
			{
				case 0: printf("No audio track found.\n");
					break;
				case 1: printf("One audio track found\n");
					audioTrack=searchAudioTrack(0);
					audioTrackNumber=0;
					break;
				default: printf("Several audio tracks found\n");
					uint32_t t;
					t=GUI_Question("Take second audio track ?");
					audioTrackNumber=t;
					audioTrack=searchAudioTrack(t);
			}
			if(audioTrack!=0xff) // one is marked
			{
				// Read information
				printf("Taking audio track : %lu %lu\n",audioTrackNumber,audioTrack);
				_isaudiopresent=1;
				fseeko(_fd,_Tracks[audioTrack].strh.offset,SEEK_SET);

				if(_Tracks[audioTrack].strh.size!=sizeof(_audiostream));
				{
				
					printf("Mmm(2) we have a bogey here, size mismatch : %lu \n"
						,_Tracks[audioTrack].strh.size);
					printf("expected %d\n",sizeof(_audiostream));
					if(_Tracks[audioTrack].strh.size<sizeof(_audiostream)-8)
					{
						GUI_Alert("Maformed header!");
						return 0;
					}		
					printf("Trying to continue anyway\n");			
				}
				fread(&_audiostream,sizeof(_audiostream),1,_fd);
#ifdef ADM_BIG_ENDIAN
				Endian_AviStreamHeader(&_audiostream);
#endif
				if(_audiostream.fccType!=MKFCC('a','u','d','s'))
				{	
					printf("Not an audio track!\n");
					return 0;
				}
				// now read extra stuff
				fseeko(_fd,_Tracks[audioTrack].strf.offset,SEEK_SET);		
				extra=_Tracks[audioTrack].strf.size-sizeof(_wavHeader);
				if(extra<0)
				{	
					printf("WavHeader is not big enough (%lu/%lu)!\n",
					_Tracks[audioTrack].strf.size,sizeof(WAVHeader));
					return 0;
				}
				fread(&_wavHeader,sizeof(WAVHeader),1,_fd);				
#ifdef ADM_BIG_ENDIAN
				Endian_WavHeader(&_wavHeader);
#endif
				if(extra>2)
				{				
					fgetc(_fd);fgetc(_fd);
					extra-=2;
					_audioExtraLen=extra;		
					_audioExtraData=new uint8_t [extra];
					fread(_audioExtraData,extra,1,_fd);
				}					
			
			}		
		}
		
		// now look at the index stuff
		// there could be 3 cases:
		// 1- It is a openDML index, meta index  + several smaller index
		// 2- It is a legacy index (type 1 , most common)
		// 3- It is a broken index or no index at all
		//
		// If it is a openDML index we will find a "indx" field in the Tracks
		// Else we will find it in _regularIndex Track
		// Since openDML often also have a regular index we will try open DML first
		
		uint8_t ret=0;
		Dump();
		
		// take the size of riff header and actual file size
		uint64_t riffSize;
		fseeko(_fd,0,SEEK_END);		
		_fileSize=ftello(_fd);
		fseeko(_fd,0,SEEK_SET);
		read32();
		riffSize=(uint64_t )read32();
				
		
		// 1st case, we have an avi < 4 Gb
		// potentially avi type 1		
		if((_fileSize<4*1024*1024*1024LL)&&
			// if riff size is ~ fileSize try regular index
			 (abs(riffSize-_fileSize)<1024*1024))
			{
				printf("Size looks good, maybe type 1 avi\n");
				if(!ret && !_Tracks[vidTrack].indx.offset) // try regular avi
					ret=indexRegular(vidTrack,audioTrack,audioTrackNumber);		
				if (!ret && _Tracks[vidTrack].indx.offset)	
					ret=indexODML(vidTrack,audioTrack,audioTrackNumber);
				if(!ret) // re-index!
					ret=indexReindex(vidTrack,audioTrack,audioTrackNumber);		
			} // else try openDML/reindexing
		else
			{
				printf("Size mismatch, not type 1 avi\n");
				if (!ret && _Tracks[vidTrack].indx.offset)	
					ret=indexODML(vidTrack,audioTrack,audioTrackNumber);
				if(!ret) // re-index!
					ret=indexReindex(vidTrack,audioTrack,audioTrackNumber);			
			
			}
				
		if(!ret) 
		{
			printf("Cound not index it properly...\n");
			return 0;
		
		}				
		if(!_audioIdx) _isaudiopresent=0;
		else
		{
			// build audio stream
		 	_audioTrack = new AVDMAviAudioStream(_audioIdx,
   						_nbAudioChunk,
						_fd,
						&_wavHeader,						
						0,
						_audioExtraLen,_audioExtraData);
		
		}
		_videostream.fccHandler=_video_bih.biCompression;
		printf("\nOpenDML file successfully read..\n");
		
		return ret;
	
}
/*
	Count how many audio track is found

*/
uint32_t OpenDMLHeader::countAudioTrack( void )
{
AVIStreamHeader tmp;
uint32_t count=0;
	for(uint32_t i=0;i<_nbTrack;i++)
	{		
			fseeko(_fd,_Tracks[i].strh.offset,SEEK_SET);
			if(_Tracks[i].strh.size!=sizeof(tmp))
			{
				
				printf("Mmm(3) we have a bogey here, size mismatch : %lu \n",_Tracks[i].strh.size);
				printf("expected %d\n",sizeof(tmp));
				if(_Tracks[i].strh.size<sizeof(tmp)-8)
				{
					GUI_Alert("Maformed header!");
					return 0;
				}		
				printf("Trying to continue anyway\n");			
			}
			
			
			fread(&tmp,sizeof(tmp),1,_fd);
#ifdef ADM_BIG_ENDIAN
			Endian_AviStreamHeader(&tmp);
#endif
			if(tmp.fccType==MKFCC('a','u','d','s'))
				{
					count++;
					printf("Track %lu/%lu is audio\n",i,_nbTrack);	
				}
			else
			{
					printf("Track %lu/%lu :\n",i);fourCC::print(tmp.fccType);printf("\n");
					
			}			
	}
	return count;
}
/*
	return index of the which-th track

*/
uint32_t  OpenDMLHeader::searchAudioTrack(uint32_t which)
{
AVIStreamHeader tmp;
uint32_t count=0;
	for(uint32_t i=0;i<_nbTrack;i++)
	{		
			fseeko(_fd,_Tracks[i].strh.offset,SEEK_SET);
			if(_Tracks[i].strh.size!=sizeof(_videostream));
			{
				
				printf("Mmm(4) we have a bogey here, size mismatch : %lu \n",_Tracks[i].strh.size);
				printf("expected %d\n",sizeof(_videostream));
				if(_Tracks[i].strh.size<sizeof(_videostream)-8)
				{
					GUI_Alert("Maformed header!");
					return 0;
				}		
				printf("Trying to continue anyway\n");			
			}
			fread(&tmp,sizeof(tmp),1,_fd);
#ifdef ADM_BIG_ENDIAN
			Endian_AviStreamHeader(&tmp);
#endif
			if(tmp.fccType==MKFCC('a','u','d','s'))
				{
					if(!which) return i;
					which--;					
				}			
	}
	assert(0);
	return 0;
}
/*
	Dump in human readable form the content of all headers
		
*/

void OpenDMLHeader::Dump( void )
{
#define X_DUMP(x) printf(#x":\t\t:%ld\n",_mainaviheader.x);
    	X_DUMP(dwStreams);
    	X_DUMP(dwMicroSecPerFrame) ;
	X_DUMP(dwMaxBytesPerSec);
    	X_DUMP(dwPaddingGranularity);
    	X_DUMP(dwFlags);
    	X_DUMP(dwTotalFrames);
    	X_DUMP(dwInitialFrames);	
    	X_DUMP(dwWidth);
    	X_DUMP(dwHeight);
 	printf("\n");
#undef X_DUMP
#define X_DUMP(x) printf("\n "#x":\t\t:%ld",_videostream.x);

				
	printf(  "video stream attached:\n" );
	printf(  "______________________\n" );	
	printf(" Extra Data  : %ld",_videoExtraLen);
	if(_videoExtraLen)
	{
		mixDump( _videoExtraData, _videoExtraLen);
	}

	printf("\n fccType     :");
	fourCC::print(_videostream.fccType);
	printf("\n fccHandler :");
	fourCC::print(_videostream.fccHandler);

	X_DUMP(dwFlags);
	X_DUMP(dwInitialFrames);
	X_DUMP(dwRate);
	X_DUMP(dwStart);
	X_DUMP(dwSampleSize);
	X_DUMP(dwScale);
	X_DUMP(dwLength); 
    	X_DUMP(dwQuality);
	X_DUMP(dwSampleSize);
	printf("\n");
	  	  
#undef X_DUMP
#define X_DUMP(x) printf(#x":\t\t:%ld\n",(long int)_video_bih.x);
    	X_DUMP( biSize);
       	X_DUMP( biWidth);
       	X_DUMP( biHeight);
       	X_DUMP( biBitCount);
       	X_DUMP( biCompression);fourCC::print(_video_bih.biCompression);printf("\n");
        X_DUMP( biSizeImage);
       	X_DUMP( biXPelsPerMeter);
       	X_DUMP( biYPelsPerMeter);
       	X_DUMP( biClrUsed);
    
	if(_isaudiopresent)
     	{
#undef X_DUMP
#define X_DUMP(x) printf("\n "#x":\t\t:%ld",_audiostream.x);



	   printf(  "\naudio stream attached:\n" );
	   printf(  "______________________\n" );


	  printf("\n fccType     :");
	  fourCC::print(_audiostream.fccType);
	  printf("\n fccHandler :");
	  fourCC::print(_audiostream.fccHandler);
	  printf("\n fccHandler :0x%lx", _audiostream.fccHandler);


	  X_DUMP(dwFlags);
	  X_DUMP(dwInitialFrames);
	  X_DUMP(dwRate);
	  X_DUMP(dwScale);
	  X_DUMP(dwStart);
	  X_DUMP(dwLength);
	  X_DUMP(dwSuggestedBufferSize);
	  X_DUMP(dwQuality);
	  X_DUMP(dwSampleSize);
	  
	      	
	  
#undef X_DUMP
#define X_DUMP(x) printf("\n "#x":\t\t:%lu",_wavHeader.x);

	  X_DUMP(encoding);
	  X_DUMP(channels);	/* 1 = mono, 2 = stereo */
	  X_DUMP(frequency);	/* One of 11025, 22050, or 44100 Hz */
	  X_DUMP(byterate);	/* Average bytes per second */
	  X_DUMP(blockalign);	/* Bytes per sample block */
	  X_DUMP(bitspersample);	/* One of 8, 12, 16, or 4 for ADPCM */
	  
	  printf(" Extra Data  : %ld\n",_audioExtraLen);
	if(_audioExtraLen)
	{
		mixDump( _audioExtraData, _audioExtraLen);
	}
	printf("\n");

	  
	 if(_wavHeader.encoding==WAV_MP3 && _audiostream.dwScale==1152)
      		{
			GUI_Alert("This looks like MP3 vbr.\n Use audio->build time map");
      		}

      	}				
}
#define PAD	for(int j=0;j<nest;j++) aprintf("\t");
/*
	Recursively climb a riff tree and not where are
	the interesting informations to be read later on

*/
void OpenDMLHeader::walk(riffParser *p)
{
	uint32_t fcc,len;
	static uint32_t nest=0;

	nest++;
	while(!p->endReached())
	{
	fcc=p->read32();
	len=p->read32();
#ifdef OPENDML_VERBOSE	
	for(int j=0;j<nest;j++) aprintf("\t");
	PAD;
	aprintf("Entry  :");fourCC::print(fcc);aprintf(" Size: %lu (%lx)\n",len,len);
#endif	
	switch(fcc)
	{
		case MKFCC('a','v','i','h'):
				aprintf("main header found \n");
				if(len!=sizeof(_mainaviheader))
				{
					printf("oops : %d / %d\n",len,sizeof(_mainaviheader));
				}
				p->read(len,(uint8_t *)&_mainaviheader);

#ifdef ADM_BIG_ENDIAN
			Endian_AviMainHeader(&_mainaviheader);
#endif
				printf("\n Main avi header :\n");				
				break;
		case MKFCC('i','d','x','1'):
				printf("Idx1 found\n");
				_regularIndex.offset=p->getPos();
				_regularIndex.size=len;
				return;
				break;				
		case MKFCC('R','I','F','F'):
				uint32_t sublen;
				p->read32();
				{

							riffParser *n;
							n=new riffParser(p,len-4);
							walk(n);
							delete n;
				}
				p->curPos=ftello(p->fd);
				break;
		case MKFCC('s','t','r','f'):
					_Tracks[_nbTrack].strf.offset=p->getPos();				
					_Tracks[_nbTrack].strf.size=len;
					p->skip(len);
					DUMP_TRACK(_nbTrack);
					break;
		case MKFCC('s','t','r','h'):
					_Tracks[_nbTrack].strh.offset=p->getPos();				
					_Tracks[_nbTrack].strh.size=len;
					p->skip(len);
					DUMP_TRACK(_nbTrack);
					break;
		case MKFCC('i','n','d','x'):
					printf("Indx found\n");
					_Tracks[_nbTrack].indx.offset=p->getPos();
					_Tracks[_nbTrack].indx.size=len;
					p->skip(len);
					break;					
							
		case MKFCC('L','I','S','T'):		
				{
				uint32_t sub,l;
					PAD;
					aprintf("\tType:");
					sub=p->read32();
					len-=4;
#ifdef OPENDML_VERBOSE					
					fourCC::print(sub);printf(" (%d bytes to go)\n",len);
#endif					
					if(sub==MKFCC('m','o','v','i'))
					{
						_movi.offset=p->getPos();
						p->skip(len);	
					}
					else
						{
							riffParser *n;
							n=new riffParser(p,len);
							walk(n);
							delete n;
						}
						p->curPos=ftello(p->fd);
					if(MKFCC('s','t','r','l')==sub)
					{
 						_nbTrack++;
					}
					
				}
				break;
		default:
			PAD;
			aprintf("\tskipping %lu bytes\n");
			p->skip(len);
			break;
		
	}
	}
	nest--;
}

