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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
#include "avilist.h"

#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "ADM_gui/GUI_mux.h"

extern PARAM_MUX muxMode;
extern int muxParam;



#include "ADM_audiofilter/audioeng_buildfilters.h"

extern uint8_t audioShift;
extern int32_t audioDelay;

GenericAviSave::GenericAviSave ()
{

  has_audio_track = has_audio_vbr = 0;
  one_audio_stock = 0;

  vbuffer = new uint8_t[MAXIMUM_SIZE * MAXIMUM_SIZE * 3];
  abuffer = new uint8_t[96000];

  assert (vbuffer);
  assert (abuffer);

  audio_filter=NULL;
  audio_filter2=NULL;
  _part=0;
  dialog_work=NULL;
  _lastIPFrameSent=0xffffff;
  _incoming=NULL;
  encoding_gui=NULL;
  _videoProcess=0;
}

GenericAviSave::~GenericAviSave ()
{
  delete[]vbuffer;
  delete[]abuffer;
  _incoming=NULL;
}

//___________________________________________________________
//      Generic Save Avi loop
//
//___________________________________________________________
//
uint8_t
GenericAviSave::saveAvi (char *name)
{
uint32_t size;
  strcpy(_name,name);
  //frametogo = frameEnd - frameStart + 1;
  frametogo=0;
  writter = new aviWrite ();
    // 1- setup audio
  if (!setupAudio ())
    {
      GUI_Alert ("Error initalizing audio filters");
	   deleteAudioFilter ();
		delete writter;
      writter = NULL;
     // guiStop();
      return 0;
    }
   
   if (!setupVideo (name))
    {
      GUI_Alert ("Error initalizing video filters");
      deleteAudioFilter ();
      delete   	writter;
      
      writter = NULL;
     // guiStop();
      return 0;
    }
  guiStart();
   if(guiUpdate(0,100))
		   guiStart();
  // 3- setup video
 frametogo=_incoming->getInfo()->nb_frames;
  printf ("\n writing %lu frames\n", frametogo);

  //__________________________________
  //   now go to main loop.....
  //__________________________________
  for (uint32_t cf = 0; cf < frametogo; cf++) 
    {
			
			// update size
			size=writter->getPos();
			size=size/(1024*1024);				 
			guiSetSize( size);	 
			// update other fields
      			if (guiUpdate (cf, frametogo))
					goto abortme;
      			//   printf("\n %lu / %lu",cf,frametogo);
      			writeVideoChunk (cf);
      			writeAudioChunk ();
			writter->sync();
     
     
    };				// end for

abortme:
  guiStop ();
  //__________________________________
  // and end save
  //__________________________________
  writter->setEnd ();
  delete       writter;
  writter = NULL;
  deleteAudioFilter ();
  // resync GUI
  printf ("\n Saving AVI (v_engine)... done\n");
  return 1;
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

  stored_audio_frame = 0;
  printf ("\n mux mode : %d mux param %d", muxMode, muxParam);

  if (audioProcessMode && currentaudiostream)	// else Raw copy mode
    {
//Process mode
      if (currentaudiostream->isCompressed ())
	{
	  if (!currentaudiostream->isDecompressable ())
	    {
	      GUI_Alert ("I cannot decompress that \n audio stream!");
	      return 0;
	    }
	}

/* shut up ...      if (!GUI_Question ("Audio Processing is activated \n Continue?"))
	{
	  return 0;
	}
*/
      double
	byt;
      byt =
	video_body->getTime (frameEnd + 1) - video_body->getTime (frameStart);
      byt *= currentaudiostream->getInfo ()->frequency;
      byt *= currentaudiostream->getInfo ()->channels;
      byt *= 2;
      byt /= 1000.;




      audio_filter = buildAudioFilter (currentaudiostream,video_body->getTime (frameStart),
				  (uint32_t) floor (byt));

      if ((audio_filter)->getInfo ()->encoding == WAV_PCM)
	if (!GUI_Question ("Audio stream is not compressed\n Continue?"))
	  {
	    deleteAudioFilter ();
	    return 0;
	  }
    }
  else
    {
      // else prepare the incoming raw stream
      // audio copy mode here
      audio_filter = (AVDMGenericAudioStream *) currentaudiostream;
      if (currentaudiostream)
	{
	  uint32_t
	    tstart;
	  tstart = video_body->getTime (frameStart);
	  (audio_filter)->goToTime (tstart);
	  printf ("\n delay : %ld shift  : %d", audioDelay, audioShift);
	  if (audioDelay)
	    {
	      if (audioShift)
		{
		  AVDMProcessAudio_RawShift *
		    ts;
		  ts = new AVDMProcessAudio_RawShift (audio_filter,
						      audioDelay, tstart);

		  audio_filter = (AVDMProcessAudioStream *) ts;;
		  printf ("\n Raw Time shift activated with %ld ms",
			  audioDelay);
		}
	    }

	}
    }

  //
  //  Now that the filter is ready compute the duration of a audio chunk
  //
  // cbr only for now
  if (currentaudiostream)
    {
      WAVHeader *
	wav = NULL;

      wav = audio_filter->getInfo ();
      has_audio_vbr = audio_filter->isVBR ();
      computeAudioCBR ();

      if (has_audio_vbr)	//VBR
			{
	  			wav->blockalign = 1152;	// Trick ala nandub
			}
    	  else			// CBR
			{
	  		//wav->blockalign = 1;
			}
    }

  return 1;
}



