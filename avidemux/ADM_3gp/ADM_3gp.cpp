/***************************************************************************
                          ADM_3gpp.cpp  -  description
                             -------------------
	
		Read quicktime/mpeg4 file format found in 3gpp file.
		They are limited to SQCIF/QCIF video size and can
		only contains
			video : h263 or mpeg4 
			audio : AMR or AAC


		For the mpeg4, the VOL headers are stored in esds atom
		and not in the first image
		Idem for MJPG and SVQ3

		The usual tree structure of a 3gp file is

		- ftyp
		- mdat
		- moov
			xxx
			trak
				tkhd (duration / ...)
			mdia
				hdlr (type)
				minf
					stsd header for audio/video
					stbl index to datas

	We ignore other chunk as they are not vital for our aim
	and just keep moov/mdia/minf/stsd/stbl stuff

http://developer.apple.com/documentation/QuickTime/QTFF/QTFFChap2/chapter_3_section_5.html#//apple_ref/doc/uid/DontLinkBookID_69-CH204-BBCJEIIA

    begin                : Tue Jul  2003
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "math.h"

#include <ADM_assert.h>
#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_3gp/ADM_3gp.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_3GP
#include "ADM_toolkit/ADM_debug.h"

#define _3GP_VERBOSE
// 14496-1 / 8.2.1
typedef enum MP4_Tag
{
	Tag_InitialObjDesc	=0x02,
	Tag_ES_Desc		=0x03,
	Tag_DecConfigDesc 	=0x04,
	Tag_DecSpecificInfo 	=0x05
};

uint8_t _3GPHeader::setFlag(uint32_t frame,uint32_t flags){
    UNUSED_ARG(frame);
    UNUSED_ARG(flags);
    if(flags & AVI_KEY_FRAME)
    {
		_idx[frame].intra=1;
    }
    else
    {
		_idx[frame].intra=0;
    }
	return 0;
}

uint32_t _3GPHeader::getFlags(uint32_t frame,uint32_t *flags){
	if(frame>= (uint32_t)_videostream.dwLength) return 0;

	if(fourCC::check(_videostream.fccHandler,(uint8_t *)"MJPG"))
	{
		*flags=AVI_KEY_FRAME;
	}
	else
	if(_idx[frame].intra) *flags=AVI_KEY_FRAME;
		else *flags=0;


	if(!frame) *flags=AVI_KEY_FRAME;

	return 1;
}


uint8_t  _3GPHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags)
            {
		    	getFlags(framenum,flags);
                	return getFrameNoAlloc(  framenum,ptr,framelen);
              }
uint8_t  _3GPHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
uint32_t offset=_idx[framenum].offset; //+_mdatOffset;

	if(_volHeaderLen && !framenum)
	{

 		fseek(_fd,_volHeader,SEEK_SET);
 		fread(ptr, _volHeaderLen, 1, _fd);

 		fseek(_fd,offset,SEEK_SET);
 		fread(ptr+_volHeaderLen, _idx[framenum].size, 1, _fd);
  		*framelen=_idx[framenum].size+_volHeaderLen;
 		return 1;
	}

 	fseek(_fd,offset,SEEK_SET);
 	fread(ptr, _idx[framenum].size, 1, _fd);
  	*framelen=_idx[framenum].size;
 	return 1;
}
_3GPHeader::~_3GPHeader()
{
	close();

}
uint8_t    _3GPHeader::close( void )
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
	if(_rdWav)
	{
		delete _rdWav;
		_rdWav=NULL;
	}
#define DEL(x) if(x) delete [] x;x=NULL;
	DEL(Sz);
	DEL(Co);
	DEL(Sc);
	DEL(Sn);
	DEL(Sync);
	DEL(SttsN);
	DEL(SttsC);
 	return 1;
}
//
//	Set default save value
//

_3GPHeader::_3GPHeader(void)
{
	_fd=NULL;
	_idx=NULL;
	_audioIdx=NULL;
	_volHeader=0;
	_volHeaderLen=0;
	_nbAudioChunk=0;
	_audioTrack=NULL;
	_rdWav=NULL;
	 Sz=NULL;
	 Co=NULL;
	 Sc=NULL;
	 Sn=NULL;
	 Sync=NULL;
	 SttsN=NULL;
	 SttsC=NULL;


}
uint8_t	_3GPHeader::getAudioStream(AVDMGenericAudioStream **audio)
{  
	if(!_nbAudioChunk)
	{
		*audio=NULL;
		return 0;
	}
	*audio=_audioTrack;
	return 1;
};
WAVHeader 	*_3GPHeader::getAudioInfo(void )
{ 	
	if(!_nbAudioChunk)
		return NULL; 
	return _audioTrack->getInfo();
} ;
uint32_t _3GPHeader::getNbStream(void)
{ 
	if(_nbAudioChunk)
		return 2;
	else
		return 1;
};
//______________________________________
//
// Open and recursively read the atoms
// until we got the information we want
// i.e. :
//	index for audio and video track
//	esds for mpeg4
//	size / codec used 
//
// We don't care about sync atom and all
// other stuff which are pretty useless on
// 3gp file anyway.
//______________________________________
uint8_t    _3GPHeader::open(char *name)
{
	printf("** opening 3gpp files **");	
	_fd=fopen(name,"rb");
	if(!_fd)
	{
		printf("\n cannot open %s \n",name);
		return 0;
	}
#define CLR(x)              memset(& x,0,sizeof(  x));

              CLR( _videostream);
              CLR(  _mainaviheader);
	      _isvideopresent=1;
	      _isaudiopresent=0;
    	      _videostream.dwScale=1000;
              _videostream.dwRate=10000;
              _mainaviheader.dwMicroSecPerFrame=100000;;     // 10 fps hard coded
              _videostream.fccType=fourCC::get((uint8_t *)"vids");
              _video_bih.biBitCount=24;
              _videostream.dwLength= _mainaviheader.dwTotalFrames=1;
              _videostream.dwInitialFrames= 0;
              _videostream.dwStart= 0;
              _video_bih.biWidth=_mainaviheader.dwWidth=16 ;
              _video_bih.biHeight=_mainaviheader.dwHeight=16;
              _videostream.fccHandler=fourCC::get((uint8_t *)"XXXX");
	      _video_bih.biCompression=_videostream.fccHandler;

	      _idx=NULL;
	      _volHeader=_volHeaderLen=0;
	  


	printf("\n");
	adm_atom *atom=new adm_atom(_fd);
	parseAtomTree(atom);
	delete atom;
	printf("Found video codec type :");fourCC::print(_videostream.fccHandler);printf("\n");

	if(!_idx) return 0;
	if(_nbAudioChunk)
	{
	      _isaudiopresent=1;
	      _audioTrack=new _3gpAudio( _audioIdx, _nbAudioChunk,_fd,_rdWav);
	}
	printf("3gp/mov file successfully read..\n");
	return 1;
}
uint8_t _3GPHeader::parseAtomTree(adm_atom *atom)
{
	static uint32_t current=0;
	static uint32_t nbSz,nbCo,nbSc,nbSync;
	static uint32_t duration;
	static uint32_t _lastW, _lastH;
	static uint32_t nest=0;
	static uint32_t nbStts;
	static uint32_t myScale=1000;
	uint32_t type;
	uint32_t n=0,j,wh,i,l=0;
	uint32_t tag=0xff;

	
	while(!atom->isDone())
	{
		adm_atom tom(	atom);
#ifdef _3GP_VERBOSE
					for(uint32_t o=0;o<nest;o++) printf("\t");
					printf("parsing atom ");
					fourCC::printBE(tom.getFCC());
					printf("\n");	
#endif
		nest++;
		switch((tom.getFCC()))
		{
			default:
#ifdef _3GP_VERBOSE
					for(uint32_t o=0;o<nest;o++) printf("\t");
					printf("skipping atom ");
					fourCC::printBE(tom.getFCC());
					printf("\n");	
#endif
					tom.skipAtom();
					break;
			case MKFCCR('m','d','a','t') : //'mdat':
					tom.skipAtom();
					break;

	 	// these are container atoms,
	     	// we go on , they includes other atom


			case MKFCCR('s','t','b','l'): //'stbl':
			case MKFCCR('m','d','i','a') : //'mdia':
			case MKFCCR('m','i','n','f'): //'minf':
			case MKFCCR('m','o','o','v'): //'moov':
			case MKFCCR('c','m','o','v'): //'cmov':
			case MKFCCR('M','O','V','I') : //'MOVI':
				parseAtomTree(&tom);
				break;
			case MKFCCR('s','m','h','d'): // Track audio header
					fourCC::printBE(tom.read32());
					tom.skipAtom();
					break;
			case MKFCCR('d','c','o','m') : //dcom':
					fourCC::printBE(tom.read32());
					tom.skipAtom();
					break;
			case MKFCCR('m','v','h','d') : //'mvhd':
				{
				uint32_t scale=0;
					tom.skipBytes(12);
					scale=tom.read32();
					duration=tom.read32();
					if(scale!=1000)
					{
						myScale=scale;
						printf("Warning : scale is not in ms %lu !\n",myScale);
						if(scale)
						{
								duration=1000*duration;
								duration/=scale;
						}
						scale=1000;
					}
					
					printf("Duration : %lu ms\n",duration);
					if(!duration) duration=1000;
					
				}
				tom.skipAtom();
				break;

			case MKFCCR('r','a','w',' '): //'raw ':
					tom.skipBytes(8);
					_rdWav=new WAVHeader;
					memset(_rdWav,0,sizeof(WAVHeader));
					printf("Raw audio detected\n");
					printf("Version : %u\n",tom.read16());
					printf("Revision :%u\n",tom.read16());
					printf("Vendor :%lu\n",tom.read32());
					_rdWav->channels=tom.read16();
					_rdWav->bitspersample=tom.read16();
					_rdWav->encoding=tom.read16();

					printf("Channels :%lu\n",_rdWav->channels);
					printf("S size :%lu\n",_rdWav->bitspersample);
					printf("Compression :%u\n",_rdWav->encoding);
					printf("Packet Size :%u\n",tom.read16());
					i=tom.read32();
					if(i&0xffff)
					{
						GUI_Alert("Expect troubles...\n");
					}
					if(!_rdWav->channels) _rdWav->channels=1;
					if(_rdWav->bitspersample<8) _rdWav->bitspersample=8;
					_rdWav->encoding=WAV_8BITS_UNSIGNED;
					_rdWav->byterate=(i>>16);
					_rdWav->frequency=_rdWav->byterate/((_rdWav->bitspersample>>3)*(_rdWav->channels));
					printf("Bitrate :%lu (%x)\n",i,i);
					printf("Byterate :%lu\n",_rdWav->byterate);
					printf("Frequency :%lu\n",_rdWav->frequency);
// hardcoded for now

					tom.skipAtom();
					break;
			case MKFCCR('u','l','a','w'): //'ulaw ':
					tom.skipBytes(8);
					_rdWav=new WAVHeader;
					memset(_rdWav,0,sizeof(WAVHeader));					
					_rdWav->channels=1;
					_rdWav->bitspersample=16;
					_rdWav->frequency=7980; // FIXME ?
					_rdWav->encoding=WAV_ULAW;	
					_rdWav->byterate=_rdWav->frequency;	
					tom.skipAtom();
					break;					
			case MKFCCR('m','p','4','a'): //'mp4a':
					tom.skipBytes(8);
					_rdWav=new WAVHeader;
					memset(_rdWav,0,sizeof(WAVHeader));
					_rdWav->encoding=WAV_AAC;
					
					#warning !!!!!!!!!!!!!!!
					#warning decode MP4 audio header!
					#warning !!!!!!!!!!!!!!!
					_rdWav->frequency=44100;
					_rdWav->channels=2;
					_rdWav->bitspersample=16;
					_rdWav->byterate=128000/8;
					
					tom.skipAtom();
					break;
			case MKFCCR('s','a','m','r'): //'mp4a':
					tom.skipBytes(8);
					_rdWav=new WAVHeader;
					memset(_rdWav,0,sizeof(WAVHeader));
					_rdWav->encoding=WAV_AMRNB;
					
					#warning !!!!!!!!!!!!!!!
					#warning decode WAV_AMRNB audio header!
					#warning !!!!!!!!!!!!!!!
					_rdWav->frequency=8000;
					_rdWav->channels=1;
					_rdWav->bitspersample=16;
					_rdWav->byterate=12000/8;
					
					tom.skipAtom();
					break;					
			case MKFCCR('s','t','t','s'): // time sample table stts
				
				printf("stts:%lu\n",tom.read32()); // version & flags
				nbStts=tom.read32();
				printf("Time stts atom found (%lu)\n",nbStts);
				printf("Using myscale %lu\n",myScale);
				SttsN=new uint32_t[nbStts];
				SttsC=new uint32_t[nbStts];
				double dur;
				for(i=0;i<nbStts;i++)
				{
					
					SttsN[i]=tom.read32();
					SttsC[i]=tom.read32();
					printf("stts: count:%u size:%u (unscaled)\n",SttsN[i],SttsC[i]);	
					//dur*=1000.*1000.;; // us
					//dur/=myScale;
				}
				tom.skipAtom();
				break;	
				
			case MKFCCR('s','t','s','s'): //'stss':
				printf("Sync atom found\n");
				tom.read32();
				nbSync=tom.read32();
				if(nbSync)
				{
					Sync=new uint32_t[nbSync];
					for(i=0;i<nbSync;i++)
					{
						Sync[i]=tom.read32();
					}
				}
				tom.skipAtom();
				break;
			case MKFCCR('t','r','a','k'): //'trak':
				printf("Track found\n");
				current=0;
				nbSz=0;
				nbCo=0;
				nbSc=0;
				nbSync=0;
				nbStts=0;
				//myScale=1000;
				parseAtomTree(&tom);
				switch(current)
				{
				uint32_t nbo;
				case 1:
           				_videostream.dwLength= _mainaviheader.dwTotalFrames=nbSz;
					if(duration)
              				_videostream.dwRate=(1000*1000*_videostream.dwLength)/duration;;
					buildIndex(&_idx,myScale,
							nbSz,Sz,nbCo,Co,nbSc,Sc,nbStts,SttsN,SttsC,
							Sn,&nbo);
					if(nbSync)
						sync(_idx,nbSz,nbSync,Sync);
					break;
				case 2:

					buildIndex(&_audioIdx,myScale,
							nbSz,Sz,nbCo,Co,nbSc,Sc,nbStts,SttsN,SttsC,
							Sn,&nbo);
					if(nbo)
						_nbAudioChunk=nbo;
					else
						_nbAudioChunk=nbSz;
					break;
				}
				DEL(Sz);
				DEL(Co);
				DEL(Sc);
				DEL(Sn);
				DEL(Sync);
				DEL(SttsC);
				DEL(SttsN);

				break;
		// misc atom that needs special care
			case MKFCCR('m','d','h','d'): //mdhd
				//
				{
				uint32_t tmpscale;
				double duration;
				printf("Decoding mdhd\n");
				tom.skipBytes(4); // flags + version
				tom.skipBytes(4); // creation time
				tom.skipBytes(4); // mod time
				tmpscale=tom.read32(); //
				printf("Myscale in mdhd:%lu\n",tmpscale);
				if(!tmpscale) tmpscale=600; // default
				duration=tom.read32();
				printf("duration in mdhd:%f (unscaled)\n",duration);
				duration=(duration*1000.)/tmpscale;
				printf("duration in mdhd:%f (scaled ms)\n",duration);
				tom.skipAtom();
				myScale=tmpscale;
				}
				break;
			case MKFCCR('h','d','l','r'): //'hdlr':
				tom.read32();
				tom.read32();
				type=tom.read32();
				switch(type)
				{	
				case MKFCCR('v','i','d','e')://'vide':
					current=1;
					printf("hdlr video found \n ");
					break;
				case MKFCCR('s','o','u','n'): //'soun':
					current=2;
					printf("hdlr audio found \n ");
					break;
				default:
					printf("hdlr found but ignored \n");
					fourCC::print(type);
					printf("\n");
				}
				tom.skipAtom();
				break;
			case MKFCCR('s','t','s','d') : //'stsd':
				tom.read32();
				n=tom.read32();
				for(i=0;i<n;i++)
					parseAtomTree(&tom);

				break;
			case MKFCCR('t','k','h','d'): //'tkhd':
				tom.skipBytes(12);
				printf("Track Id: %lu\n",tom.read32());
				tom.skipBytes(4);
				printf("Duration: %lu (ms)\n",(tom.read32()*1000)/myScale);
				tom.skipBytes(8);
				tom.skipBytes(4); // layers
				tom.skipBytes(40); // layers

				_lastW=tom.read32()>>16;
				_lastH=tom.read32()>>16;
				tom.skipAtom();
				printf("tkhd : %ld %ld\n",_lastW,_lastH);
				break;

			case MKFCCR('s','2','6','3') : //'s263':
				tom.skipBytes(24);
				wh=tom.read32();
				tom.skipBytes(40);


              			_video_bih.biWidth=_mainaviheader.dwWidth=wh>>16 ;
              			_video_bih.biHeight=_mainaviheader.dwHeight=wh & 0xffff;
				printf("H263 : %ld x %ld \n",_video_bih.biWidth,_video_bih.biHeight);
              			_videostream.fccHandler=fourCC::get((uint8_t *)"H263");
                        _video_bih.biCompression=_videostream.fccHandler;
				// d263 atom here	 -> ignored
				tom.skipAtom();
				break;
			case MKFCCR('S','V','Q','3'): //'SVQ3':
					// For SVQ3, the codec needs it to begin by SVQ3
					// We go back by 4 bytes to get the 4CC
						printf("SVQ3 atom found\n");
						_videoExtraLen=tom.getSize()+4;
						_videoExtraData=new uint8_t[ _videoExtraLen ];
						if(!tom.readPayload(_videoExtraData+4,_videoExtraLen-4 ))
						{
							GUI_Alert("Problem reading SVQ3 headers");
						}
						_videoExtraData[0]='S';
						_videoExtraData[1]='V';
						_videoExtraData[2]='Q';
						_videoExtraData[3]='3';
						_video_bih.biWidth=_mainaviheader.dwWidth=_lastW ;
              					_video_bih.biHeight=_mainaviheader.dwHeight=_lastH;

						printf("SVQ3 Header size : %lu",_videoExtraLen);
						_videostream.fccHandler=fourCC::get((uint8_t *)"SVQ3");
				 		_video_bih.biCompression=_videostream.fccHandler;
						//mixDump(_videoExtraData,30);
						tom.skipAtom();
				break;

			case MKFCCR('j','p','e','g'): //'jpeg':
				_volHeader=ftell(_fd);
				_volHeaderLen=tom.getSize();
				_video_bih.biWidth=_mainaviheader.dwWidth=_lastW ;
              			_video_bih.biHeight=_mainaviheader.dwHeight=_lastH;

				printf("Jpeg Header at %lu, size %lu \n",_volHeader,_volHeaderLen);
				_videostream.fccHandler=fourCC::get((uint8_t *)"MJPG");
				 _video_bih.biCompression=_videostream.fccHandler;
				tom.skipAtom();
				break;

			case MKFCCR('e','s','d','s'): //'esds':
					// in case of mpeg4 we only take
					// the mpeg4 vol header
					if(fourCC::check(_videostream.fccHandler,(uint8_t *)"DIVX"))
					{

						tom.skipBytes(4);
						tag=0xff;
						while(tag!=Tag_DecSpecificInfo && !tom.isDone())
						{
							tag=tom.read();
							l=readPackedLen(&tom);
							printf("\t Tag : %lu Len : %lu\n",tag,l);
							switch(tag)
							{
								case Tag_ES_Desc:
									tom.skipBytes(3);
									break;
								case Tag_DecConfigDesc:
									tom.skipBytes(1+1+3+4+4);	
									break;
								case Tag_DecSpecificInfo:

									_volHeader=ftell(_fd);
									_volHeaderLen=l;
									break;
							}
						}
					}
					else
					{	
						_volHeader=ftell(_fd);
						_volHeaderLen=tom.getSize();
					}


				tom.skipAtom();
				break;
			case MKFCCR('m','p','4','v'): //'mp4v':
				tom.skipBytes(24);				
				wh=tom.read32();
				printf("MP4 : %ld x %ld \n",_video_bih.biWidth,_video_bih.biHeight);
				if(wh!= (128<<16)+96 && wh!=(176<<16)+144)
				{
					if(GUI_Question("Not QCIF/SQCif, force size ?"))
					{
						if(GUI_Question("Is it QCIF ?"))
						{
							wh=(176<<16)+144;
						}
						else	wh=(128<<16)+96;

					}

				}
              			_video_bih.biWidth=_mainaviheader.dwWidth=wh>>16 ;
              			_video_bih.biHeight=_mainaviheader.dwHeight=wh & 0xffff;
              			_videostream.fccHandler=fourCC::get((uint8_t *)"DIVX");
				_video_bih.biCompression=_videostream.fccHandler;
				tom.skipBytes(50);
				parseAtomTree(&tom);
				tom.skipAtom();
				break;
			case MKFCCR('s','t','s','z'): //'stsz':
				tom.read32();
				n=tom.read32();
              			nbSz=tom.read32();

				printf("%lu frames /%lu nbsz..\n",n,nbSz);
				if(n)
					{
						Sz=new uint32_t[1];
						#warning could be smarter...
						printf("All samples are the same size...(%ld)\n",nbSz);
							Sz[0]=nbSz;
							nbSz=0;
					}
				else
				{
					Sz=new uint32_t[nbSz];
					for(j=0;j<nbSz;j++)
					{
							Sz[j]=tom.read32();

					}
				}
				tom.skipAtom();
				break;
			case MKFCCR('s','t','c','o'): //'stco':

				tom.read32();
				nbCo=tom.read32();
				Co=new uint32_t[nbCo];
				for(j=0;j< nbCo;j++)
				{
					Co[j]=tom.read32();
					aprintf("Chunk offset : %lu / %lu  : %lu\n",
						j,nbCo,Co[j]);
				}

				tom.skipAtom();
				break;
			case MKFCCR('s','t','s','c'): //'stsc':

				tom.read32();
				nbSc=tom.read32();
				Sc=new uint32_t[nbSc];
				Sn=new uint32_t[nbSc];
				for(j=0;j<nbSc;j++)
				{

					Sc[j]=tom.read32();
					Sn[j]=tom.read32();
					tom.read32();
					aprintf("\t sc  %d : sc start:%lu sc count: %lu\n",j,Sc[j],Sn[j]);
				}
				tom.skipAtom();
				break;
				
		}
		nest--;
	}
	
	return 1;
}
/*
	Brute force index position rebuilding
		from Chunk / Sample / sample per chunk

		The method is a big hairy

		We got a stsc table that says on many sample per chunk we have

			1 3
			4 7
			8 9

			It means
				1 3
				2 3
				3 4
				4 7
				5 7
				6 7
				8 9
				9 9  etc..
	We got the first chunk  that have the new sample/chunk  until it changes

	Sc[nbSc] hold the fist chunk that has   Sn[j] chunk in it

	Stsz holds the size of each sample
	If they are all the same size, nbSz=0 and nbSz[0] holds the size


http://developer.apple.com/documentation/QuickTime/QTFF/QTFFChap2/chapter_3_section_5.html#//apple_ref/doc/uid/DontLinkBookID_69-CH204-BBCJEIIA
*/
uint8_t	_3GPHeader::buildIndex(
				_3gpIndex **idx,
				uint32_t myScale,
				uint32_t nbSz,	uint32_t *Sz,
				uint32_t nbCo ,		uint32_t *Co,
				uint32_t nbSc,		uint32_t *Sc,
				uint32_t nbStts,	uint32_t *SttsN,uint32_t *SttsC,
				uint32_t *Sn, 			uint32_t *outNbChunk)

