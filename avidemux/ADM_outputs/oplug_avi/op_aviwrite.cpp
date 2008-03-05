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

 /*
 * MODIFIED Feb 2005 by GMV: ODML write support
 */
 
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
#include "ADM_assert.h"
#include <math.h>

#include "config.h"
#include <pthread.h>

#include "avifmt.h"
#include "avifmt2.h"
#include "ADM_audio/aviaudio.hxx"
#include "fourcc.h"
#include "subchunk.h"
#include "avilist.h"
#include "op_aviwrite.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_osSupport/ADM_quota.h"
#include "ADM_fileio.h"

// MOD Feb 2005 by GMV
#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_SAVE_AVI
#include "ADM_osSupport/ADM_debug.h"
#include "prefs.h"
// END MOD Feb 2005 by GMV
uint32_t ADM_UsecFromFps1000(uint32_t fps1000);
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
        _file=NULL;
		
	// MOD Feb 2005 by GMV: ODML support
	odml_indexes=NULL;
	// END MOD Feb 2005 by GMV
}

// MOD Feb 2005 by GMV: remove ODML index
aviWrite::~aviWrite(){
	if (myindex)
		delete myindex;

	if (LAll)
		delete LAll;

	if (LMovie)
		delete LMovie;

	if (LMain)
		delete LMain;

	myindex = NULL;
	LAll = NULL;
	LMovie = NULL;
	LMain = NULL;

	odml_destroy_index();
}
// END MOD Feb 2005 by GMV