//
//      Compute the duration of an audio chunk in cbr mode
//
uint8_t
GenericAviSave::computeAudioCBR (void)
{
  uint32_t
    one_frame;
  aviInfo
    info;

  double
    one_frame_double, one_delta_frame;
  WAVHeader *
    wav = NULL;

  assert (audio_filter);

  wav = audio_filter->getInfo ();
  //
  video_body->getVideoInfo (&info);

  // In case of process, we must take
  // the output fps in case the filter alters
  // the fps. Else we would end up with a badly muxed
  // file as the audio and video won't have the same 
  // clockrate.
  if(_videoProcess)
  {	ADV_Info *lastInfo;
  	lastInfo=getLastVideoFilter()->getInfo ();
	fps1000=lastInfo->fps1000;
  } // else in copy mode we take it from source
  else
  {
  	fps1000 = info.fps1000;
  }
  // compute duration of a audio frame
  // in ms
  assert (fps1000);
  printf ("\n fps : %lu\n", fps1000);
  one_frame_double = (double) fps1000;
  one_frame_double = 1. / one_frame_double;
  // now we have 1/1000*fps=1/1000*duration of a frame in second
  one_frame_double *= 1000000.;
  // in ms now;
  one_frame = (uint32_t) floor (one_frame_double);
  printf ("\n One audio frame : %lu ms\n", one_frame);


  double
    pcm;
  // *2 because one sample is 16 bits
  // fix hitokiri bug part 1.
  pcm = one_frame_double * 2 * wav->frequency * wav->channels;
  pcm /= 1000;
  one_pcm_audio_frame = (uint32_t) floor (pcm);
  printf ("\n one PCM audio frame is %lu bytes \n", one_pcm_audio_frame);

  // get the equivalent in bytes
  assert (wav);
  one_frame_double /= 1000.;	// go back to seconds
  one_frame_double *= wav->byterate;


  one_frame = (uint32_t) floor (one_frame_double);

  if (one_frame & 1)
    one_frame--;
  one_delta_frame = one_frame_double - one_frame;
  // Real ? correction of hitokiri bug
  one_delta_frame *= 1000;
  printf ("\n One audio frame : %lu bytes", one_frame);
  printf ("\n audio byterate  : %lu bytes/sec", wav->byterate);
//    one_frame = one_frame * 10 + (uint32_t) floor(one_delta_frame);
  one_audio_frame_full = one_frame;
  one_audio_frame_left = (uint32_t) floor (one_delta_frame);
  printf ("\n One partial audio frame : %lu ms\n", one_audio_frame_left);
  return 1;
}

//---------------------------------------------------------------------------
uint8_t
GenericAviSave::writeAudioChunk (void)
{
  uint32_t
    len;
  // if there is no audio, we do nothing
  if (!audio_filter)
    return 1;
  //******************************
  // Audio CBR
  //******************************
  if (!has_audio_vbr)
    {

      len = 0;
      one_audio_stock += one_audio_frame_left;
      one_audio_stock += 1000 * one_audio_frame_full;

      switch (muxMode)
	{

	case MUX_N_FRAMES:
	  stored_audio_frame++;
	  if (stored_audio_frame < muxParam)
	    return 1;
	  stored_audio_frame = 0;
	case MUX_REGULAR:

	  while (one_audio_stock > 1000)
	    {
	      len++;
	      one_audio_stock -= 1000;
	    }
	  break;
	case MUX_N_BYTES:
	  if (one_audio_stock < 1000 * muxParam)
	    return 1;
	  while (one_audio_stock > 1000 * muxParam)
	    {
	      len += muxParam;
	      one_audio_stock -= 1000 * muxParam;
	    }
	  break;
	default:
	  assert (0);
	}

      if (len & 1)
	{
	  len--;
	  one_audio_stock += 1000;
	}


      // dumb me !!!!
      // hitokiri fix 2
      //uint32_t len2=len;

      //printf("\n audio  asked : %lu",len);
      len = audio_filter->read (len, abuffer);
      //printf("\n audio  got : %lu",len);
      //printf("\n stock  : %lu",one_audio_stock);

      if(len&1)
      	{ printf("Odd!\n");}
      
      if (len)
	{
	  writter->saveAudioFrame (len, abuffer);
	}
      return 1;



    }
  //******************************
  // Audio VBR
  //******************************

  // Here we deal with VBR audio
  // The principle :
  // we think in term of *DECODED* PCM length
  // and ask for the equivalent to audio stream
  // In that case one_audio_stock can be negative
  // It represents the bucket of PCM frame
  // ask for
  uint32_t real, compressed;


  one_audio_stock = one_pcm_audio_frame + one_audio_stock;
  do
    {
      compressed = audio_filter->readPCMeq (0, abuffer, &real);
//     printf("\n asked  pcm: %lu got %lu pcm, soit %lu compressed",
      //                                                                    len,real,compressed);
      // compressed is what we have to write actually
      if (compressed)
	{
	  writter->saveAudioFrame (compressed, abuffer);
	}
      // now "real" is the PCM equivalent we got
      // and we asked len, store the delta to prevent error from
      // propagating
      one_audio_stock -= real;
    }
  while (one_audio_stock > 0 && (compressed));
  return 1;



}

void
GenericAviSave::guiStart (void)
{
	dialog_work=new DIA_working("Saving AVI");
	dialog_work->update(0,100);

}
void
GenericAviSave::guiSetSize (uint32_t size)
{
	

}
void
GenericAviSave::guiStop (void)
{
 	delete dialog_work;
	dialog_work=NULL;

}

uint8_t
GenericAviSave::guiUpdate (uint32_t nb, uint32_t total)
{
	assert(dialog_work);
  return dialog_work->update (nb, total);


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
      GUI_Alert ("Cannot initiate save:");

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
				assert(r);
				if(!r)
				{
					printf("\n Could not locate last non B frame \n");
					return 0;
				}

			}
}


// EOF
