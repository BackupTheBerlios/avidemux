/***************************************************************************
                          ADM_mpegscan.cpp  -  description
                             -------------------
                             
		Deals with indexed mpeg file (~ DVD2AVI way)                             
                             
    begin                : Sat Oct 12 2002
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
#include <math.h>

#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_library/fourcc.h"
#include "ADM_mpeg2dec/ADM_mpegscan.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"
#include "ADM_mpeg2dec/ADM_mpegAudio.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"

#define W _mainaviheader.dwWidth
#define H _mainaviheader.dwHeight


mpeg2decHeader::mpeg2decHeader  (void)
{
	
	_video_track_id=0xe0;
	_nbIFrame=0;
	 	memset(&_mainaviheader,0,sizeof(_mainaviheader));
		memset(&_videostream,0,sizeof(_videostream));


		_startSeq=NULL;
		_indexMpegPTS=NULL;
		_audioStream=NULL;
}
//
// Delete everything associated (cache...demuxer...)
//
mpeg2decHeader::~mpeg2decHeader  ()
{
	  #define FREETAB(x) {if(x) {delete [] x;x=NULL;}}
	  #define FREE(x) {if(x) {delete  x;x=NULL;}}
	  
	  FREE(demuxer);
	  FREETAB(_indexMpegPTS);

	  #undef FREE
	  #undef FREETAB

}


void 				mpeg2decHeader::Dump(void)
{
	
}
uint32_t 		mpeg2decHeader::getNbStream(void)
{
	return 1;
}
uint8_t 		mpeg2decHeader::needDecompress(void)
{
		return 1;
}                 

/*
  	Open the index file and grab all informations


*/
uint8_t			mpeg2decHeader::open(char *name)
{
		FILE *file;
	
		uint32_t w=720,h=576,fps=0;
		uint8_t type;
		uint64_t abs;
		uint32_t dummy,audio=0;
	  	
	  	char string[1024]; //,str[1024];;
	
			
		printf("\n  opening d2v file : %s\n",name);
		file=fopen(name,"rt");
		if(!file) 
			{
				printf("\n Error !!\n");
				return 0;
			}
		
		fgets(string,1023,file);   	// File header
	   	sscanf(string,"IDXM %c",&type);
	   // not an elementary stream nor program stream -> WTF
	   	if((type!='E' ) && (type!='P'))
	   	{
			printf(" Unknown stream type\n");
			fclose(file);
			return 0;		
		}
		
	  	fgets(string,1023,file);   	// # of I frames
	  	sscanf(string,"%X",&_nbFrames);
	  	printf(" %d I-Frames\n", _nbFrames);	  			
		
		fgets(string,1023,file);   	// nb file
		printf(" Nb files : %s\n",string);
		fgets(string,1023,file);   	// File header
		string[strlen(string)-1]=0; // remove CR
		printf("\t  vob :%s\n",string);

		switch(type)
		{
			case 'E':		
							demuxer=new ADM_mpegDemuxerElementaryStream;
							break;
			case 'P':			
							// video as primary, audio as secondary
							demuxer=new ADM_mpegDemuxerProgramStream(0xe0,0xc0);
							break;
			default: assert(0);
		}
		
		if(!demuxer->open(string))
		{
				printf("\n cannot open mpeg >%s<\n",string);
				delete demuxer;
				demuxer=NULL;
				fclose(file);
				return 0;
		}
		fgets(string,1023,file);   	// Size x*y
		sscanf(string,"%u %u %u",&w,&h,&fps);
		printf(" %d x %d %u format \n",w,h,fps);

		_indexMpegPTS=new MFrame[_nbFrames+1];
		if(!_indexMpegPTS)
			{
					GUI_Alert("out of memory!!");
					assert(0);
			}

		
		// Now I Offset + nb B/P frames		
		uint8_t t;
		uint64_t of;
		uint32_t sz;

		DIA_working *work=new DIA_working("Opening mpeg..");

		for(uint32_t y=0;y<_nbFrames;y++)
		{
		     fgets(string,1023,file);     

				  sscanf(string,"%c %llX %u",
				  &t,	 	&of,    	&sz);
				   if(t!=(uint8_t)'I')
				   { 
				    _indexMpegPTS[y].type=t;
				    _indexMpegPTS[y].offset=of;
				    _indexMpegPTS[y].size=sz;
   				    _indexMpegPTS[y].absoffset=0;

			        }
			        else
			        {
						  sscanf(string,"%c %llX %llX %X %u",
														  &t,	 	&of, &abs,&dummy,   	&sz);

					audio=dummy;
					_indexMpegPTS[y].type=t;
				    	_indexMpegPTS[y].offset=of;
				   	 _indexMpegPTS[y].absoffset=abs;
				   	 _indexMpegPTS[y].size=sz;
				    	_nbIFrame++;
				  //  printf("I: %llX %llX\n",	 _indexMpeg[y].offset,_indexMpeg[y].absoffset);			    
			        			
					 }

					work->update(  y,_nbFrames)   ;
			}
			delete work;
		
			// Drop the last P/B frames as we won't be able to decode them
			// (last frame must be an I frame for decodeGop to work)
			uint32_t dropped=0;
			for(uint32_t y=_nbFrames-1;y>0;y--)
			{
					if(_indexMpegPTS[y].type!='B') break;
					_nbFrames--;
					dropped++;
			}
			printf("Dropping %d last B/P frames\n",dropped);			
			printf(" Creating start sequence (%llu)..\n",_indexMpegPTS[0].offset);
			
			//
			
			uint32_t scancode=0;
			uint32_t count=0,found=0;
			uint32_t firstPic=_indexMpegPTS[0].size;
			uint8_t *tmp=new uint8_t[firstPic];
			
			
			demuxer->goTo(0);
			demuxer->read(tmp,firstPic);
			_startSeqLen=0;
			_startSeq=NULL;
			// lookup up gop start
			while(count<firstPic)
			{
				scancode<<=8;
				scancode+=tmp[count];
				count++;
				if(scancode==0x000001b8 || scancode==0x00000100)
				{
					found=1;
					break;
				}							
			}
			if(found && count>4)
			{
				
				_startSeqLen=count-4;
				_startSeq=new uint8_t[_startSeqLen];
				memcpy(_startSeq,tmp,_startSeqLen);
				printf("seqLen : %u seq %x %x %x %x\n",
					_startSeqLen, _startSeq[0],
							_startSeq[1],
							_startSeq[2],
							_startSeq[3]);      					
			}
			else
			{
				printf("Mmm cound not find a gop start.....\n");
			}
			delete [] tmp;
		 	
			demuxer->goTo(0);
      			
			

      			// switch DTS->PTS
      			if(!renumber(0))
			{
				GUI_Alert("Mpeg renumbering error !");
				return 0;
			}
			// Feed start seq
			
      			demuxer->goTo(0);

			if(audio) printf("Audio bytes : %u\n",audio);                        								
		 _isaudiopresent=0; 
		 _isvideopresent=1; 
	 	 _videostream.dwScale=1000;
     _videostream.dwRate=fps;
    
      _mainaviheader.dwMicroSecPerFrame=(uint32_t)floor(0);;     
      _videostream.fccType=fourCC::get((uint8_t *)"vids");
      _video_bih.biBitCount=24;
      _videostream.fccHandler=fourCC::get((uint8_t *)"MPEG");
      _video_bih.biCompression=fourCC::get((uint8_t *)"MPEG");;
      
      _videostream.dwInitialFrames= 0;
      _videostream.dwStart= 0;
      _video_bih.biWidth=_mainaviheader.dwWidth=w ;
      _video_bih.biHeight=_mainaviheader.dwHeight=h;
     
     fclose(file);
     // audio ?

     if(audio)
     {
			 	printf(" Creating audio stream..\n");
				 _audioStream=new AVDMMpeg2decAudioStream(name,_nbIFrame);
			  	 _isaudiopresent=1;
		}
		else
		{
				_audioStream=NULL;
		}
     printf("Mpeg index file successfully read\n");         

     return 1; 
}
//
//	Create GOP renumbering and PTS index entry
//
uint8_t  mpeg2decHeader::renumber(uint32_t nob)
{
            UNUSED_ARG(nob);

			 MFrame *tmp=new  MFrame[_nbFrames+2];;
			//__________________________________________
			// the direct index is in DTS time (i.e. decoder time)
			// we will now do the PTS index, so that frame numbering is done
			// according to the frame # as they are seen by editor / user			
			// I1 P0 B0 B1 P1 B2 B3 I2 B7 B8
			// xx I1 B0 B1 P0 B2 B3 P1 B7 B8               
			//__________________________________________
			uint32_t forward=0;			
			uint32_t curPTS=0;
			uint32_t dropped=0;
			for(uint32_t c=1;c<_nbFrames;c++)
			{
				switch(_indexMpegPTS[c].type)
					{
						case 'P':
						case 'I':
								memcpy(&tmp[curPTS],
										&_indexMpegPTS[forward],
										sizeof(MFrame));
								forward=c;
								curPTS++;
								dropped++;
								break;
						case 'B' : // we copy as is
								if(dropped>=1)
								{
								memcpy(&tmp[curPTS],
										&_indexMpegPTS[c],
										sizeof(MFrame));
								curPTS++;
								}
#if 1								
								else
								{ 
								  printf("Frame dropped\n");
								}
#endif								
								break;
						default:
								printf("Frame : %u / %u , type %d\n",
										c,_nbFrames,_indexMpegPTS[c].type);
				
								assert(0);
						}
			}
			// put back last frame we had in store
			memcpy(&tmp[curPTS],
					&_indexMpegPTS[forward],
					sizeof(MFrame));


			_nbFrames=_videostream.dwLength= _mainaviheader.dwTotalFrames=curPTS+1;;
			delete [] _indexMpegPTS;
			_indexMpegPTS=tmp;
			return 1;

}

//-------------------------

//------------------------
uint8_t			mpeg2decHeader::close(void)
{
		printf("\n closing mpeg2dec\n");

#define FRE(x) { if(x) {delete [] x;x=NULL;}}
			FRE(_startSeq);
			FRE(_indexMpegPTS);

#undef FRE
#define FRE(x) { if(x) {delete  x;x=NULL;}}
			FRE(demuxer);

#undef FRE
    // audio stream wille be cleaned-up by itself
    return 0;
}
//-----------------------------------
//-----------------------------------

WAVHeader *mpeg2decHeader::getAudioInfo(void )
{
	if(!_audioStream)
		 return NULL;
	return _audioStream->getInfo();
}
//-----------------------------------
//-----------------------------------

uint8_t			mpeg2decHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
		*audio=_audioStream;
		if(!_audioStream) return 0;
		return 1;


}
uint8_t  mpeg2decHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
		*len=_startSeqLen;
		*data=_startSeq;
		return 1;
}
uint8_t mpeg2decHeader::getFrameSize(uint32_t frame,uint32_t *size) {

	if(frame>=_nbFrames ) return 0;
	*size=_indexMpegPTS[frame].size;
	return 1;

}
// EOF