{
uint32_t chunkCount[8000];
uint32_t i,j,cur;

	*outNbChunk=0;
	aprintf("+_+_+_+_+_+\n");
	aprintf("co : %lu sz: %lu sc: %lu co[0]%lu \n",nbCo,nbSz,nbSc,Co[0]);
	aprintf("+_+_+_+_+_+\n");

	ADM_assert(Sc);
	ADM_assert(Sn);
	ADM_assert(Co);
	ADM_assert(Sz);

	// first set size
	if(!nbSz)// in that case they are all the same size, i.e.audio
	{		// ugly hack...
		uint32_t chunk_size=0;
		uint32_t cur_entry=0;
		uint32_t cur_size=0;
		uint32_t packet_size=8000;
		uint32_t nbPacket;

		chunk_size=Sz[0]/nbCo;

		*outNbChunk=nbPacket=nbCo*(1+(chunk_size/packet_size));
		printf("All the same size : %lu\n",chunk_size);
		printf("Using packet size of %lu, %lu packets ..\n",packet_size,*outNbChunk);
		// we try to avoid too big packet_size
		// so we split them in packet size number
#if 1
		*idx=new _3gpIndex[nbCo];
		memset(*idx,0,nbCo*sizeof(_3gpIndex));
		*outNbChunk=nbCo;
		_3gpIndex *ix=*idx;
		for(i=0;i<nbCo;i++)
		{
			ix[i].size=chunk_size;
			ix[i].offset=Co[i];;
			aprintf("Size : %lu offset : %lu\n",ix[i].size,ix[i].offset);
		}
		// Time to update audio (as we are probably in audio)
		// they should be all the same duration
		ADM_assert(nbStts==1);
		ADM_assert(SttsC[0]==1); // 
		printf("created %u blocks\n",nbCo);
		// update timestamps on block
		double dur=SttsN[0];
		double total=0;
		
			dur=dur/myScale;
			dur/=nbCo;		// we got the duration of one block
			dur*=1000*1000.;	// in us now
		for(uint32_t i=0;i<nbCo;i++)
		{
			(*idx)[i].time=(uint64_t)floor(total);
			total+=dur;
		
		}
		return 1;
#else
		*idx=new _3gpIndex[nbPacket];
		_3gpIndex *ix=*idx;

		for(i=0;i<nbCo;i++)
		{
			cur_size=0;
			while((cur_size+chunk_size)<chunk_size)
			{
				ix[cur_entry].size=packet_size;
				ix[cur_entry].offset=cur_size;
				cur_size+=packet_size;
				aprintf("Size : %lu offset : %lu\n",ix[cur_entry].size,ix[cur_entry].offset);
				cur_entry++;
			}
			if(cur_size<chunk_size)
			{
				ix[cur_entry].size=chunk_size-cur_size;
				ix[cur_entry].offset=Co[i]+cur_size;
				cur_entry++;
			}
		}
		return 1;
#endif
		
		
	}
	else
	{
		*idx=new _3gpIndex[nbSz];
		memset(*idx,0,nbSz*sizeof(_3gpIndex));

		for(i=0;i<nbSz;i++)
		{
			(*idx)[i].size=Sz[i];
			aprintf("\t size : %d : %lu\n",i,Sz[i]);
		}
		// if no sample to chunk we map directly
	{	// first build the # of sample per chunk table
		for(i=0;i<nbSc;i++)
		{
			for(j=Sc[i]-1;j<nbCo;j++)
			{
				chunkCount[j]=Sn[i];
			}
			aprintf("(%d) sc: %lu sn:%lu\n",i,Sc[i],Sn[i]);

		}
			for(j=0;j<nbCo;j++)
			{
				aprintf("\n count number : %d - %lu\n",j,Sn[j]);
			}
		// now we have for each chunk the number of sample in it
		cur=0;
		for(j=0;j<nbCo;j++)
		{
			int tail=0;
			aprintf("--starting at %lu , %lu to go\n",Co[j],chunkCount[j]);
			for(uint32_t k=0;k<chunkCount[j];k++)
			{
				(*idx)[cur].offset=Co[j]+tail;
				tail+=(*idx)[cur].size;
				aprintf(" sample : %d offset : %lu\n",cur,(*idx)[cur].offset);
				aprintf("Tail : %lu\n",tail);
				cur++;
			}


		}
		*outNbChunk=cur;;
	}
	}
	// Now deal with duration
	// the unit is us FIXME, probably said in header
	// we put each sample duration in the time entry
	// then sum them up to get the absolute time position
_3gp_time:	
	uint32_t nbChunk=*outNbChunk;
	if(nbStts)		//uint32_t nbStts,	uint32_t *SttsN,uint32_t SttsC,
	{
		uint32_t start=0;
		if(nbStts>1)
		{
			for(uint32_t i=0;i<nbStts;i++)
			{
				for(uint32_t j=0;j<SttsN[i];j++)
				{
					(*idx)[start].time=(uint64_t)SttsC[i];
					start++;
					ADM_assert(start<=nbChunk);
				}	
			}
		}
		else
		{
			// All same duration
			for(uint32_t i=0;i<nbChunk;i++)
				(*idx)[i].time=(uint64_t)SttsC[0]; // this is not an error!
		
		}
		// now collapse
		uint64_t total=0;
		float    ftot;
		uint32_t thisone;
		for(uint32_t i=0;i<nbChunk;i++)
		{
			thisone=(*idx)[i].time;
			ftot=total;
			ftot*=1000.*1000.;
			ftot/=myScale;
			(*idx)[i].time=(uint32_t)floor(ftot);
			total+=thisone;
			aprintf("Audio chunk : %lu time :%lu\n",i,(*idx)[i].time);
		}
		// Time is now built, it is in us
	
	
	}
	else // there is not ssts
	{
		GUI_Alert("No stts table !");
		ADM_assert(0);	
	}
	return 1;
}
//
//	That tag are coded like this
//	Each 8 bits is in fact a 7 Bits part while b7=1
// 	So we concanate them while possible
uint32_t _3GPHeader::readPackedLen(adm_atom *tom )
{
	uint32_t len=0;
	uint8_t	 b=0;

	do
	{
	b=tom->read();
	len=len<<7;
	len+=b&0x7f;
	}while(b&0x80);
	return len;
}
//
//	Update a index with the sync track that gives the intra frame position
//
uint8_t 	_3GPHeader::sync(_3gpIndex *idx,uint32_t index_size, uint32_t sync_size,uint32_t *sync)
{
	ADM_assert(index_size>=sync_size);
	for(uint32_t i=0;i<sync_size;i++)
	{
		idx[sync[i]-1].intra=1;
	}
	return 1;

}

uint8_t _3GPHeader::getFrameSize (uint32_t frame, uint32_t * size){
  if(frame >= _videostream.dwLength) return 0;
  *size = _idx[frame].size;
  return 1;
}
