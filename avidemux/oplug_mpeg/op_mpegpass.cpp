//
// C++ Implementation: op_mpegpass
//
// Description: 
//   This save mpeg video to mpegPS in video copy mode
//	Audio can be in copy mode or not.
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "ADM_library/default.h"
#include "avi_vars.h"
//#include "aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_dialog/DIA_working.h"

#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_lvemux/ADM_muxer.h"

/**
	Save a cut and audio edited to mpeg-PS
	Usefull for editing PVR captured files for example

*/
 AVDMGenericAudioStream *mpt_getAudioStream(uint32_t *pcm);

void mpeg_passthrough(  char *name )
{
  uint32_t len, flags;
  AVDMGenericAudioStream *audio=NULL;
  uint32_t pcm,audiolen;
  uint8_t *buffer = new uint8_t[avifileinfo->width * avifileinfo->height * 3];
  
  DIA_working *work;
  MpegMuxer *muxer=NULL;
  
  	printf("Saving as mpg PS to file %s\n",name);
  
  	// First we check it is mpeg
  	if(!fourCC::check(avifileinfo->fcc,(uint8_t *)"MPEG"))
  	{
  		GUI_Alert("This is not a mpeg.\nSwitch to process mode.");
		return ;
  	}
  	if(!currentaudiostream)
  	{
  		GUI_Alert("There is no audio track.");
		return ;
  	}
  
  	audio=mpt_getAudioStream(&pcm);
	if(!audio)
	{
		GUI_Alert("Audio track is not suitable!\n");
		return;
	}
		
	if(!pcm)
	{
		GUI_Alert("Audio track is not suitable(2)!\n");
		return;
	}
	
  	muxer=new MpegMuxer();
	if(!muxer->open(name,8000,avifileinfo->fps1000,audio->getInfo()))
	{
		delete muxer;
		muxer=NULL;
		printf("Muxer init failed\n");
		return ;
		
	}
  
  ///____________________________
  work=new DIA_working("Saving MpegPS stream");

  // preamble
  video_body->getRawStart (frameStart, buffer, &len);
  muxer->writeVideoPacket (len,buffer);

  for (uint32_t i = frameStart; i < frameEnd; i++)
    {
      work->update (i - frameStart, frameEnd - frameStart);
      if(!work->isAlive()) goto _abt;
      assert (video_body->getFlags (i, &flags));
      if (flags & AVI_B_FRAME)	// oops
	{
	  // se search for the next i /p
	  uint32_t found = 0;

	for (uint32_t j = i + 1; j < frameEnd; j++)
	{
		assert (video_body->getFlags (j, &flags));
		if (!(flags & AVI_B_FRAME))
		{
			found = j;
			break;
		}

	}
	if (!found)	    goto _abt;
	  // Write the found frame
	video_body->getRaw (found, buffer, &len);
	muxer->writeVideoPacket (len,buffer);
	audiolen=pcm;
	audiolen = audio->read (audiolen,buffer);
	if(audiolen)
		muxer->writeAudioPacket(audiolen,buffer);
	  
	  // and the B frames
	for (uint32_t j = i; j < found; j++)
	    {
		video_body->getRaw (j, buffer, &len);
		muxer->writeVideoPacket (len,buffer);
		audiolen=pcm;
		audiolen = audio->read (audiolen,buffer);
		if(audiolen)
			muxer->writeAudioPacket(audiolen,buffer);
	    }
	  i = found;		// Will be plussed by for
	}
      else			// P or I frame
	{
	  	video_body->getRaw (i, buffer, &len);
		muxer->writeVideoPacket (len,buffer);
		audiolen=pcm;
		audiolen = audio->read (audiolen,buffer);
		if(audiolen)
			muxer->writeAudioPacket(audiolen,buffer);
	}

    }
_abt:
  delete work;
  muxer->close();
  delete muxer;
  delete buffer;
  deleteAudioFilter();

}

AVDMGenericAudioStream *mpt_getAudioStream(uint32_t *mypcm)
{

// Second check the audio track
  	uint32_t fps1000;
	uint32_t one_pcm_audio_frame;
	int err;
	AVDMGenericAudioStream *_audio=NULL;
     
      	// compute the number of bytes in the incoming stream
      	// to feed the filter chain
      	double	byt;
      	byt =	video_body->getTime (frameEnd + 1) - video_body->getTime (frameStart);
      	byt *= currentaudiostream->getInfo ()->frequency;
      	byt *= currentaudiostream->getInfo ()->channels;
      	byt *= 2;
      	byt /= 1000.;

	if(audioProcessMode)
	{
      		_audio = buildAudioFilter (currentaudiostream,video_body->getTime (frameStart),
				  (uint32_t) floor (byt));
	}
	else
	{
	
        	uint32_t    tstart;
	  	tstart = video_body->getTime (frameStart);
		_audio = (AVDMGenericAudioStream *) currentaudiostream;
	  	_audio->goToTime (tstart);	  
	}
   	
	if(_audio->getInfo()->encoding!=WAV_MP2 && _audio->getInfo()->encoding!=WAV_AC3)
	{
		deleteAudioFilter();
		return NULL;
	}
	//________________
	uint32_t one_frame;
  	aviInfo   info;
	double    one_frame_double, one_delta_frame;
  	WAVHeader *wav = NULL;

  	assert (_audio);

  	wav = _audio->getInfo ();
  
  	fps1000 = avifileinfo->fps1000;
  	
  	// compute duration of a audio frame
  	// in ms
  	assert (fps1000);
  	printf (" fps : %lu\n", fps1000);
  	one_frame_double = (double) fps1000;
  	one_frame_double = 1. / one_frame_double;
  	// now we have 1/1000*fps=1/1000*duration of a frame in second
  	one_frame_double *= 1000000.;
  	// in ms now;
  	one_frame = (uint32_t) floor (one_frame_double);
  	printf (" One audio frame : %lu ms\n", one_frame);


  	double    pcm;
  	// *2 because one sample is 16 bits
  	// fix hitokiri bug part 1.
  	pcm = one_frame_double * 2 * wav->frequency * wav->channels;
  	pcm /= 1000;
  	one_pcm_audio_frame = (uint32_t) floor (pcm);
  	printf (" one PCM audio frame is %lu bytes \n", one_pcm_audio_frame);

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
  	
	*mypcm=one_frame;
	return _audio;
}

