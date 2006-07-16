/***************************************************************************
                          op_avisave.cpp  -  description
                             -------------------
    begin                : Fri May 3 2002
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
 
 /*
* MODIFIED BY GMV 30.1.05: prepared for ODML
*/
 
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>


#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
//#include "avilist.h"

#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_assert.h" 

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "GUI_mux.h"

uint32_t muxSize=4090;
extern PARAM_MUX muxMode;
extern int muxParam;



#include "ADM_audiofilter/audioeng_buildfilters.h"

const char *getStrFromAudioCodec( uint32_t codec);
//_________________________
uint8_t ADM_aviUISetMuxer(  void )
{
	return DIA_setUserMuxParam ((int *) &muxMode, (int *) &muxParam, (int *) &muxSize);
}


//_______ set the autosplit size
uint8_t ADM_aviSetSplitSize(uint32_t size)
{
	muxSize=size;
	return 1;
}

GenericAviSave::GenericAviSave ()
{

  has_audio_track = has_audio_vbr = 0;
 vbuffer = new uint8_t[MAXIMUM_SIZE * MAXIMUM_SIZE * 3];
//  vbuffer=new ADMImage(MAXIMUM_SIZE,MAXIMUM_SIZE);
  abuffer = new uint8_t[96000];

  ADM_assert (vbuffer);
  ADM_assert (abuffer);

  audio_filter=NULL;
  audio_filter2=NULL;
  _part=0;
  dialog_work=NULL;
  _lastIPFrameSent=0xffffff;
  _incoming=NULL;
  encoding_gui=NULL;
  _videoProcess=0;
  _audioCurrent=_audioTarget=0;
 _audioTotal=0;  
 _file=NULL;
}

GenericAviSave::~GenericAviSave ()
{
  delete vbuffer;
  delete[]abuffer;
  _incoming=NULL;
  ADM_assert(!_file);
}

//___________________________________________________________
//      Generic Save Avi loop
//
//___________________________________________________________
//
uint8_t  GenericAviSave::saveAvi (const char *name)
{
uint32_t size;
uint8_t ret=0;
  strcpy(_name,name);
  //frametogo = frameEnd - frameStart + 1;
  frametogo=0;
  
  writter = new aviWrite ();
    // 1- setup audio
  guiStart();
  if (!setupAudio ())
    {
      guiStop();
      GUI_Error_HIG (_("Error initalizing audio filters"), NULL);
      deleteAudioFilter (audio_filter);
      delete writter;
      writter = NULL;
     // guiStop();
      return 0;
    }
   
   if (!setupVideo (_name))
    {
      guiStop();
      GUI_Error_HIG (_("Error initalizing video filters"), NULL);
      deleteAudioFilter (audio_filter);
      delete   	writter;
      
      writter = NULL;
     // guiStop();
      return 0;
    }
  
  // 3- setup video
  frametogo=_incoming->getInfo()->nb_frames;
  fps1000=_incoming->getInfo()->fps1000;
  printf ("\n writing %lu frames\n", frametogo);

  //__________________________________
  //   now go to main loop.....
  //__________________________________
  for (uint32_t cf = 0; cf < frametogo; cf++) 
    {
			
			
			
      			if (guiUpdate (cf, frametogo))
					goto abortme;
      			//   printf("\n %lu / %lu",cf,frametogo);
      			writeVideoChunk (cf);
      			writeAudioChunk (cf);
			//writter->sync();
     
     
    };				// end for
    ret=1;
abortme:
  guiStop ();
  //__________________________________
  // and end save
  //__________________________________
  writter->setEnd ();
  delete       writter;
  writter = NULL;
  deleteAudioFilter (audio_filter);
  // resync GUI
  printf ("\n Saving AVI (v_engine)... done\n");
  return ret;
}

//_________________________________________________________________
//
//                                                              Set up audio system
//_________________________________________________________________
uint8_t
GenericAviSave::setupAudio (void)
{
// 1- Prepare audio filter
//__________________________

  _audioInBuffer = 0;
  _audioTarget=_audioCurrent=0;
  _audioTotal=0;
  audio_filter=NULL;
   if(!currentaudiostream) 
   {
   	encoding_gui->setAudioCodec("None");
	return 1;
   }
  printf (" mux mode : %d mux param %d\n", muxMode, muxParam);

  if (audioProcessMode())	// else Raw copy mode
    {
      
      audio_filter = buildAudioFilter (currentaudiostream,video_body->getTime (frameStart));
      if(!audio_filter) return 0;
      encoding_gui->setAudioCodec(getStrFromAudioCodec(audio_filter->getInfo()->encoding));
    }
  else // copymode
    {
      // else prepare the incoming raw stream
      // audio copy mode here
      encoding_gui->setAudioCodec("Copy");
      audio_filter=buildAudioFilter( currentaudiostream,video_body->getTime (frameStart));
      if(!audio_filter) return 0;
    }

   
  return 1;
}




