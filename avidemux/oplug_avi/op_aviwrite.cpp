/***************************************************************************
                          oplug_avi.cpp  -  description
                             -------------------

		This set of function is here to provide a simple api to the gui
		It will facilitate the use of other function such as audio processing
		etc...

    begin                : Mon Feb 11 2002
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
#include <strings.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "ADM_audio/aviaudio.hxx"
#include "fourcc.h"
#include "riffparser.h"
#include "subchunk.h"
#include "avilist.h"
#include "op_aviwrite.hxx"
#include "ADM_toolkit/toolkit.hxx"
//------------
typedef struct
{
  uint32_t fcc, flags, offset, len;
}
IdxEntry;

//
// We put them

IdxEntry *myindex = NULL;


aviWrite::aviWrite( void )
{
	_out=NULL;
	LAll=NULL;
	LMovie=NULL;
	LMain=NULL;
		
}
uint8_t aviWrite::sync( void )
{
	ADM_assert(_out);
	fflush(_out);
	return 1;

}
//
// Overwrite some headers with their final value
//
//
uint8_t aviWrite::updateHeader (MainAVIHeader * mainheader,
			AVIStreamHeader * videostream,
			AVIStreamHeader * astream)
{
  UNUSED_ARG(astream);

  fseek (_out, 32, SEEK_SET);
// Update main header
#ifdef ADM_BIG_ENDIAN
	MainAVIHeader ma;
	memcpy(&ma,mainheader,sizeof(MainAVIHeader));
	Endian_AviMainHeader(&ma);		
  	fwrite (&ma, sizeof (ma), 1, _out);
#else
  	fwrite (mainheader, sizeof (MainAVIHeader), 1, _out);
#endif
// now update video stream header
  fseek (_out, 0x6c, SEEK_SET);
#ifdef ADM_BIG_ENDIAN

	AVIStreamHeader as;
	memcpy(&as,videostream,sizeof(as));
	Endian_AviStreamHeader(&as);		
  	fwrite (&as, sizeof (as), 1, _out);
#else
  fwrite (videostream, sizeof (AVIStreamHeader), 1, _out);
#endif
  // should do audio too, but i's relatively harmless...
  // Yes, indeed it helps for VBR audio :)

  return 1;
}

//________________________________________________
//   Beginning of the write process
//   We fill-in the headers
//	1- Create list and write main header
//_______________________________________________
uint8_t aviWrite::writeMainHeader( void )
{

  ADM_assert (_out);
  ADM_assert (LAll == NULL);

  fseek (_out, 0, SEEK_SET);	// rewind

  LAll = new AviList ("RIFF", _out);
  LAll->Begin ("AVI ");
  // Header chunk
  LMain = new AviList ("LIST", _out);
  LMain->Begin ("hdrl");
  LMain->Write32 ("avih");
  LMain->Write32 (sizeof (MainAVIHeader));
#ifdef ADM_BIG_ENDIAN
	MainAVIHeader ma;
	memcpy(&ma,&_mainheader,sizeof(ma));
	Endian_AviMainHeader(&ma);		
	LMain->Write((uint8_t *)&ma,sizeof(ma));	
#else
  	LMain->Write ((uint8_t *) &_mainheader, sizeof (MainAVIHeader));
#endif
	return 1;
}
//________________________________________________
//   Beginning of the write process
//   We fill-in the headers
//	2- Write video headers
//_______________________________________________
uint8_t aviWrite::writeVideoHeader( uint8_t *extra, uint32_t extraLen )
{

  ADM_assert (_out);

      _videostream.fccType = fourCC::get ((uint8_t *) "vids");
      _bih.biSize=sizeof(_bih)+extraLen;
#ifdef ADM_BIG_ENDIAN
	// in case of Little endian, do the usual swap crap
	
	AVIStreamHeader as;
	BITMAPINFOHEADER b;
	memcpy(&as,&_videostream,sizeof(as));
	Endian_AviStreamHeader(&as);		
	memcpy(&b,&_bih,sizeof(_bih));
	Endian_BitMapInfo( &b );
  	setStreamInfo (_out, (uint8_t *) &as,
		  (uint8_t *)&b,sizeof(BITMAPINFOHEADER),
		  extra,extraLen, 	 
		 0x1000);
#else
  	setStreamInfo (_out, (uint8_t *) &_videostream,
		  (uint8_t *)&_bih,sizeof(BITMAPINFOHEADER),
		  extra,extraLen, 	 
		 0x1000);

#endif
	return 1;
}
typedef struct VBRext
    {
  uint16_t   	    cbsize ;
  uint16_t          wId ;
  uint32_t          fdwflags ;
  uint16_t          nblocksize ;
  uint16_t          nframesperblock  ;
  uint16_t          ncodecdelay ;
} VBRext;


//________________________________________________
//   Beginning of the write process
//   We fill-in the headers
//	3- Write audio headers 
//   That one can be used several times so we pass stuff
//   as parameter
//_______________________________________________
static 	uint32_t aacBitrate[16]=
{
	96000, 88200, 64000, 48000,
	44100, 32000, 24000, 22050,
	16000, 12000, 11025,  8000,
	0,     0,     0,     0 
};

uint8_t aviWrite::writeAudioHeader (	AVDMGenericAudioStream * stream, AVIStreamHeader *header )
{
WAVHeader *wav=NULL;
// pre compute some headers with extra data in...
uint8_t wmaheader[12];
VBRext  mp3vbr;
uint8_t aacHeader[12];
uint8_t *extra=NULL;
uint32_t extraLen=0;
     
	if(!stream) return 1;

	memset(wmaheader,0,12);
	memset(&mp3vbr,0,sizeof(mp3vbr));

	wmaheader[16-16]=0x0a;
	wmaheader[19-16]=0x08;
	wmaheader[22-16]=0x01;
	wmaheader[24-16]=0x74;
	wmaheader[25-16]=01;

        wav = stream->getInfo ();
      ADM_assert (wav);

      memset (header, 0, sizeof (AVIStreamHeader));
      header->fccType = fourCC::get ((uint8_t *) "auds");
      header->dwInitialFrames = 0;
      header->dwStart = 0;
      header->dwRate = wav->byterate;
      header->dwSampleSize = 1;
      header->dwQuality = 0xffffffff;
      header->dwSuggestedBufferSize = 8000;
      header->dwLength = stream->getLength ();
      
	switch(wav->encoding)
	{
		case WAV_AAC:
		{
		// nb sample in stream  
		
			double len;
			len=_videostream.dwLength;
#if 0			
			len/=_videostream.dwRate;
			len*=_videostream.dwScale;			
			len*=wav->frequency;
			len/=1024;
#endif			
			header->dwLength= floor(len);//_videostream.dwLength; 
		 // AAC is mostly VBR
		 header->dwFlags=1;
		 header->dwInitialFrames=0;
		 header->dwRate=wav->frequency;
		 
		 	
		 
		 header->dwScale=1024; //sample/packet 1024 seems good for aac
		 header->dwSampleSize = 0;
		 header->dwSuggestedBufferSize=8192;
		 header->dwInitialFrames = 0;	 
		
		// header->dwLength= _videostream.dwLength; 
		 wav->blockalign=1024;	  
		 wav->bitspersample = 0; 
		 
		//*b++ = (BYTE)((profile +1) << 3 | (SRI >> 1));
		//*b++ = (BYTE)(((SRI & 0x1) << 7) | (aacsource->GetChannelCount() << 3));
		
		int SRI=4;	// Default 44.1 khz
		for(int i=0;i<16;i++) if(wav->frequency==aacBitrate[i]) SRI=i;
		aacHeader[0]=0x2;
		aacHeader[1]=0x0;
		aacHeader[2]=(2<<3)+(SRI>>1); // Profile LOW
		aacHeader[3]=((SRI&1)<<7)+((wav->channels)<<3);
		

		extra=&(aacHeader[0]);
		extraLen=4;
		}
		break;
	case WAV_MP3:							
		  // then update VBR fields
		  mp3vbr.cbsize = R16(12);
		  mp3vbr.wId = R16(1);
		  mp3vbr.fdwflags = R32(2);				  
	    	  mp3vbr.nframesperblock = R16(1);
		  mp3vbr.ncodecdelay = 0;
		  
		  wav->bitspersample = 0;
		  mp3vbr.nblocksize=R16(0x180); //383; // ??
    
		  header->dwScale = 1;
	  	  header->dwInitialFrames = 1;
    
		  if (wav->blockalign ==1152)	// VBR audio
			{			// We do like nandub do
		  	//ADM_assert (audiostream->asTimeTrack ());
		  	wav->blockalign = 1152;	// just a try
		     	wav->bitspersample = 16;
		  
		    	header->dwRate 	= wav->frequency;	//wav->byterate;
			header->dwScale = wav->blockalign;
			header->dwLength= _videostream.dwLength;
		   
  			header->dwSampleSize = 0;
		  	printf ("\n VBR audio detected\n");
		  	//
		  	// use extended headers
		  	//		
		  	//
		  	double	    nb;
		  	nb = stream->getLength ();
		  	nb/= _videostream.dwLength;
		  	// avg value of a block (i.e. a mpeg frame)
		  	mp3vbr.nblocksize = (uint32_t) floor (nb);			
		  	mp3vbr.nblocksize = R16(mp3vbr.nblocksize); 
		   }	
		   else wav->blockalign=1;

			extra=(uint8_t *)&mp3vbr;
			extraLen=sizeof(mp3vbr);

			  break;
	
					 
	case WAV_WMA:
			header->dwScale 	= wav->blockalign;
			header->dwSampleSize 	= wav->blockalign;
			header->dwInitialFrames =1;				
			header->dwSuggestedBufferSize=10*wav->blockalign;				
			
			extra=(uint8_t *)&wmaheader;
			extraLen=12;
			break;
	default:
			header->dwScale = 1;  
			wav->blockalign=1;	
			break;
    }
#ifdef ADM_BIG_ENDIAN
	// in case of Little endian, do the usual swap crap
	
	AVIStreamHeader as;
	WAVHeader w;
	memcpy(&as,header,sizeof(as));
	Endian_AviStreamHeader(&as);		
	memcpy(&w,wav,sizeof(w));
	Endian_WavHeader( &w );
  	setStreamInfo (_out, 
		(uint8_t *) &as,
		  (uint8_t *)&w,sizeof(WAVHeader),
		  extra,extraLen, 	 
		 0x1000);
#else
	setStreamInfo (_out,
			(uint8_t *) header,
	 		(uint8_t *) wav, sizeof (WAVHeader),
			extra,extraLen, 0x1000);
#endif

  return 1;
}

//_______________________________________________________
//
//   Begin to save, built header and prepare structure
//   The nb frames is indicative but the real value
//   must be smaller than this parameter
//
//_______________________________________________________
uint8_t aviWrite::saveBegin (char 	*name,
		     MainAVIHeader 	*inmainheader, 
		     uint32_t 		nb_frame,
		     AVIStreamHeader * invideostream,
		     BITMAPINFOHEADER	*bih,
		     uint8_t 		*videoextra,
		     uint32_t  		videoextraLen,
		     AVDMGenericAudioStream * inaudiostream,
		     AVDMGenericAudioStream * inaudiostream2)
{

	asize=asize2=0;

//  Sanity Check
  	ADM_assert (_out == NULL);
  	if (!(_out = fopen (name, "wb")))
  {
		printf("Problem writing : %s\n",name);
    return 0;
    }
  curindex = 0;
  vframe = asize = 0;
  nb_audio=0;
// update avi header according to the information WE want
//
  	memcpy (&_mainheader, inmainheader, sizeof (MainAVIHeader));
  	_mainheader.dwFlags = AVIF_HASINDEX + AVIF_ISINTERLEAVED;
	if(!_mainheader.dwMicroSecPerFrame)
	{
		_mainheader.dwMicroSecPerFrame=40000;
	}
  // update main header codec with video codev
  if (inaudiostream)
  {
    		_mainheader.dwStreams = 2;
		nb_audio=1;
  }
  else
    		_mainheader.dwStreams = 1;

	if(inaudiostream2)
	{
	 	printf("\n +++Dual audio stream...\n");
     		_mainheader.dwStreams ++;
		nb_audio++;
	}

  	_mainheader.dwTotalFrames = nb_frame;
//  Idem for video stream
//
  	memcpy (&_videostream, invideostream, sizeof (AVIStreamHeader));
  	_videostream.dwLength = nb_frame;
	_videostream.fccType=fourCC::get((uint8_t *)"vids");
	memcpy(&_bih,bih,sizeof(_bih));
  //___________________
  // Prepare header
  //___________________

	writeMainHeader( );

	writeVideoHeader(videoextra,videoextraLen );

	writeAudioHeader (	inaudiostream , &_audio1 );
	writeAudioHeader (	inaudiostream2, &_audio2); 

	LMain->End();
	delete LMain;
	LMain=NULL;
  //

  ADM_assert (!LMovie);
  	LMovie = new AviList ("LIST", _out);
  LMovie->Begin ("movi");
  curindex = 0;
  // the *2 is for audio and video
  // the *3 if for security sake
  myindex = (IdxEntry *) malloc (sizeof (IdxEntry) * (nb_frame * 4));
  asize = 0;
  vframe = 0;
  return 1;
}

//_______________________________________________________
// Write video frames and update index accordingly
//_______________________________________________________
uint8_t aviWrite::saveVideoFrame (uint32_t len, uint32_t flags, uint8_t * data)
{
  vframe++;
  return saveFrame (len, flags, data, (uint8_t *) "00dc");

}

uint8_t aviWrite::saveAudioFrame (uint32_t len, uint8_t * data)
{
  asize += len;
  return saveFrame (len, (uint32_t) 0, data, (uint8_t *) "01wb");
}
uint8_t aviWrite::saveAudioFrameDual (uint32_t len, uint8_t * data)
{
  asize2 += len;
  return saveFrame (len, (uint32_t) 0, data, (uint8_t *) "02wb");
}



uint8_t aviWrite::saveFrame (uint32_t len, uint32_t flags,
		     uint8_t * data, uint8_t * fcc)
{
  uint32_t
    offset;
  // offset of this chunk compared to the beginning
  offset = LMovie->Tell () - 8 - LMovie->TellBegin ();
  LMovie->WriteChunk (fcc, len, data);
  // Now store the index part

  myindex[curindex].fcc = fourCC::get (fcc);
  myindex[curindex].len = len;
  myindex[curindex].flags = flags;
  myindex[curindex].offset = offset;
  curindex++;
  return 1;
}

//_______________________________________________________
// End movie
//_______________________________________________________
uint8_t aviWrite::setEnd (void)
{

  // First close the movie
  LMovie->End ();
  delete LMovie;
  LMovie = NULL;



  printf ("\n writing %lu index parts", curindex);
  printf ("\n received %lu video parts", vframe);

// Updating compared to what has been really written
//

  // Write index  
  LAll->Write32 ("idx1");
  LAll->Write32 (curindex * 16);

  for (uint32_t i = 0; i < curindex; i++)
    {
      LAll->Write32 (myindex[i].fcc);
      LAll->Write32 (myindex[i].flags);
      LAll->Write32 (myindex[i].offset);	// abs position
      LAll->Write32 (myindex[i].len);
    }
  // Close movie


  LAll->End ();
  delete
    LAll;
  LAll = NULL;
 printf ("\n Updating headers...\n");
  _mainheader.dwTotalFrames = vframe;
  _videostream.dwLength = vframe;
  //astream.dwLength = asize;

// Update Header
  updateHeader (&_mainheader, &_videostream, NULL);


	printf("\n End of movie, \n video frames : %lu\n audio frames : %lu",vframe,asize);
  // need to update headers now
  // AUDIO SIZE ->TODO

  fclose (_out);
  _out = NULL;
  return 1;

}

//
//
//
uint8_t aviWrite::setStreamInfo (FILE * fo,
			 uint8_t * stream,
			 uint8_t * info, uint32_t infolen, 
			 uint8_t * extra, uint32_t extraLen,
			 uint32_t maxxed)
{


  AviList * alist;
  uint8_t * junk;
  uint32_t junklen;

  alist = new AviList ("LIST", fo);


  // 12 LIST
  // 8 strf subchunk
  // 8 strl subchunk
  // 8 defaultoffset
  alist->Begin ("strl");

  // sub chunk 1
  alist->WriteChunk ((uint8_t *) "strh", sizeof (AVIStreamHeader),
		     (uint8_t *) stream);

  uint8_t *buf=new uint8_t[infolen+extraLen];

	memcpy(buf,info,infolen);
	if(extraLen)
		memcpy(infolen+buf,extra,extraLen);

  alist->WriteChunk ((uint8_t *) "strf", infolen+extraLen, buf);

  junklen = maxxed - sizeof (AVIStreamHeader) - infolen-extraLen;
  junk = (uint8_t *) malloc (junklen);
  ADM_assert (junk);
  memset (junk,0, junklen);
  //
  // Fill junk with out info string  
  uint32_t len=strlen("Avidemux");
  
  if(junklen>len)
  	memcpy(junk,"Avidemux",len);	
  
  alist->WriteChunk ((uint8_t *) "JUNK", junklen, junk);
  free (junk);

  alist->End ();
  delete alist;
  delete buf;
  return 1;


}
// return how much has been written
uint32_t	aviWrite::getPos( void )
{
uint32_t pos;
	 // we take size of file + index
	 // with 32 bytes per index entry
	 // 
	 ADM_assert(_out);
	 pos=ftello(_out);
	 return pos+curindex*4*4;	 	
}
// EOF
