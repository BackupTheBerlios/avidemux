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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "config.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "ADM_lavcodec.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/bitmap.h"
#include "subchunk.h"
#include "avilist.h"




#include "ADM_audio/aviaudio.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
//#include "ADM_gui/GUI_vars.h"
#include "ADM_gui/GUI_mux.h"
#include "oplug_mpeg/op_mpeg.h"
#include "oplug_mpegFF/oplug_vcdff.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_piper.h"
#include "prefs.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_colorspace/colorspace.h"
#include "ADM_audiodevice/audio_out.h"

#include "ADM_dialog/DIA_busy.h"
#include "ADM_dialog/DIA_working.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"

#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_codecs/ADM_ffmpeg.h"

#include "mpeg2enc/ADM_mpeg2enc.h"
uint8_t DIA_Requant(float *perce,uint32_t *quality,uint64_t init);
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
	DIA_working	*work;
	
	float 		percent;
	uint32_t 	quality;
	
	if(!prefs->get(REQUANT_PATH, &requant))
	{
		GUI_Alert("Use misc->prefs to tell where m2vrequant is\n");
		return;
	}
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
	if(! DIA_Requant(&percent,&quality,size)) return;
	
	printf("Using shrink factor %f, with qual=%lu\n",percent,quality);
	
	// now get the name
	GUI_FileSelWrite("Requantized file to write ?", &out_name);
	if(!out_name) return;
	
	
	A_requantize2(percent,quality,out_name);
}
void A_requantize2( float percent, uint32_t quality, char *out_name )	
{
	FILE 		*file=NULL;
	char 		*requant;
	char 		cmd[1024];
	uint32_t 	len,flags;
	DIA_working	*work;
	
	if(!prefs->get(REQUANT_PATH, &requant))
	{
		GUI_Alert("Use misc->prefs to tell where m2vrequant is\n");
		return;
	}
	
	uint64_t size=0;
	uint32_t fsize=0;
	uint8_t *buffer = new uint8_t[avifileinfo->width * avifileinfo->height * 2];
 
	// Seq header must be taken this time
 	video_body->getRawStart ( 0,(uint8_t*)buffer, &len);
	size=len;
	for(uint32_t i=frameStart;i<frameEnd;i++)
	{
		fsize=0;
		video_body->getFrameSize(i,&fsize);
		size+=fsize;
	}
	
	
	printf("Total size %llu bytes\n",size);
	// Format is downquantize method [0-3] and size of stream
	sprintf(cmd,"%s %f %lu %llu > %s",requant,percent,quality,size,out_name);
	printf("Cmd : %s\n",cmd);
	// send header
	// recheck the file is there
	file=fopen(requant,"rb");
	if(!file)	
		{
			GUI_Alert("Cannot locate M2Vrequant\nUse misc->preference");
			return;
		}
	fclose(file);
	file=popen(cmd,"w");
	if(!file)
		{	// probably too late if it fails
			GUI_Alert("Cannot locate M2Vrequant\nUse misc->preference");
			return;
		}
	printf("Sending seq header\n");
	fwrite(buffer,1,len,file);
	work=new DIA_working("Requantizing mpeg stream");	
	//  	and then body
	//	We re-order to have DTS order
	//
	for(uint32_t i=frameStart;i<frameEnd;i++)
	{
	
      		//printf("Frame %lu/%lu\n",i,avifileinfo->nb_frames);
		work->update (i-frameStart ,frameEnd-frameStart);
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
	  		if (!found)
	    			goto _abt;
	  		// Write the found frame
	  		video_body->getRaw (found, buffer, &len);
	  		fwrite (buffer, len, 1, file);
	  		// and the B frames
	  		for (uint32_t j = i; j < found; j++)
	    		{
	      			video_body->getRaw (j, buffer, &len);
	      			fwrite (buffer, len, 1, file);
	    		}
	  		i = found;		// Will be plussed by for
		}
      		else			// P or I frame
		{
	  		video_body->getRaw (i, buffer, &len);
	  		fwrite (buffer, len, 1, file);	  		
		}
	}
_abt:
	delete work;
	delete [] buffer;
	pclose(file);
	GUI_Alert("Done!");

}
