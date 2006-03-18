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
#include "ADM_dialog/DIA_encoding.h"

#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_outputfmt.h"
#include "ADM_lavformat/ADM_lavformat.h"

#include "ADM_lvemux/ADM_muxer.h"

// To have access to low level infos 
#include "ADM_codecs/ADM_mpeg.h"
#include "ADM_lavcodec.h"
#include "ADM_codecs/ADM_ffmp43.h"



/**
	Save a cut and audio edited to mpeg-PS
	Usefull for editing PVR captured files for example

*/
#define PACK_AUDIO(x) \
{ \
	uint32_t samples; \
	uint32_t fill=0; \
        if(total_got<target_sample) \
	while(muxer->needAudio()) \
	{				\
		if(!audio->getPacket(audiobuffer, &audiolen, &samples))	\
		{ \
			printf("passthrough:Could not get audio\n"); \
			break; \
		}\
		if(audiolen) {\
			muxer->writeAudioPacket(audiolen,audiobuffer); \
                        work->feedAudioFrame(audiolen);\
                        }\
		total_got+=samples; \
	} \
}

uint8_t isMpeg12Compatible(uint32_t fourcc);
extern const char *getStrFromAudioCodec( uint32_t codec); 
uint8_t mpeg_passthrough(const char *name,ADM_OUT_FORMAT format )
{
  uint32_t len, flags;
  AVDMGenericAudioStream *audio=NULL;
  uint32_t audiolen;
  
  DIA_encoding *work;
  ADM_MUXER_TYPE mux;
  
  double total_wanted=0;
  double total_got=0;
  uint8_t ret=0;
 
  ADMMpegMuxer *muxer=NULL;
  ADMBitstream bitstream;
  
  	printf("Saving as mpg PS to file %s\n",name);
  
  	// First we check it is mpeg
	if(!isMpeg12Compatible(avifileinfo->fcc))
  	{
  		GUI_Error_HIG("This is not MPEG compatible", "You can't use the Copy codec.");
		return 0 ;
  	}
  	if(!currentaudiostream)
  	{
  		GUI_Error_HIG("There is no audio track", NULL);
		return 0;
  	}
  
	ADM_assert (video_body->getFlags (frameStart, &flags));
        if(!(flags&AVI_KEY_FRAME))
        {
                GUI_Error_HIG("The first frame is not intra frame", "Use the &lt;&lt; and the &gt;&gt; buttons to move using Intra frames.");
                return 0;
        }
	
  	audio=mpt_getAudioStream();
	
	// Have to check the type
	// If it is mpeg2 we use DVD-PS
	// If it is mpeg1 we use VCD-PS
	// Later check if it is SVCD
	if(!audio)
	{
		GUI_Error_HIG("Audio track is not suitable", NULL);
		return 0;
	}
	// Check
	WAVHeader *hdr=audio->getInfo();
	uint32_t isMpeg1;
	uint32_t isLav;
	if(!prefs->get(FEATURE_USE_LAVCODEC_MPEG, &isLav))
		{
		 isLav=0;
		}

	if(!isLav)
	{
		decoderMpeg *mpeghdr;
	
		mpeghdr=(decoderMpeg *)video_body->rawGetDecoder(0);
		isMpeg1=mpeghdr->isMpeg1();
	}
	else
	{
		// How to know if it is mpeg 1?
		// Assume it is not
		/*
		decoderFFMpeg12 *mpeghdr;
	
		mpeghdr=(decoderFFMpeg12 *)video_body->rawGetDecoder(0);
		isMpeg1=mpeghdr->isMpeg1();
		*/
		isMpeg1=0;
	
	}
	
	switch(format)
        {
        case ADM_PS:	
                if(isMpeg1)
                {
                        if(hdr->frequency!=44100 ||  hdr->encoding != WAV_MP2)
                        {
                                GUI_Error_HIG("Incompatible audio", "For VCD, audio must be 44.1 kHz MP2.");
                                return 0 ;
                        }
                        mux=MUXER_VCD;
                        printf("PassThrough: Using VCD PS\n");        
                        }else
                {    // Mpeg2 
                        aviInfo info;
                        video_body->getVideoInfo(&info);
                        if(hdr->frequency==44100 && info.width==480&& hdr->encoding == WAV_MP2 ) // SVCD ?
                        {
                                mux=MUXER_SVCD;
                                printf("PassThrough: Using SVCD PS\n");
                        }
                        else
                        {
                                 // mpeg2, we do only DVD right now
                                if(hdr->frequency!=48000 || 
                                (hdr->encoding != WAV_MP2 && hdr->encoding!=WAV_AC3 && hdr->encoding!=WAV_LPCM))
                                {
                                        deleteAudioFilter();
                                        GUI_Error_HIG("Incompatible audio", "For DVD, audio must be 48 kHz MP2, AC3 or LPCM.");
                                        return 0;
                                }
                                mux=MUXER_DVD;
                                printf("PassThrough: Using DVD PS\n");
                        }
                }

  	        muxer=new mplexMuxer();
#if 0
#warning TEST
#warning TEST
#warning TEST
#warning TEST
#warning TEST
#warning TEST
                muxer=new lavMuxer();
#endif
                break;
        case ADM_TS:     
             printf("Using TS output format\n");   
             muxer=new tsMuxer(); //lavMuxer();
             mux=MUXER_TS;
             break;
        default:
                ADM_assert(0);
                break;
        }
        
        if(!muxer)
         {
                 printf("No muxer ?\n");
                 return 0;
        }
	if(!muxer->open(name,0,mux,avifileinfo,audio->getInfo()))
	{
		delete muxer;
		muxer=NULL;
		printf("Muxer init failed\n");
		return 0;
		
	}
        
	// In copy mode it is better to recompute the gop timestamp
	muxer->forceRestamp();
  ///____________________________
  work=new DIA_encoding(avifileinfo->fps1000);
  work->setCodec("Copy");
  work->setAudioCodec("---");
  work->setPhasis("Saving");
  if(!audioProcessMode())
     work->setAudioCodec("Copy");
  else
     work->setAudioCodec(getStrFromAudioCodec(audio->getInfo()->encoding));
  switch(mux)
  {
    case MUXER_TS: work->setContainer("Mpeg TS");break;
    case MUXER_VCD: work->setContainer("Mpeg VCD");break;
    case MUXER_SVCD: work->setContainer("Mpeg SVCD");break;
    case MUXER_DVD: work->setContainer("Mpeg DVD");break;
    default:
        ADM_assert(0);
  }
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
  uint32_t target_sample=0;
  double target_time;
  aviInfo info;
        video_body->getVideoInfo(&info);
        target_time=frameEnd-frameStart+1;
        target_time*=1000;
        target_time/=info.fps1000; // target_time in second
        target_time*=audio->getInfo()->frequency;
        target_sample=(uint32_t)floor(target_time);

  uint8_t *buffer = new uint8_t[avifileinfo->width * avifileinfo->height * 3];
  uint8_t *audiobuffer = new uint8_t[4*48000*2]; // 2 sec worth of lpcm

        bitstream.data=buffer;
        PACK_AUDIO(0);

  for (uint32_t i = frameStart; i < frameEnd; i++)
    {
      
      work->setFrame(i - frameStart, frameEnd - frameStart);
      
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
	if (!found)
        {
                        // Last gop might be incomplete
                    if( abs(i-frameEnd)<25) ret=1;
                    else ret=0;
        	    goto _abt;
        }
	  // Write the found frame
	video_body->getRaw (found, bitstream.data, &bitstream.len);
        bitstream.dtsFrame=cur++;
        bitstream.ptsFrame=found;
        muxer->writeVideoPacket(&bitstream);
//	muxer->writeVideoPacket (len,buffer,cur++,found-frameStart);	
        work->feedFrame(bitstream.len);
	PACK_AUDIO(0)
	
	  
	  // and the B frames
	for (uint32_t j = i; j < found; j++)
	    {
                video_body->getRaw (j, bitstream.data, &bitstream.len);
                bitstream.dtsFrame=cur++;
                bitstream.ptsFrame=j-frameStart;
		muxer->writeVideoPacket (&bitstream);
                work->feedFrame(bitstream.len);
		PACK_AUDIO(0)
		
    	    }
	  i = found;		// Will be plussed by for
	}
      else			// P or I frame
	{
		if(i==frameStart) // Pack sequ_start with the frame
		{
			// Check if seq header is there..
			video_body->getRaw (i, bitstream.data, &bitstream.len);
			if(buffer[0]==0 && buffer[1]==0 && buffer[2]==1 && buffer[3]==0xb3) // Seq start
			{
                                bitstream.dtsFrame=cur++;
                                bitstream.ptsFrame=i-frameStart;
				muxer->writeVideoPacket (&bitstream);
                                work->feedFrame(bitstream.len);
				PACK_AUDIO(0)
				
				
			}
			else // need to add seq start
			{
				uint32_t seq;
				video_body->getRawStart (frameStart, buffer, &seq);  		
	  			video_body->getRaw (i, buffer+seq, &len);
                                bitstream.len=len+seq;
                                bitstream.dtsFrame=cur++;
                                bitstream.ptsFrame=i-frameStart;
				muxer->writeVideoPacket (&bitstream);
                                work->feedFrame(bitstream.len);
				PACK_AUDIO(0)
				
			}
		}
		else
		{
			video_body->getRaw (i, buffer, &bitstream.len);
                        bitstream.dtsFrame=cur++;
                        bitstream.ptsFrame=i-frameStart;
			muxer->writeVideoPacket (&bitstream);
                        work->feedFrame(bitstream.len);
			PACK_AUDIO(0)
			
		}
	}

    }
    ret=1;
_abt:
  delete work;
  muxer->close();
  delete muxer;
  delete [] buffer;
  delete [] audiobuffer;
  deleteAudioFilter();
  return ret;

}
