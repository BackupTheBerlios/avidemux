/***************************************************************************
                          ADM_requantize  -  
                             ----------------
	Requantize a mpeg stream using 
	http://www.metakine.com/products/dvdremaster/modules.html			     
	M2vRequantizer
			     
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by mean
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
//#define TEST_MP2
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "ADM_lavcodec.h"

#include "fourcc.h"

#include "ADM_toolkit/filesel.h"

#include "avi_vars.h"
#include "ADM_assert.h"
#include "ADM_toolkit/toolkit.hxx"
#include "prefs.h"



#include "ADM_audio/aviaudio.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"

#include "ADM_audiofilter/audioeng_buildfilters.h"

#include "ADM_encoder/adm_encConfig.h"

#include "ADM_dialog/DIA_busy.h"
#include "ADM_dialog/DIA_encoding.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"

#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_REQUANT
#include "ADM_toolkit/ADM_debug.h"

extern "C"
{
	#include "ADM_requant/tcrequant.h"
}
#include "ADM_lvemux/ADM_muxer.h"


uint8_t DIA_Requant(float *perce,uint32_t *quality,uint64_t init,uint32_t audioSize);
void A_requantize2( float percent, uint32_t quality, char *out_name )	;

/*
	We pipe the raw mpeg stream to mpeg2requantizer

*/
void A_requantize( void )
{

	// Try to open requtantize
	
	char 		*requant;
	char 		cmd[1024],*out_name;
	uint32_t 	len,flags;
	
	float 		percent;
	uint32_t 	quality;
	AVDMGenericAudioStream *audio=NULL;
	uint32_t	audioSize;
	double 		audioInc;
	
	
	//
	// compute the size of the whole stream, we neglect seq header here
	uint64_t size=0;
	uint32_t fsize=0;
	
	for(uint32_t i=frameStart;i<frameEnd;i++)
	{
		fsize=0;
		video_body->getFrameSize(i,&fsize);
		size+=fsize;
	}
	//
	audio=mpt_getAudioStream(&audioInc,1);
	if(audio)
	{
		audioSize=audio->getLength();
	}
	else
		audioSize=0;
	//
	deleteAudioFilter();
	printf("Found audio :%lu\n",audioSize);
	if(! DIA_Requant(&percent,&quality,size,audioSize)) return;
	
	printf("Using shrink factor %f, with qual=%lu\n",percent,quality);
	
	// now get the name
	GUI_FileSelWrite("Requantized file to write ?", &out_name);
	if(!out_name) return;
	
	A_requantize2(percent,quality,out_name);
}

#define PACK_AUDIO 	{ uint32_t audiolen=0, audioread=0;	\
				audioWanted+=audioInc; \
				if(muxer->audioEmpty()) \
	 				audiolen=(uint32_t)floor(8+audioWanted-audioGot);\
				else \
 					audiolen=(uint32_t)floor(audioWanted-audioGot);\
				audioread = audio->read (audiolen,audioBuffer); \
				if(audioread!=audiolen) printf("Mmm not enough audio..\n"); \
				muxer->writeAudioPacket(audioread,audioBuffer);\
				audioGot+=audioread;}
#define PACK_FRAME(i) \
			video_body->getRaw (i, buffer, &len); \
			Mrequant_frame(buffer,  len,outbuffer, &lenout); \
			encoding->feedFrame(lenout);	\
			aprintf("%lu in:%03lu out:%03lu\n",i,len>>10,lenout>>10); \
			if(!muxer) \
	  			fwrite (outbuffer, lenout, 1, file); \
			else\
			{\
				muxer->writeVideoPacket(lenout,outbuffer); \
				aprintf("Requant in %x %x %x %x\n",buffer[0],buffer[1],buffer[2],buffer[3]); \
				aprintf("Requant out %x %x %x %x %x %x\n",outbuffer[0],\
					outbuffer[1],outbuffer[2],outbuffer[3],outbuffer[4],outbuffer[5]); \
				PACK_AUDIO; \
			}
			
			
//			printf("%lu %lu -> %lu\n",i,len,lenout); 
			
