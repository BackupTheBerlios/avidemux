//
// C++ Implementation: op_ogsave
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <math.h>
#include <time.h>
#include <sys/time.h>


#include <ADM_assert.h>
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
//#include "avilist.h"

#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_library/default.h"
#include "oplug_ogm/op_ogsave.h"

//_______________________________________________
uint8_t ogmSave(char  *name)
{
uint8_t ret;
	ADM_ogmWrite *writter;
	if(videoProcessMode())
		writter=new ADM_ogmWriteProcess;
	else
		writter=new ADM_ogmWriteCopy;
	writter->save(name);
	
	delete writter;
	return ret;
}

//_______________________________________________
ADM_ogmWrite::ADM_ogmWrite(void)
{
_audioBuffer=_videoBuffer=NULL;
videoStream=videoStream=NULL;
encoding_gui=NULL;
audioFilter=NULL;
_fd=NULL;
_togo=0;
_packet=0;
TwoPassLogFile=NULL;
}

//_______________________________________________
ADM_ogmWrite::~ADM_ogmWrite()
{
#define FREE_IF(x) if(x) {delete x;x=NULL;}
#define FREE_IFX(x) if(x) {delete [] x;x=NULL;}

	FREE_IFX(_audioBuffer);
	FREE_IFX(_videoBuffer);
	FREE_IF(videoStream);
	FREE_IF(videoStream);
	FREE_IF(encoding_gui);
	FREE_IF(audioFilter);
	
	if(_fd) fclose(_fd);
	_fd=NULL;
	if(TwoPassLogFile)
		delete [] TwoPassLogFile;
	TwoPassLogFile=NULL;


}
//_______________________________________________
uint8_t ADM_ogmWrite::save(char *name)
{

uint8_t *bufr;
uint32_t len,flags;
uint8_t error=0;


		_fd=fopen(name,"wb");
		if(!_fd)
		{
			GUI_Error_HIG("File error", "Cannot open \"%s\" for writing.", name);
			return 0;
		}

		videoStream=new ogm_page(_fd,1);
	
		encoding_gui=new DIA_encoding(25000);
                encoding_gui->setContainer("OGM");
		//______________ Write headers..._____________________
		
		if(!initVideo(name))
		{
			fclose(_fd);
			_fd=NULL;
			GUI_Error_HIG("Could not initialize video", NULL);
			return 0;
		
		}
		if(!initAudio())
		{
			fclose(_fd);
			_fd=NULL;
			GUI_Error_HIG("Could not initialize audio", NULL);
			return 0;
		
		}

		encoding_gui->setFps(_fps1000);
		encoding_gui->reset();
		// ___________________Then body_______________________
		for(uint32_t j=0;j<=_togo && !error;j++)
		{
			encoding_gui->setFrame(j,_togo);
			if(!encoding_gui->isAlive())
			{
				error=1;
				continue;
			}
			if(!writeVideo(j)) error=1;
			if(!writeAudio(j)) error=1;
			
			

		}
		delete encoding_gui;
		encoding_gui=NULL;
		//________________ Flush______________________
		videoStream->flush();
		endAudio();
		//deleteAudioFilters();
		// ____________Close____________________
		fclose(_fd);
		_fd=NULL;

	return !error;
}
// Dummy ones
uint8_t	ADM_ogmWrite::initVideo(char *name)
{
		ADM_assert(0);
		return 0;

}
//___________________________________________________
uint8_t	ADM_ogmWrite::writeVideo(uint32_t frame)
{
		ADM_assert(0);
		return 0;

}
// EOF
