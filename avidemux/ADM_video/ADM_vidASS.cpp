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
#include "ADM_assert.h"

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"
#ifdef HAVE_ENCODER

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"
#include "ADM_video/ADM_vidASS.h"
#include "ADM_filter/video_filters.h"
#include "ADM_colorspace/ADM_rgb.h"

#ifdef USE_FREETYPE

#ifndef DIR_SEP
# ifdef WIN32
#   define DIR_SEP '\\'
# else
#   define DIR_SEP '/'
# endif
#endif

static FILTER_PARAM assParam={7,{/* float */ "font_scale",
	/*float*/ "line_spacing",
	/* int */ "top_margin",
	/* int */ "bottom_margin",
	/* bool */ "extract_embedded_fonts",
	/* ADM_filename */ "fonts_dir",
	/* ADM_filename */ "subfile" }};

SCRIPT_CREATE(ass_script,ADMVideoSubASS,assParam);
BUILD_CREATE(ass_create,ADMVideoSubASS);

char *ADMVideoSubASS::printConf() {
 	static char buf[50];
	sprintf((char *)buf," ASS Subtitles: ");
	
	char *filename = (char*)_params->subfile;
	if(strrchr(filename, DIR_SEP) != NULL && *(strrchr(filename, DIR_SEP) + 1) != 0)
		filename = strrchr(filename, DIR_SEP) + 1;
	strncpy(buf+16, filename, 50-16);
	buf[49] = 0;
 	
	return buf;
}

//_______________________________________________________________

ADMVideoSubASS::ADMVideoSubASS(AVDMGenericVideoStream *in, CONFcouple *conf) {
  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_params = new ASSParams;
	ADM_assert(_params);
	
	_ass_i = NULL;
	_ass_t = NULL;
	
	if(conf) {
		#define _COUPLE_GET(x) conf->getCouple(#x, &(_params->x));
		_COUPLE_GET(font_scale)
		_COUPLE_GET(line_spacing)
		_COUPLE_GET(top_margin)
		_COUPLE_GET(bottom_margin)
		_COUPLE_GET(subfile)
		_COUPLE_GET(fonts_dir)
		_COUPLE_GET(extract_embedded_fonts)
	}	
	else {
		_params->font_scale = 1.;
		_params->line_spacing = _params->top_margin = _params->bottom_margin = 0;
		_params->subfile = (ADM_filename*)ADM_alloc(sizeof(ADM_filename));
		_params->fonts_dir = (ADM_filename*)ADM_alloc(6*sizeof(ADM_filename));
		_params->subfile[0] = 0;
		strcpy((char*)_params->fonts_dir, "/tmp/");
		_params->extract_embedded_fonts = 1;
		
		#if 0
		ADM_dealloc(_params->subfile);
		_params->subfile = (ADM_filename*)ADM_alloc(sizeof(ADM_filename)*21);
		strcpy((char*)_params->subfile, "/home/moonz/test.ass");
		#endif
	}
	
	_uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
  	ADM_assert(_uncompressed);
  	_info.encoding=1;
	
	/* ASS initialization */
	if(*_params->subfile != 0) {
		ass_settings_t settings;
		
		_ass_i = ass_init();
		_ass_t = ass_read_file((char*)_params->subfile);
		
		ADM_assert(_ass_i);
		ADM_assert(_ass_t);
		
		_info.height += _params->top_margin + _params->bottom_margin;
		settings.frame_width = _info.width;
		settings.frame_height = _info.height;
		settings.font_size_coeff = _params->font_scale;
		settings.line_spacing = _params->line_spacing;
		settings.top_margin = _params->top_margin;
		settings.bottom_margin = _params->bottom_margin;
		settings.aspect = ((double)_info.width) / ((double)_info.height);
		
		ass_configure(_ass_i, &settings);
	}
}

ADMVideoSubASS::~ADMVideoSubASS() {
 	if(_uncompressed) DELETE(_uncompressed);
	
	if(_params) {
		ADM_dealloc(_params->subfile);
		ADM_dealloc(_params->fonts_dir);
		DELETE(_params);
	}
}