void A_requantize2( float percent, uint32_t quality, char *out_name )	
{
	FILE 		*file=NULL;
	uint32_t 	len,flags,lenout;
	DIA_encoding	*encoding=NULL;
	AVDMGenericAudioStream *audio=NULL;
	double 		audioInc=0;
	double		audioWanted=0,audioGot=0;
	uint32_t	audioLen;
	MpegMuxer	*muxer=NULL;
	uint32_t	fps1000=0;
	
	
	uint64_t size=0;
	uint32_t fsize=0;
	uint8_t *buffer 	= new uint8_t[avifileinfo->width * avifileinfo->height * 2];
	uint8_t *outbuffer 	= new uint8_t[avifileinfo->width * avifileinfo->height * 2];
	uint8_t *audioBuffer 	= new uint8_t[56000*10]; //  10 sec of audio at max bitrate
 
	Mrequant_init (percent,1);
	
	
 	
	// get audio if any
	audio=mpt_getAudioStream(&audioInc);
	if(audio)
	{
		fps1000=avifileinfo->fps1000;
		muxer=new MpegMuxer;
		if(!muxer->open(out_name,MUX_MPEG_VRATE,fps1000,audio->getInfo(),(float)audioInc))
		{
			GUI_Alert("Muxer init failed\n");
			goto _abt;
		}
		// The requantizer will not alter the gop timestamp
		// they (may) need to be resynced
		muxer->forceRestamp();
		
	}
	else
	{
		file=fopen(out_name,"wb");
		if(!file)
		{
			GUI_Alert("Problem writing file");
			goto _abt;
		}			
	}
	
	
		
	
	encoding=new DIA_encoding(avifileinfo->fps1000);

	encoding->setPhasis("Requantizing.");
	encoding->setFrame(0,frameEnd-frameStart);
	
	
	for(uint32_t i=frameStart;i<frameEnd;i++)
	{
		encoding->setFrame(i-frameStart,frameEnd-frameStart);
		
      		if(!encoding->isAlive()) goto _abt;
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
	    			goto _abt;
	  		// Write the found frame
	  		PACK_FRAME(found);
	  		// and the B frames
	  		for (uint32_t j = i; j < found; j++)
	    		{
	      			PACK_FRAME(j);
			}
	  		i = found;		// Will be plussed by for
		}
      		else	// P or I frame
		{	// First frame is special as we want the stream to begin by a SEQ_HEADER
			// In case it is aleady the case do nothing
			// If not, we fetch the seq header from the extraData, hoping it is there
			if(i==frameStart)
			{
				video_body->getRaw (i, buffer, &len);
				
				if(buffer[0]==0 && buffer[1]==0 && buffer[2]==1 && buffer[3]==0xb3) // Seq start
				{	
					printf("1st frame contains the seq header\n");
					Mrequant_frame(buffer,  len,outbuffer, &lenout); 	
					encoding->feedFrame(lenout);	
					if(!muxer) 
	  					fwrite (outbuffer, lenout, 1, file); 
					else
					{
						muxer->writeVideoPacket(lenout,outbuffer); 
						aprintf("in:%03lu out:%03lu\n",len>>10,lenout>>10); \
						PACK_AUDIO; 
					}
				}
				else // need to add seq start
				{
					uint32_t seq;
					video_body->getRawStart (frameStart, buffer, &seq);	
					printf("Adding seq header (%lu)\n",seq);
					
	  				video_body->getRaw (i, buffer+seq, &len);
					printf("Stat : %x %x %x %x\n",buffer[seq],buffer[seq+1],buffer[seq+2],
								buffer[seq+3]);
					Mrequant_frame(buffer,  len+seq,outbuffer, &lenout);
					encoding->feedFrame(lenout);	 
	  				if(!muxer) 
	  					fwrite (outbuffer, lenout, 1, file); 
					else
					{
						muxer->writeVideoPacket(lenout,outbuffer); 
						PACK_AUDIO; 
						aprintf("in:%03lu out:%03lu\n",len>>10,lenout>>10); \
					}
				}
			}
			else
	  			PACK_FRAME(i);	
		}
	}
_abt:
	if(encoding) delete encoding;
	if(buffer) delete [] buffer;
	if(outbuffer) delete [] outbuffer;
	if(audioBuffer) delete [] audioBuffer;
	if(file) fclose(file);
	if(muxer) {	muxer->close();
			delete muxer;
			muxer=NULL;
		   }
	Mrequant_end();
	deleteAudioFilter();
	GUI_Alert("Done!");

}
