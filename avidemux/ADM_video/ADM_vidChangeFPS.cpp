/***************************************************************************
                          ADM_vidChangeFPS.h  -  description
                             -------------------
    begin                : Sat Jan 22 2005
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
#include <math.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_enter.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidChangeFPS.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM chfpsParam={2,{"newfps","precision"}};

SCRIPT_CREATE(chfps_script,ADMVideoChangeFPS,chfpsParam);
BUILD_CREATE(chfps_create,ADMVideoChangeFPS);

void dump_frames_array(uint32_t* frames_array) {
	printf("dump_frames_array: %d frames\n", frames_array[0]);
	for(uint32_t i=0; i<frames_array[0];i++) {
		//printf("dump_frames_array: frame %d correspond to old frame %d\n", i, frames_array[i+1]);
	}
}

ADMVideoChangeFPS::ADMVideoChangeFPS(AVDMGenericVideoStream* in, CONFcouple* couples) {
	frames_array = NULL;
	
	_in = in;
	memcpy(&_info, _in->getInfo(), sizeof(_info));
	
	if(couples) {
		printf("ChangeFPS (Construct): have new config, rebuilding frames_array\n");
		
		_param=NEW(CHFPS_PARAMS);
		
		GET(newfps);
		GET(precision);
		
		buildFramesArray();
		
		_info.fps1000  = _param->newfps;
		_info.nb_frames = frames_array[0];
		
		dump_frames_array(frames_array);
	}
	else {
		printf("ChangeFPS (Construct): no couples, building default frames_array\n");
		
		_param=NEW(CHFPS_PARAMS);
		
		_param->newfps    = _in->getInfo()->fps1000;
		_param->precision = 1000;
		
		frames_array = new uint32_t[_in->getInfo()->nb_frames+1];
		
		uint32_t i;
		
		for(i=0; i < _in->getInfo()->nb_frames; i++) {
			frames_array[i+1] = i;
		}
		
		frames_array[0] = i;
		
		dump_frames_array(frames_array);
	}
	
	_info.encoding = 1;
	_uncompressed  = new ADMImage(_in->getInfo()->width, _in->getInfo()->height);
	
	ADM_assert(_uncompressed);    	  	
}

ADMVideoChangeFPS::~ADMVideoChangeFPS() {
	delete _uncompressed;
	DELETE(_param);
	_uncompressed = NULL;
	if(frames_array != NULL) {
		delete [] frames_array;
                frames_array=NULL;
	}
}

uint8_t ADMVideoChangeFPS::getFrameNumberNoAlloc(uint32_t frame, uint32_t* len, ADMImage* data, uint32_t* flags) 
{
	if(frame>=_info.nb_frames) return 0;
	//printf("ChangeFPS (getFrame): %d->%d\n", frame, frames_array[frame+1]);
	return _in->getFrameNumberNoAlloc(frames_array[frame+1],len,data,flags);
}

uint8_t ADMVideoChangeFPS::configure(AVDMGenericVideoStream* instream) {
	UNUSED_ARG(instream);
	
	int prec = (int)log10((double)_param->precision);
	
	float newfps=(float)_param->newfps/1000.;
	
	memcpy(&_info,_in->getInfo(),sizeof(_info));
	
	GUI_getIntegerValue(&prec, 1, 8, "Precision");
	DIA_GetFloatValue(&newfps, 1., (float)_info.fps1000/500., "New FPS", "New FPS");
	
	_param->newfps    =(uint32_t)floor( (float)newfps*1000.); 
	_param->precision = (uint32_t)floor(pow(10, prec));
	
	printf("ChangeFPS (configure): have new config, rebuilding frames_array\n");
	
	buildFramesArray();
	
	_info.fps1000  = _param->newfps;
	_info.nb_frames = frames_array[0];
	
	dump_frames_array(frames_array);
	
	return 1;
}

uint8_t	ADMVideoChangeFPS::getCoupledConf( CONFcouple **couples) {
	ADM_assert(_param);
	*couples = new CONFcouple(2);
	
	CSET(newfps);
	CSET(precision);
	
	return 1;
}

char* ADMVideoChangeFPS::printConf() {
 	static char buf[50];
 	sprintf(buf," Change FPS %ffps->%ffps", (float)_in->getInfo()->fps1000/1000., (float)_param->newfps/1000.);
        return buf;
}

void ADMVideoChangeFPS::buildFramesArray() {
	uint32_t i, cur_frame, new_frames, ratio, d_fps;
	uint32_t old_fps, new_fps, precision, frames;
	bool change_frame;
	
	if(frames_array != NULL) {
		delete [] frames_array;
	}
	
	frames    = _in->getInfo()->nb_frames;
	old_fps   = _in->getInfo()->fps1000;
	new_fps   = _param->newfps;
	precision = _param->precision;
	
	printf("ChangeFPS (buildFramesArray): olp fps=%d, new fps=%d, frames=%d, precision=%d\n", old_fps, new_fps, frames, precision);
	
	if(new_fps < old_fps) {
		frames_array = new uint32_t[frames+1];
	}
	else {
		frames_array = new uint32_t[frames*2+1];
	}
	
	new_frames = 0;
	d_fps      = (new_fps > old_fps) ? new_fps - old_fps: old_fps - new_fps;
	ratio      = (uint32_t)(double)((double)old_fps*(double)precision/(double)d_fps);
	
	change_frame = false;
	
	for(cur_frame=0; cur_frame < frames; cur_frame++) {
		for(i=cur_frame*precision; i < (cur_frame+1)*precision; i++) {
			if((i%ratio) == 0) {
				change_frame = true;
				break;
			}
		}
		
		if(new_fps < old_fps) {
			if(!change_frame) {
				frames_array[++new_frames] = cur_frame;
			}
		}
		else {
			frames_array[++new_frames] = cur_frame;
			if(change_frame) {
				frames_array[++new_frames] = cur_frame;
			}
		}
		
		change_frame = false;
	}
	
	frames_array[0] = new_frames;
}
#endif