#define _r(c)  ((c)>>24)
#define _g(c)  (((c)>>16)&0xFF)
#define _b(c)  (((c)>>8)&0xFF)
#define _a(c)  ((c)&0xFF)
#define rgba2y(c)  ( (( 263*_r(c)  + 516*_g(c) + 100*_b(c)) >> 10) + 16  )
#define rgba2u(c)  ( (( 450*_r(c) - 376*_g(c) -  73*_b(c)) >> 10) + 128 )
#define rgba2v(c)  ( ((-152*_r(c) - 298*_g(c) + 450*_b(c)) >> 10) + 128 )

uint8_t ADMVideoSubASS::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, ADMImage *data, uint32_t *flags) {
	unsigned i, j, k, l, val;
	uint8_t y, u, v, opacity, orig_u, orig_v;
	uint8_t orig_y;
	uint8_t *bitmap, *ydata, *udata, *vdata;
	
	ADM_assert(frame<_info.nb_frames);
	ADM_assert(_params);
	ADM_assert(_ass_i);
	ADM_assert(_ass_t);
	
	long long where = (long long)(_info.orgFrame + frame) * 1000000LL / (long long)_info.fps1000;
	
	if(!_in->getFrameNumberNoAlloc(frame, len, _uncompressed, flags))
		return 0;
	
	/* Add black borders */
	memset(YPLANE(data),16,_info.width*_info.height);
	memset(UPLANE(data),128,(_info.width*_info.height)>>2);
	memset(VPLANE(data),128,(_info.width*_info.height)>>2);
	
	memcpy(YPLANE(data) + _info.width * _params->top_margin, YPLANE(_uncompressed),
		_in->getInfo()->width * _in->getInfo()->height);
	memcpy(UPLANE(data) + (_info.width>>1) * (_params->top_margin>>1), UPLANE(_uncompressed),
		(_in->getInfo()->width>>1) * (_in->getInfo()->height>>1));
	memcpy(VPLANE(data) + (_info.width>>1) * (_params->top_margin>>1), VPLANE(_uncompressed),
		(_in->getInfo()->width>>1) * (_in->getInfo()->height>>1));
	
	ass_image_t *img = ass_render_frame(_ass_i, _ass_t, where);
	
	while(img) {
		y = rgba2y(img->color);
		u = rgba2u(img->color);
		v = rgba2v(img->color);

		opacity = 255 - _a(img->color);
		
		bitmap = img->bitmap;
		ydata = YPLANE(data) + img->dst_y * _info.width + img->dst_x;
		udata = UPLANE(data) + (img->dst_y >> 1) * (_info.width >> 1) + (img->dst_x >> 1);
		vdata = VPLANE(data) + (img->dst_y >> 1) * (_info.width >> 1) + (img->dst_x >> 1);
		
		for(i = 0; i < img->h; ++i) {
			for(j = 0; j < img->w; ++j) {
				k = *(bitmap+j) * opacity / 255;
				orig_y = *(ydata+j);
				*(ydata+j) = (k*y + (255-k)*orig_y) / 255;
			}
			
			bitmap += img->stride;
			ydata += _info.width;
		}
		
		bitmap = img->bitmap;
		
		for(i = 0; i < img->h; i += 2) {
			for(j = 0, l = 0; j < img->w; j += 2, ++l) {
				val = 0;
				val += *(bitmap + j);
				val += *(bitmap + j + 1);
				val += *(bitmap + img->stride + j);
				val += *(bitmap + img->stride + j + 1);
				val >>= 2;
				
				k = val * opacity / 255;
				orig_u = *(udata+l);
				orig_v = *(vdata+l);
				
				*(udata+l) = (k*u + (255-k)*orig_u) / 255;
				*(vdata+l) = (k*v + (255-k)*orig_v) / 255;
			}
			
			bitmap += img->stride << 1;
			udata += _info.width >> 1;
			vdata += _info.width >> 1;
		}
		
		img = img->next;
	}
	
	return 1;
}

uint8_t	ADMVideoSubASS::getCoupledConf(CONFcouple **conf) {
	*conf=new CONFcouple(7);
	
#define _COUPLE_SET(x) (*conf)->setCouple(#x, _params->x);
	_COUPLE_SET(font_scale)
	_COUPLE_SET(line_spacing)
	_COUPLE_SET(top_margin)
	_COUPLE_SET(bottom_margin)
	_COUPLE_SET(subfile)
	_COUPLE_SET(fonts_dir)
	_COUPLE_SET(extract_embedded_fonts)
	
	return 1;
}
#endif /* HAVE_ENCODER */

#endif /* USE_FREETYPE */