//---------------------------------------------------------------------------
uint8_t
GenericAviSave::writeAudioChunk (uint32_t frame)
{
  uint32_t    len;
  // if there is no audio, we do nothing
  if (!audio_filter)
    return 1;
    
  double t;
  
  t=frame+1;
  t=t/fps1000;
  t=t*1000*audio_filter->getInfo()->frequency;
  _audioTarget=(uint32_t )floor(t);
  
  	uint32_t sample,packetLen,packets=0;
	

	if(audio_filter->packetPerFrame()
		|| audio_filter->isVBR() )
	{
		while(_audioCurrent<_audioTarget)
		{
			if(!audio_filter->getPacket(abuffer,&packetLen,&sample))
			{
				printf("AVIWR:Could not read packet\n");
				return 0;
			}
			_audioCurrent+=sample;
	 		writter->saveAudioFrame (packetLen,abuffer);
			encoding_gui->feedAudioFrame(packetLen);
		}
	 	return 1;
	}

	sample=0;
	// _audioTarget is the # of sample we want
	while(_audioCurrent<_audioTarget)
	{
		if(!audio_filter->getPacket(abuffer+_audioInBuffer,&packetLen,&sample))
		{
			printf("AVIWR:Could not read packet\n");
			break;
		}
		_audioInBuffer+=packetLen;
		_audioTotal+=packetLen;
		_audioCurrent+=sample;		
		packets++;
	}
//   	printf("Aviwr:Fq:%lu fps1000:%lu frame:%lu Found %lu packet for %lu bytes , cur=%lu target=%lu total:%llu\n",
// 					audio_filter->getInfo()->frequency,
// 					fps1000,
// 					frame,packets,_audioInBuffer,
// 					_audioCurrent,_audioTarget,_audioTotal);
//   
      switch (muxMode)
	{

	case MUX_N_FRAMES:
	  stored_audio_frame++;
	  if (stored_audio_frame < muxParam)
	    return 1;
	  stored_audio_frame = 0;
	case MUX_REGULAR:

	 
	  break;
	case MUX_N_BYTES:
	  	if(_audioInBuffer<muxParam) return 1;
		break;
	  break;
	default:
	  ADM_assert (0);
	}
      
     
      
      if (_audioInBuffer)
	{
	  writter->saveAudioFrame (_audioInBuffer, abuffer);
	  encoding_gui->feedAudioFrame(_audioInBuffer);
	  _audioInBuffer=0;
	}
      return 1;
  
}

void
GenericAviSave::guiStart (void)
{
	encoding_gui=new DIA_encoding(25000);
	encoding_gui->setCodec("Copy");
	encoding_gui->setFrame (0, 100);
        encoding_gui->setContainer("Avi");

}

void
GenericAviSave::guiStop (void)
{
 	ADM_assert(encoding_gui);
  	delete encoding_gui;
  	encoding_gui=NULL;

}
void GenericAviSave::guiSetPhasis(char *str)
{
	ADM_assert(encoding_gui);
	encoding_gui->setPhasis(str);
	
}
uint8_t
GenericAviSave::guiUpdate (uint32_t nb, uint32_t total)
{
  ADM_assert(encoding_gui);
  encoding_gui->setFrame (nb, total);
  if ( encoding_gui->isAlive () == 1)
    return 0;
  return 1;


}
//	Return 1 if we exceed the chunk limit
//
uint8_t  GenericAviSave::handleMuxSize ( void )
{
	uint32_t pos;
	
			pos=writter->getPos();
			if(pos>=muxSize*1024*1024)
				{
					 return 1  ;
					
				}
				return 0;
	
	
}
//
//	Finish the current avi and start a new one
//
uint8_t   GenericAviSave::reigniteChunk( uint32_t dataLen, uint8_t *data )
{
	// MOD Feb 2005 by GMV: ODML exit
	if(writter->doODML!=aviWrite::NO)return 1;	// exit if odml has to be used
	// END MOD Feb 2005 by GMV
	
	    // first end up the current chunk
	     	writter->setEnd ();
  			delete       writter;
  			writter = NULL;
     	// then create a new one
         writter = new aviWrite ();
    
				_part++;
				char n[500];
				
				sprintf(n,"%s%02d",_name,_part);
								         
      	 printf("\n *** writing a new avi part :%s\n",n);
          
	        if (!writter->saveBegin (n,
			   &_mainaviheader,
			   frameEnd - frameStart + 1, 
			   &_videostreamheader,
			   &_bih,
			   data,dataLen,
			   audio_filter,
			   audio_filter2))
    {
      GUI_Error_HIG (_("Cannot initiate save"), NULL);

      return 0;
    }
    return 1;
}

/**
	Search Forward Reference frame from the current B frame
*/
uint32_t GenericAviSave::searchForward(uint32_t startframe)
{
		uint32_t fw=startframe;
		uint32_t flags;
		uint8_t r;

			while(1)
			{
				fw++;
				r=video_body->getFlags (fw, &flags);
				if(!(flags & AVI_B_FRAME))
				{
					return fw;

				}
				ADM_assert(r);
				if(!r)
				{
					printf("\n Could not locate last non B frame \n");
					return 0;
				}

			}
}

const char *getStrFromAudioCodec( uint32_t codec)
{
	switch(codec)
	{
                case WAV_DTS: return (const char *)"DTS";
		case WAV_PCM: return (const char *)"PCM";
		case WAV_MP2: return (const char *)"MP2";
		case WAV_MP3: return (const char *)"MP3";
		case (WAV_WMA):  return (const char *)"WMA";
		case (WAV_LPCM): return (const char *)"LPCM";	
		case (WAV_AC3):  return (const char *)"AC3";
		case (WAV_OGG): return (const char *)"Ogg Vorbis";
		case (WAV_MP4): return (const char *)"MP4 audio";
		case (WAV_AAC): return (const char *)"AAC";
		case (WAV_QDM2): return (const char *)"QDM2";
		case (WAV_AMRNB): return (const char *)"AMR narrow band";
		case (WAV_MSADPCM): return (const char *)"MSADPCM";
		case (WAV_ULAW): return (const char *)"ULAW";
                case WAV_IMAADPCM: return (const char *)"IMA ADPCM";
                case WAV_8BITS_UNSIGNED:return (const char *)"PCM 8bits";
	}

	return (const char *)"Unknown codec";

}

//---------------------------------------

// EOF
