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
#include <time.h>
#include <sys/time.h>
#include <errno.h>


#include "ADM_library/default.h"
#include "avi_vars.h"
//#include "aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include <ADM_assert.h>
#include "ADM_dialog/DIA_working.h"

#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_lavformat/ADM_lavformat.h"
#include "ADM_lvemux/ADM_muxer.h"

// To have access to low level infos 
#include "ADM_codecs/ADM_mpeg.h"


/**
	Save a cut and audio edited to mpeg-PS
	Usefull for editing PVR captured files for example

*/
#define PACK_AUDIO(x) \
{ \
	uint32_t samples; \
	uint32_t fill=0; \
	while(muxer->needAudio()) \
	{				\
		if(!audio->getPacket(buffer, &audiolen, &samples))	\
		{ \
			printf("passthrough:Could not get audio\n"); \
			break; \
		}\
		if(audiolen) \
			muxer->writeAudioPacket(audiolen,buffer); \
		total_got+=audiolen; \
	} \
}

uint8_t isMpeg12Compatible(uint32_t fourcc);
 
void mpeg_passthrough(  char *name )
{
  uint32_t len, flags;
  AVDMGenericAudioStream *audio=NULL;
  uint32_t audiolen;
  uint8_t *buffer = new uint8_t[avifileinfo->width * avifileinfo->height * 3];
  
  DIA_working *work;
  ADM_MUXER_TYPE mux;
  
  double total_wanted=0;
  double total_got=0;
 
 //#define MYMUXER MpegMuxer
 #define MYMUXER lavMuxer
  MYMUXER *muxer=NULL;
  
  	printf("Saving as mpg PS to file %s\n",name);
  
  	// First we check it is mpeg
	if(!isMpeg12Compatible(avifileinfo->fcc))
  	{
  		GUI_Alert("This is not a mpeg.\nSwitch to process mode.");
		return ;
  	}
  	if(!currentaudiostream)
  	{
  		GUI_Alert("There is no audio track.");
		return ;
  	}
  
	
	
  	audio=mpt_getAudioStream();
	
	// Have to check the type
	// If it is mpeg2 we use DVD-PS
	// If it is mpeg1 we use VCD-PS
	// Later check if it is SVCD
	if(!audio)
	{
		GUI_Alert("Audio track is  not suitable!\n");
		return;
	}
	// Check
	WAVHeader *hdr=audio->getInfo();
	
	decoderMpeg *mpeghdr;
	
	mpeghdr=(decoderMpeg *)video_body->rawGetDecoder(0);
	if(mpeghdr->isMpeg1())
	{
		if(hdr->frequency!=44100 ||  hdr->encoding != WAV_MP2)
		{
			GUI_Alert("This is not compatible with VCD mpeg.\n");
			return ;
		}
		mux=MUXER_VCD;
		printf("PassThrought: Using VCD PS\n");
	
	}else
	{     // mpeg2, we do only DVD right now
		if(hdr->frequency!=48000 || 
		(hdr->encoding != WAV_MP2 && hdr->encoding!=WAV_AC3))
		{
			deleteAudioFilter();
			GUI_Alert("Audio track is not suitable!\n");
			return ;
		}
		mux=MUXER_DVD;
		printf("PassThrought: Using DVD PS\n");
	}
	
	
  	muxer=new MYMUXER();
	if(!muxer->open(name,0,mux,avifileinfo,audio->getInfo()))
	{
		delete muxer;
		muxer=NULL;
		printf("Muxer init failed\n");
		return ;
		
	}
	// In copy mode it is better to recompute the gop timestamp
	muxer->forceRestamp();
  ///____________________________
  work=new DIA_working("Saving MpegPS stream");

  // preamble
  /*
  video_body->getRawStart (frameStart, buffer, &len);
  muxer->writeVideoPacket (len,buffer);
*/
// 	while(total_got<3000)
// 	{
//   		PACK_AUDIO(0)
// 	}
  uint32_t cur=0;
  for (uint32_t i = frameStart; i < frameEnd; i++)
    {
      
      
      work->update (i - frameStart, frameEnd - frameStart);
      if(!work->isAlive()) goto _abt;
      ADM_assert (video_body->getFlags (i, &flags));
      if (flags & AVI_B_FRAME)	// oops
	{
	  // se search for the next i /p
	  uint32_t found = 0;

	for (uint32_t j = i + 1; j < frameEnd; j++)
	{
		ADM_assert (video_body->getFlags (j, &flags));
		if (!(flags & AVI_B_FRAME))
		{
			found = j;
			break;
		}

	}
	if (!found)	    goto _abt;
	  // Write the found frame
	video_body->getRaw (found, buffer, &len);
	
	muxer->writeVideoPacket (len,buffer,cur++,found-frameStart);	
	PACK_AUDIO(0)
	
	  
	  // and the B frames
	for (uint32_t j = i; j < found; j++)
	    {
		video_body->getRaw (j, buffer, &len);
		
		muxer->writeVideoPacket (len,buffer,cur++,j-frameStart);
		PACK_AUDIO(0)
		
    	    }
	  i = found;		// Will be plussed by for
	}
      else			// P or I frame
	{
		if(i==frameStart) // Pack sequ_start with the frame
		{
			// Check if seq header is there..
			video_body->getRaw (i, buffer, &len);
			if(buffer[0]==0 && buffer[1]==0 && buffer[2]==1 && buffer[3]==0xb3) // Seq start
			{
				muxer->writeVideoPacket (len,buffer,cur++,i-frameStart);
				PACK_AUDIO(0)
				
				
			}
			else // need to add seq start
			{
				uint32_t seq;
				video_body->getRawStart (frameStart, buffer, &seq);  		
	  			video_body->getRaw (i, buffer+seq, &len);
				
				muxer->writeVideoPacket (len+seq,buffer,cur++,i-frameStart);
				PACK_AUDIO(0)
				
			}
		}
		else
		{
			video_body->getRaw (i, buffer, &len);
			
			muxer->writeVideoPacket (len,buffer,cur++,i-frameStart);
			PACK_AUDIO(0)
			
		}
	}

    }
_abt:
  delete work;
  muxer->close();
  delete muxer;
  delete buffer;
  deleteAudioFilter();

}