uint8_t aviWrite::sync( void )
{
	ADM_assert(_file);
	_file->flush();
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

        ADM_assert(_file);
        
        _file->seek(32);
// Update main header
#ifdef ADM_BIG_ENDIAN
	MainAVIHeader ma;
	memcpy(&ma,mainheader,sizeof(MainAVIHeader));
	Endian_AviMainHeader(&ma);		
  	_file->write ((uint8_t *)&ma, sizeof (ma));
#else
  	_file->write ((uint8_t *)mainheader, sizeof (MainAVIHeader));
#endif
// now update video stream header
        _file->seek(0x6c);
#ifdef ADM_BIG_ENDIAN

	AVIStreamHeader as;
	memcpy(&as,videostream,sizeof(as));
	Endian_AviStreamHeader(&as);		
  	_file->write ((uint8_t *)&as, sizeof (as));
#else
        _file->write ((uint8_t *)videostream, sizeof (AVIStreamHeader));
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

  ADM_assert (_file);
  ADM_assert (LAll == NULL);
  _file->seek(0);
  

  LAll = new AviList ("RIFF", _file);
  LAll->Begin ("AVI ");
  // Header chunk
  LMain = new AviList ("LIST", _file);
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

  ADM_assert (_file);

      _videostream.fccType = fourCC::get ((uint8_t *) "vids");
      _bih.biSize=sizeof(_bih)+extraLen;
	// MOD Feb 2005 by GMV: video super index length
	uint32_t odml_video_super_idx_size;
        if(doODML!=NORMAL)
        {
            
            odml_video_super_idx_size=24+odml_default_nbrof_index*16;
        }else
        {
            odml_video_super_idx_size=24+odml_indexes[0].odml_nbrof_index*16;
        }
	// END MOD Feb 2005 by GMV
#ifdef ADM_BIG_ENDIAN
	// in case of Little endian, do the usual swap crap
	
	AVIStreamHeader as;
	BITMAPINFOHEADER b;
	memcpy(&as,&_videostream,sizeof(as));
	Endian_AviStreamHeader(&as);		
	memcpy(&b,&_bih,sizeof(_bih));
	Endian_BitMapInfo( &b );
  	setStreamInfo (_file, (uint8_t *) &as,
		  (uint8_t *)&b,sizeof(BITMAPINFOHEADER),
		// MOD Feb 2005 by GMV: ODML support
		odml_video_super_idx_size,0,
		// END MOD Feb 2005 by GMV
		  extra,extraLen, 	 
		 0x1000);
#else
  	setStreamInfo (_file, (uint8_t *) &_videostream,
		  (uint8_t *)&_bih,sizeof(BITMAPINFOHEADER),
		// MOD Feb 2005 by GMV: ODML support
		odml_video_super_idx_size,0,
		// END MOD Feb 2005 by GMV
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
/**
        \fn WriteAudioHeader

*/
uint8_t aviWrite::writeAudioHeader (	AVDMGenericAudioStream * stream, AVIStreamHeader *header
// MOD Feb 2005 by GMV: ODML support
,uint8_t	odml_stream_nbr
// END MOD Feb 2005 by GMV
)
{
// MOD Feb 2005 by GMV: audio super index length
uint32_t odml_audio_super_idx_size;;
// END MOD Feb 2005 by GMV
WAVHeader wav;
// pre compute some headers with extra data in...
uint8_t wmaheader[12];
VBRext  mp3vbr;
uint8_t aacHeader[12];
uint8_t *extra=NULL;
uint32_t extraLen=0;
     
	if(!stream) return 1;
        if(doODML!=NORMAL)
        {
            odml_audio_super_idx_size=24+odml_default_nbrof_index*16;
        }else
        {
            odml_audio_super_idx_size=24+odml_indexes[odml_stream_nbr].odml_nbrof_index*16;
        }
	memset(wmaheader,0,12);
	memset(&mp3vbr,0,sizeof(mp3vbr));

	wmaheader[16-16]=0x0a;
	wmaheader[19-16]=0x08;
	wmaheader[22-16]=0x01;
	wmaheader[24-16]=0x74;
	wmaheader[25-16]=01;

        memcpy(&wav,stream->getInfo (),sizeof(wav));
      

      memset (header, 0, sizeof (AVIStreamHeader));
      header->fccType = fourCC::get ((uint8_t *) "auds");
      header->dwInitialFrames = 0;
      header->dwStart = 0;
      header->dwRate = wav.byterate;
      header->dwSampleSize = 1;
      header->dwQuality = 0xffffffff;
      header->dwSuggestedBufferSize = 8000;
      header->dwLength = stream->getLength ();
      
	switch(wav.encoding)
	{
                case WAV_IMAADPCM:
                        wav.blockalign=1024;
                        header->dwScale         = wav.blockalign;
                        header->dwSampleSize    = 1;
                        header->dwInitialFrames =1;                             
                        header->dwSuggestedBufferSize=2048;                                
                        break;
		case WAV_AAC:
		{
		// nb sample in stream  
		
			double len;
			len=_videostream.dwLength;
#if 1			
			len/=_videostream.dwRate;
			len*=_videostream.dwScale;			
			len*=wav.frequency;
			len/=1024;
#else		
			header->dwLength= floor(len);//_videostream.dwLength; 
#endif			
		 // AAC is mostly VBR
		 header->dwFlags=1;
		 header->dwInitialFrames=0;
		 header->dwRate=wav.frequency;
		 
		 	
		 
		 header->dwScale=1024; //sample/packet 1024 seems good for aac
		 header->dwSampleSize = 0;
		 header->dwSuggestedBufferSize=8192;
		 header->dwInitialFrames = 0;	 
		
		// header->dwLength= _videostream.dwLength; 
		 wav.blockalign=1024;	  
		 wav.bitspersample = 0; 
		 
		//*b++ = (BYTE)((profile +1) << 3 | (SRI >> 1));
		//*b++ = (BYTE)(((SRI & 0x1) << 7) | (aacsource->GetChannelCount() << 3));
		
		int SRI=4;	// Default 44.1 khz
		for(int i=0;i<16;i++) if(wav.frequency==aacBitrate[i]) SRI=i;
		aacHeader[0]=0x2;
		aacHeader[1]=0x0;
		aacHeader[2]=(2<<3)+(SRI>>1); // Profile LOW
		aacHeader[3]=((SRI&1)<<7)+((wav.channels)<<3);
		

		extra=&(aacHeader[0]);
		extraLen=4;
		}
		break;
#if 1		
	case WAV_DTS:
	case WAV_AC3: // Vista compatibility
		 		  extra=(uint8_t *)wmaheader;
		 		  extra[0]=0;
		 		  extra[1]=0;
				  extraLen=2;
				  header->dwScale = 1;  
				  wav.blockalign=1;	
			break;
#endif			
	case WAV_MP3:							
		  // then update VBR fields
		  mp3vbr.cbsize = R16(12);
		  mp3vbr.wId = R16(1);
		  mp3vbr.fdwflags = R32(2);				  
	    	  mp3vbr.nframesperblock = R16(1);
		  mp3vbr.ncodecdelay = 0;
		  
		  wav.bitspersample = 0;
		  mp3vbr.nblocksize=R16(0x180); //384; // ??
    
		  header->dwScale = 1;
	  	  header->dwInitialFrames = 1;
                  extra=(uint8_t *)&mp3vbr;
		  extraLen=sizeof(mp3vbr);
		  if (stream->isVBR()) //wav->blockalign ==1152)	// VBR audio
			{			// We do like nandub do
		  	//ADM_assert (audiostream->asTimeTrack ());
		  	wav.blockalign = 1152;	// just a try
		     	wav.bitspersample = 16;
		  
		    	header->dwRate 	= wav.frequency;	//wav->byterate;
			header->dwScale = wav.blockalign;
			header->dwLength= _videostream.dwLength;
		   
  			header->dwSampleSize = 0;
		  	printf ("\n VBR audio detected\n");
		  	//
		  	// use extended headers
		  	//		
		  	//
			mp3vbr.nblocksize=1152;	
			
		   }	
		   else 
                   {
                     wav.blockalign=1;
                     
                     
                   }

			

			  break;
	
					 
	case WAV_WMA:
			header->dwScale 	= wav.blockalign;
			header->dwSampleSize 	= wav.blockalign;
			header->dwInitialFrames =1;				
			header->dwSuggestedBufferSize=10*wav.blockalign;				
			
			extra=(uint8_t *)&wmaheader;
			extraLen=12;
			break;
          case WAV_8BITS_UNSIGNED:
                        wav.encoding=WAV_PCM;
			header->dwScale 	= 1;
                        wav.bitspersample=8;
                        break;
                        
                        
	default:
			header->dwScale = 1;  
			wav.blockalign=1;	
			break;
    }
#ifdef ADM_BIG_ENDIAN
	// in case of Little endian, do the usual swap crap
	
	AVIStreamHeader as;
	WAVHeader w;
	memcpy(&as,header,sizeof(as));
	Endian_AviStreamHeader(&as);		
	memcpy(&w,&wav,sizeof(w));
	Endian_WavHeader( &w );
  	setStreamInfo (_file, 
		(uint8_t *) &as,
		  (uint8_t *)&w,sizeof(WAVHeader),
		// MOD Feb 2005 by GMV: ODML support
		odml_audio_super_idx_size,odml_stream_nbr,
		// END MOD Feb 2005 by GMV
		  extra,extraLen, 	 
		 0x1000);
#else
	setStreamInfo (_file,
			(uint8_t *) header,
	 		(uint8_t *) &wav, sizeof (WAVHeader),
			// MOD Feb 2005 by GMV: ODML support
			odml_audio_super_idx_size,odml_stream_nbr,
			// END MOD Feb 2005 by GMV
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
        if (!(_out = qfopen (name, "wb")))
        {
                printf("Problem writing : %s\n",name);
                return 0;
        }
        _file=new ADMFile();
        if(!_file->open(_out))
        {
                printf("Cannot create ADMfileio\n");
                delete _file;
                _file=NULL;
                return 0;
        }
        curindex = 0;
        vframe = asize = 0;
        nb_audio=0;
        
// update avi header according to the information WE want
//
        memcpy (&_mainheader, inmainheader, sizeof (MainAVIHeader));
        _mainheader.dwFlags = AVIF_HASINDEX + AVIF_ISINTERLEAVED;


        
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

// Update usecperframe
double f;
        f=_videostream.dwRate;
        f*=1000;
        f/=_videostream.dwScale;
        _mainheader.dwMicroSecPerFrame=ADM_UsecFromFps1000( (uint32_t)floor(f));
        

        // Recompute image size
uint32_t is;	
        is=_bih.biWidth*_bih.biHeight;
        is*=(_bih.biBitCount+7)/8;
        _bih.biSizeImage=is;



	// MOD Feb 2005 by GMV: initialize ODML data
	// test for free data structures
	if(odml_indexes!=NULL){
		aprintf("\n ODML writer error: data structures not empty for init!");
		return 0;
	}
	// set generation mode
        uint32_t pref_odml=0;
        
        if(!prefs->get(FEATURE_USE_ODML, &pref_odml))
        {
          pref_odml=0;                
        }            
        
	doODML=NO;	// only option for users without largefile support
	#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
        if(pref_odml)        
	       doODML=HIDDEN;	// default; TODO: user should be able to choose NO for plain avi
	#endif
	if(doODML!=NO){
		// get number of streams
		odml_nbrof_streams=_mainheader.dwStreams;
		aprintf("\nnumber of streams: %lu\n",odml_nbrof_streams);
		// get number of frames per index

		odml_index_size=(long)ceil(1000000.0/(double)_mainheader.dwMicroSecPerFrame*600.0);	// one index per 10 Minutes; decrease if 4GB are not enough for this amount of time 
                aprintf("\n old number of frames per index: %lu\n",odml_index_size);
                double fps=invideostream->dwRate/invideostream->dwScale;      

                        aprintf("Fps1000:%f\n",fps);
                        fps=600*fps; // 10 mn worth;
                odml_index_size=(int)floor(fps);

		aprintf("\nnumber of frames per index: %lu\n",odml_index_size);
		// get number or indexes per stream
		odml_default_nbrof_index=(long)ceil((double)nb_frame/(double)odml_index_size);
		aprintf("\nnumber of indexes per stream: %lu\n",odml_default_nbrof_index);
		// init some other values
		odml_header_fpos=0;
		odml_riff_fpos[0]=0;odml_riff_fpos[1]=0;odml_riff_fpos[2]=0;odml_riff_fpos[3]=0;
		odml_riff_count=0;
		odml_frames_inAVI=0;
		// create odml index data structure
		odml_indexes=(odml_super_index_t*) ADM_alloc (sizeof(odml_super_index_t) * odml_nbrof_streams); // super index list
		memset(odml_indexes,0,sizeof(odml_super_index_t) * odml_nbrof_streams);
		for(int a=0;a<odml_nbrof_streams;++a)
                {	// for each stream -> one super index
                        odml_indexes[a].odml_nbrof_index=odml_default_nbrof_index;
			odml_indexes[a].odml_index= (odml_index_t*) ADM_alloc (sizeof(odml_index_t) * odml_default_nbrof_index); // index list
			memset(odml_indexes[a].odml_index,0,sizeof(odml_index_t) * odml_default_nbrof_index);
			for(int b=0;b<odml_default_nbrof_index;++b)
                        {	// for each index
				odml_indexes[a].odml_index[b].index=(odml_index_data_t*) ADM_alloc (sizeof(odml_index_data_t) * odml_index_size);	// index data
				memset(odml_indexes[a].odml_index[b].index,0,sizeof(odml_index_data_t) * odml_index_size);
				odml_indexes[a].odml_index[b].nEntriesInUse=0;	// (redundant)
			}
			// init data
			odml_indexes[a].index_count=0;
		}
	}
            else
        {
            odml_default_nbrof_index=16;
        }
	// END MOD Feb 2005 by GMV
	
  //___________________
  // Prepare header
  //___________________

	writeMainHeader( );

	writeVideoHeader(videoextra,videoextraLen );

	// MOD Feb 2005 by GMV: ODML support
	/*writeAudioHeader (	inaudiostream , &_audio1 );
	writeAudioHeader (	inaudiostream2, &_audio2);*/ 
	writeAudioHeader (	inaudiostream , &_audio1,1);
	writeAudioHeader (	inaudiostream2, &_audio2,2); 
	// odml header placeholder
	odml_write_dummy_chunk(LMain, &odml_header_fpos, 16);
	// END MOD Feb 2005 by GMV

	LMain->End();
	delete LMain;
	LMain=NULL;
  //

  ADM_assert (!LMovie);
  
  LMovie = new AviList ("LIST", _file);
  LMovie->Begin ("movi");
  curindex = 0;
  // the *2 is for audio and video
  // the *3 if for security sake
  myindex = (IdxEntry *) ADM_alloc (sizeof (IdxEntry) * (nb_frame * 4));
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
  // MOD Feb 2005 by GMV:  interleave ODML index dummy and index frame 
	// write initial index chunks
	if(vframe==2 && doODML!=NO){	// apparently some players require a video frame at first in the movi list, so we put the initial index dummys behind it (bye bye index before data)
		odml_write_dummy_chunk(LMovie, &(odml_indexes[0].odml_index[0].fpos), 24+8*odml_index_size);
		if(odml_nbrof_streams>1)
			odml_write_dummy_chunk(LMovie, &(odml_indexes[1].odml_index[0].fpos), 24+8*odml_index_size);
		if(odml_nbrof_streams>2)
			odml_write_dummy_chunk(LMovie, &(odml_indexes[2].odml_index[0].fpos), 24+8*odml_index_size);
	}
	// test for new riff
	odml_riff_break(len+8); // data size + fcc + size info (padding is handled in odml_riff_break)
	// index frame
	if(!odml_index_frame(0, len,flags&AVI_KEY_FRAME)){
		aprintf("\ncan not index video frame %lu\n",vframe);
	}
// END MOD Feb 2005 by GMV
  return saveFrame (len, flags, data, (uint8_t *) "00dc");

}

uint8_t aviWrite::saveAudioFrame (uint32_t len, uint8_t * data)
{
  asize += len;
// MOD Feb 2005 by GMV: index frame and interleave ODML index dummy
	odml_riff_break(len+8); // data size + fcc + size info (padding is handled in odml_riff_break)
	if(!odml_index_frame(1, len,false)){
		aprintf("\ncan not index audio frame %lu\n",asize);
	}
// END MOD Feb 2005 by GMV
  return saveFrame (len, (uint32_t) 0, data, (uint8_t *) "01wb");
}
uint8_t aviWrite::saveAudioFrameDual (uint32_t len, uint8_t * data)
{
  asize2 += len;
// MOD Feb 2005 by GMV: index frame and interleave ODML index dummy
	odml_riff_break(len+8); // data size + fcc + size info (padding is handled in odml_riff_break)
	if(!odml_index_frame(2, len,false)){
		aprintf("\ncan not index audio (dual) frame %lu\n",asize);
	}
// END MOD Feb 2005 by GMV
  return saveFrame (len, (uint32_t) 0, data, (uint8_t *) "02wb");
}



uint8_t aviWrite::saveFrame (uint32_t len, uint32_t flags,
		     uint8_t * data, uint8_t * fcc)
{
  uint32_t offset;
  // offset of this chunk compared to the beginning
// MOD Feb 2005 by GMV: do not write idx1 in case of ODML
  //offset = LMovie->Tell () - 8 - LMovie->TellBegin ();
if(doODML!=NORMAL){
  offset = LMovie->Tell () - 8 - LMovie->TellBegin ();
}
// END MOD Feb 2005 by GMV
  LMovie->WriteChunk (fcc, len, data);
  // Now store the index part

// MOD Feb 2005 by GMV: do not write idx1 in case of ODML
if(doODML!=NORMAL){
// END MOD Feb 2005 by GMV
  myindex[curindex].fcc = fourCC::get (fcc);
  myindex[curindex].len = len;
  myindex[curindex].flags = flags;
  myindex[curindex].offset = offset;
  curindex++;
// MOD Feb 2005 by GMV: do not write idx1 in case of ODML
}
// END MOD Feb 2005 by GMV
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


// MOD Feb 2005 by GMV: do not write idx1 in case of ODML
    if(doODML!=NORMAL)
    {  // Regular index
            // END MOD Feb 2005 by GMV
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
            // MOD Feb 2005 by GMV: do not write idx1 in case of ODML
    }
// END MOD Feb 2005 by GMV
  // Close movie
#ifndef MOVINDEX
  LAll->End ();
  delete    LAll;
  LAll = NULL;
#endif


 printf ("\n Updating headers...\n");

// MOD Feb 2005 by GMV: ODML header and index
	if(doODML==NORMAL)
        {
		odml_write_sindex(0, "00dc");	// video super index
		if(odml_nbrof_streams>1)odml_write_sindex(1,"01wb");	// audio super index
		if(odml_nbrof_streams>2)odml_write_sindex(2,"02wb");	// audio super index (dual)
		// odml header
		_file->seek(odml_header_fpos);
		AviList* LHeader =  new AviList("LIST", _file);
		LHeader->Begin("odml");
		LHeader->Write32("dmlh");
		LHeader->Write32((uint32_t)4);	// chunk size
		LHeader->Write32(vframe);	// total number of frames
		LHeader->End();
		delete LHeader;
		// indexes
		if(!odml_write_index(0, "00dc", "ix00")){	// video indexes
			aprintf("error writing video indexes");
		}
		if(odml_nbrof_streams>1)
			if(!odml_write_index(1, "01wb", "ix01")){	// audio indexes
				aprintf("error writing audio indexes");
			}
		if(odml_nbrof_streams>2)
			if(!odml_write_index(2, "02wb", "ix02")){	// audio indexes (dual)
				printf("error writing audio (dual) indexes");
			}
	}
	odml_destroy_index();		
// END MOD Feb 2005 by GMV
#ifdef MOVINDEX
  LAll->End ();
  delete    LAll;
  LAll = NULL;
#endif

// MOD Feb 2005 by GMV: set number or frames in first riff
  //_mainheader.dwTotalFrames = vframe;
	if(doODML==NORMAL)
		_mainheader.dwTotalFrames=odml_frames_inAVI;
	else
  _mainheader.dwTotalFrames = vframe;
// END MOD Feb 2005 by GMV
	
  _videostream.dwLength = vframe;
  //astream.dwLength = asize;

// Update Header
  updateHeader (&_mainheader, &_videostream, NULL);


	printf("\n End of movie, \n video frames : %lu\n audio frames : %lu",vframe,asize);
  // need to update headers now
  // AUDIO SIZE ->TODO
  delete _file;
  _file=NULL;
  
  qfclose (_out);
  _out = NULL;
  return 1;

}

//
//
//
uint8_t aviWrite::setStreamInfo (ADMFile * fo,
			 uint8_t * stream,
			 uint8_t * info, uint32_t infolen, 
			// MOD Feb 2005 by GMV: ODML support
			 uint32_t odml_headerlen,
			 uint8_t odml_stream_nbr,
			// END MOD Feb 2005 by GMV
			 uint8_t * extra, uint32_t extraLen,
			 uint32_t maxxed)
{


  AviList * alist;
  uint8_t * junk;
  int32_t junklen;

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

    // compute junkLen, it might also hold the oldml superindex
    uint32_t consumed=odml_headerlen*4;
  junklen = (consumed+maxxed-1)/maxxed;
  junklen=junklen*maxxed;
  
  printf("[ODML] using ODML placeholder of size %u bytes\n",junklen);
  junk = (uint8_t *) ADM_alloc (junklen);
  ADM_assert (junk);
  memset (junk,0, junklen);
  //
  // Fill junk with out info string  
  uint32_t len=strlen("Avidemux");
  
  if(junklen>len)
  	memcpy(junk,"Avidemux",len);	

  if(doODML!=NO)
  {
    odml_indexes[odml_stream_nbr].fpos=_file->tell();
    odml_indexes[odml_stream_nbr].pad=junklen;
  }
  alist->WriteChunk ((uint8_t *) "JUNK", junklen, junk);
  ADM_dealloc (junk);

  // MOD Feb 2005 by GMV: ODML header
// MOVEINDEX
#ifndef MOVINDEX
  //odml_write_dummy_chunk(alist, &odml_indexes[odml_stream_nbr].fpos, odml_headerlen);
#endif
  // END MOD Feb 2005 by GMV

  alist->End ();
  delete alist;
  delete[] buf;
  return 1;


}
// return how much has been written
uint32_t	aviWrite::getPos( void )
{
uint32_t pos;
	 // we take size of file + index
	 // with 32 bytes per index entry
	 // 
	 ADM_assert(_file);
	 pos=_file->tell();
	 return pos+curindex*4*4;	 	
}

// MOD Feb 2005 by GMV:  ODML functions
/**
        \fn odml_destroy_index
*/
void aviWrite::odml_destroy_index(void){
	// destroy odml index data structure
	if(doODML!=NO){
		if(odml_indexes){
			for(int a=0;a<odml_nbrof_streams;++a){
				if(odml_indexes[a].odml_index){
					for(int b=0;b<odml_indexes[a].odml_nbrof_index;++b)
                                        {
						if(odml_indexes[a].odml_index[b].index)
							ADM_dealloc (odml_indexes[a].odml_index[b].index);
					}
					ADM_dealloc (odml_indexes[a].odml_index);
				}
			}
			ADM_dealloc (odml_indexes);
		}
		odml_indexes=NULL;
	}
}
void aviWrite::odml_write_dummy_chunk(AviList* alist, uint64_t* fpos, uint32_t size){
	if(doODML!=NO){
		// save file position
		*fpos=alist->Tell();
		aprintf("\nwrite dummy chunk at file position %Lu with data size %lu\n",*fpos, size);
		// generate dummy data
		uint8_t* dummy=(uint8_t*)ADM_alloc (size);
		memset(dummy,0,size);
		// write dummy chunk
		alist->WriteChunk ((uint8_t *) "JUNK", size, dummy);	
		// clean up
		ADM_dealloc (dummy);
	}
}
/**
        \fn reallocIndeces
*/
void aviWrite::reallocIndeces( odml_super_index_t *idx)
{
    uint32_t nw,old;
    odml_index_t   *newindex;
    odml_index_t *oldindex;
            old=idx->odml_nbrof_index;
            nw=old*2;
            printf("Increasing # of indeces from %d to %d\n",old,nw);
            oldindex=idx->odml_index;
            newindex=(odml_index_t *)ADM_alloc(sizeof(odml_index_t)*nw);
            memset(newindex,0,sizeof(odml_index_t)*nw);
            memcpy(newindex,oldindex,old*sizeof(odml_index_t));
            idx->odml_index=newindex;
            ADM_dealloc(oldindex);
            idx->odml_nbrof_index=nw;
            // Now fill in the new
            uint32_t lineSize=sizeof(odml_index_data_t) * odml_index_size;
             for(int b=old;b<nw;++b)
                        {	// for each index, alloc
                            newindex[b].index=(odml_index_data_t*) ADM_alloc (lineSize);
                            memset(newindex[b].index,0,lineSize);
                            newindex[b].nEntriesInUse=0;	// (redundant)
                        }

}


/**
        \fn odml_index_frame
*/
bool aviWrite::odml_index_frame(int stream_nbr, uint32_t data_size, bool keyFrame){
	if(doODML!=NO){
//		ADM_assert(!stream_nbr<odml_nbrof_streams);
		odml_super_index_t* sidx=odml_indexes+stream_nbr;	// access to super index
		if(sidx->odml_index[sidx->index_count].nEntriesInUse==odml_index_size)
                {	// new index needed?
			if(sidx->index_count>=sidx->odml_nbrof_index-1)	// can index counter be increased?
                                reallocIndeces(sidx);
                        ++(sidx->index_count);	// increment index counter
			
			// handle possible riff break
			odml_riff_break(data_size+8); // data size + fcc + size info (padding is handled in odml_riff_break)
			// write placeholder
			odml_write_dummy_chunk(LMovie, &(sidx->odml_index[sidx->index_count].fpos), 24+8*odml_index_size);
			sidx->odml_index[sidx->index_count].nEntriesInUse=0;
		}
		odml_index_t* idx=sidx->odml_index+(sidx->index_count);		// access to index
		odml_index_data_t* idxd=idx->index+(idx->nEntriesInUse);	// access to unused index data
		
		uint64_t pos=LMovie->Tell()+8;	// preview position of data
		idxd->fpos=pos;	// store file position of data
		
		if(keyFrame)
			idxd->size=data_size; //store data size
		else	// if no key frame
			idxd->size=data_size|0x80000000; //store data size with bit 31 set
		
		++(idx->nEntriesInUse);	// advance to next free index data entry
	}
	return true;
}
void aviWrite::odml_write_sindex(int stream_nbr, char* stream_fcc)
{
        
	// Warning: This changes the file position
	if(doODML==NORMAL)
                {
                    uint32_t startAt=odml_indexes[stream_nbr].fpos;
                    uint32_t pad=odml_indexes[stream_nbr].pad; 
                    uint32_t endAt=startAt+pad+8;
#ifndef MOVINDEX
		_file->seek(startAt);
#endif
		aprintf("\nwriting super index at file pos %Lu, total available size %u\n",odml_indexes[stream_nbr].fpos,pad);
		AviList* LIndex =  new AviList("JUNK", _file);	// abused writing aid (don't call Begin or End; the fcc is unused until 'Begin')
                uint32_t nbEntries=odml_indexes[stream_nbr].index_count+1;
		LIndex->Write32("indx");			// 4cc
		LIndex->Write32(24+nbEntries*16);	// size
		LIndex->Write16(4);				// wLongsPerEntry
		LIndex->Write8(0);				// bIndexSubType
		LIndex->Write8(0);				// bIndexType (AVI_INDEX_OF_INDEXES)
		LIndex->Write32(nbEntries);// nEntriesInUse
		LIndex->Write32(stream_fcc);			// dwChunkId;
		LIndex->Write32((uint32_t)0);
                LIndex->Write32((uint32_t)0);
                LIndex->Write32((uint32_t)0);// reserved
		for(uint32_t a=0;a<nbEntries;++a)
                {	// for each chunk index
                        uint64_t pos;
                        pos=odml_indexes[stream_nbr].odml_index[a].fpos;
                        LIndex->Write64(pos);	//absolute file position
                        LIndex->Write32(32 + 8 * odml_index_size);	// complete index chunk size
                        LIndex->Write32(odml_indexes[stream_nbr].odml_index[a].nEntriesInUse);	// duration
                        aprintf("\nstream %lu, index %lu Position: %lu  EntriesInUse:%lu\n",stream_nbr, a ,pos,
                        odml_indexes[stream_nbr].odml_index[a].nEntriesInUse);
		}
                uint32_t at=LIndex->Tell();
                
                int32_t junkLen=endAt-at-8;
                ADM_assert(junkLen>=9);
                printf("Padding ODML index with junk of size %d, total padding %lu\n",junkLen, odml_indexes[stream_nbr].pad);
		delete LIndex;
// Now create out junk chunk if needed, to padd the odml
                AviList *Junk=new AviList("JUNK",_file);
                uint8_t *zz=new uint8_t[junkLen];
                if(junkLen>9) strcpy((char *)zz,"Avidemux");
                Junk->WriteChunk ((uint8_t *)"JUNK", junkLen, zz);
                delete [] zz;
                ADM_assert(endAt==Junk->Tell());
                delete Junk;
                
                
	}	
}
bool aviWrite::odml_write_index(int stream_nbr, char* stream_fcc, char* index_fcc){	// write index
	// Warning: This changes the file position
	if(doODML==NORMAL){
		aprintf ("\n writing %lu interleaved ODML indexes for %lu frames in stream %s", odml_indexes[stream_nbr].index_count+1, vframe, stream_fcc);
		AviList* LIndex =  new AviList("JUNK", _file);	// abused writing aid (don't call Begin or End; the fcc is unused until 'Begin')
		for(int a=0;a<=odml_indexes[stream_nbr].index_count;++a){	// for each index
			odml_index_t* idx=odml_indexes[stream_nbr].odml_index+a;		// access to index
			_file->seek(idx->fpos);					// shift file pointer
			LIndex->Write32(index_fcc);			// 4cc
			LIndex->Write32(24+odml_index_size*8);		// data size
			LIndex->Write16(2);				// wLongsPerEntry
			LIndex->Write8(0);				// bIndexSubType
			LIndex->Write8(1);				// bIndexType (AVI_INDEX_OF_CHUNKS)
			LIndex->Write32(idx->nEntriesInUse);		// nEntriesInUse
			LIndex->Write32(stream_fcc);			// dwChunkId;
			uint64_t base_off=idx->index[0].fpos-8;		// lets take the position of the first frame in the index as base
			uint64_t rel_pos;
			LIndex->Write64(base_off);			// qwBaseOffset
			LIndex->Write32((uint32_t)0);			// reserved
			for(int b=0;b<idx->nEntriesInUse;++b){		// for each frame in the current index
				odml_index_data_t* idxd=idx->index+b;	// access to index data
				rel_pos=idxd->fpos-base_off;	// get relative file position
				if(rel_pos>(uint64_t)4*1024*1024*1024){	// index chunks have a maximum offset of 4GB
					printf("\nData rate too high for index size. Decrease index duration.\n"); // decrease the multiplicator in saveBegin that calculates odml_index_size
					printf("base:%Lu abs:%Lu rel:%Lu stream:%lu index:%lu entry:%lu",base_off,idxd->fpos,rel_pos,stream_nbr,a,b);
					delete LIndex;
					return false;
				}
				LIndex->Write32(rel_pos);	// relative file position
				LIndex->Write32(idxd->size);		// data size
			}
		}
		delete LIndex;
	}
	return true;
}
void aviWrite::odml_riff_break(uint32_t len){	// advance to the next riff if required
	if(doODML!=NO){
		// get padded size
		uint64_t len2=len;
		if(len & 1)++len2;
		// preview file position
		len2+=LMovie->Tell();
		// will we get over the next GB border?
		if( len2>((uint64_t)1024*1024*1024*(odml_riff_count+1)) ){
			if(doODML==HIDDEN){
				aprintf("\nstarting new (hidden) RIFF at %Lu\n",LMovie->Tell());
				if(odml_riff_count<4)	// we have only 4 buffers but this has to be enough
					odml_write_dummy_chunk(LMovie, odml_riff_fpos+odml_riff_count, 16);	// write dummy
				if(odml_riff_count==0) odml_frames_inAVI=vframe-1;	// rescue number of frames in first AVI (-1 since there may be no audio for the last video frame)
			}else{	// restart riff and movie
				aprintf("\nstarting new RIFF at %Lu\n",LMovie->Tell());
				// restart lists
				LMovie->End();
				LAll->End();
				LAll->Begin ("AVIX");
				LMovie->Begin ("movi");
			}
			++odml_riff_count;
		}
		// ODML required for movie?
		if(doODML==HIDDEN){
			if( ((uint64_t)getPos()+len+17) >= ((uint64_t)4*1024*1024*1024) ){	//if (written data + new chunk + index (old type) for new chunk + possible padding) does not fit into 4GB
				printf("\nswitching to ODML mode at %lu\n",LMovie->Tell());
				uint64_t last_pos=LMovie->Tell();	// rescue current file position
				// close First RIFF
				for(int a=0;a<4;++a){	// for each hidden riff
					if(odml_riff_fpos[a]!=0){
						_file->seek(odml_riff_fpos[a]);	// set file pointer to start of next riff
						LMovie->End();
						LAll->End();
						LAll->Begin("AVIX");
						LMovie->Begin("movi");
					}
				}
				// goto end of file
				_file->seek(last_pos);
				// following riffs can start directly
				doODML=NORMAL;	// write RIFF breaks directly
			}
		}
	}
}
// END MOD Feb 2005 by GMV

// EOF
