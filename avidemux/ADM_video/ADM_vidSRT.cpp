/***************************************************************************
                          ADM_vidSRT.cpp  -  description
                             -------------------
    begin                : Thu Dec 12 2002
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
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <math.h>
#include <math.h>
#include <iconv.h>
#include "config.h"

#ifdef USE_FREETYPE

#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidFont.h"
#include "ADM_video/ADM_vidSRT.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"

static const char *FONTNAME="/usr/X11R6/lib/X11/fonts/truetype/arial.ttf";

BUILD_CREATE(subtitle_create,ADMVideoSubtitle);

//__________________________________________________________________

//_______________________________________________________________

//__________________________________________________________________

char 								*ADMVideoSubtitle::printConf(void)
 {
	  	static char buf[50];
 	
 				sprintf((char *)buf,"Subtitle ");
        return buf;
	}

//--------------------------------------------------------	
ADMVideoSubtitle::ADMVideoSubtitle(
									AVDMGenericVideoStream *in,CONFcouple *couples)
{

  uint32_t frame;
//  char c;

  _uncompressed=NULL;
  _font=NULL;

  _in=in;
  memcpy(&_info,_in->getInfo(),sizeof(_info));

	frame=3*_in->getInfo()->width*_in->getInfo()->height;

	_subs=NULL;
	_fd=NULL;
 	_line=0;
	_oldframe=0;
	_oldline=0;
	_font = new ADMfont();									
	if(couples)
	{
			SUBCONF *_param;
			_conf=NEW( SUBCONF);


			_param=_conf; // keep macro happy

			GET(_fontsize);
			GET(_subname);
			GET(_fontname);
			GET(_charset);
			GET(_baseLine);
			GET(_Y_percent);
			GET(_U_percent);
			GET(_V_percent);
			GET(_selfAdjustable);

			if(_conf->_baseLine>_info.height-_conf->_fontsize*SRT_MAX_LINE)
					_conf->_baseLine=_conf->_fontsize*SRT_MAX_LINE;

			char *ft,*sub;
			ft=(char *)malloc(500);
			sub=(char *)malloc(500);
			strcpy(ft,_conf->_subname); _conf->_subname=ft;
			strcpy(sub,_conf->_fontname); _conf->_fontname=sub;

			sub=(char *)malloc(500);
			strcpy(sub,_conf->_charset); _conf->_charset=sub;
			loadSubtitle();
			loadFont();

	}
	else
	{
			_conf=NEW( SUBCONF);
			_conf->_Y_percent=255;
			_conf->_U_percent=0;
			_conf->_V_percent=0;

			_conf->_fontsize=24;
			_conf->_fontname=(char *)malloc(500);
			_conf->_subname=(char *)malloc(500);
			_conf->_charset=(char *)malloc(500);
			_conf->_baseLine=_info.height-24*SRT_MAX_LINE;
			_conf->_selfAdjustable=1;
			strcpy(_conf->_fontname,FONTNAME);
			strcpy(_conf->_subname,"");
			strcpy(_conf->_charset,"UNICODE");
	}

  

  	_info.encoding=1;


	_bitmap=0xffffffff;
#define BITMAP_SIZE _info.width*_conf->_fontsize*SRT_MAX_LINE
	_bitmapBuffer=new uint8_t[_info.width*_info.height];
	_maskBuffer=new uint8_t[_info.width*_info.height];
	assert(_bitmapBuffer);
	assert(_maskBuffer);
}
uint8_t	ADMVideoSubtitle::loadSubtitle( void )
{
char c;
			_fd=fopen(_conf->_subname,"rt");
			if(!_fd)
			{
				GUI_Alert("Could not open subtitle file");
				return 0;
			}
			c=fgetc(_fd);
			fseek(_fd,0,SEEK_SET);

			switch(c)
			{
			case '{' :
						loadSubTitle();
						break;
			case '1' :
						loadSRT();
						break;
			default:
					GUI_Alert("\n cannot identify \nsubtitle format ");
			}
			fclose(_fd);
			_fd=NULL;
			_bitmap=0xffffffff;
			if(_line)	// that way we will have the first sub
				_oldline=_line-1;
			return 1;
}
uint8_t	ADMVideoSubtitle::getCoupledConf( CONFcouple **couples)
{
SUBCONF *_param;

			_param=_conf; // keep macro happy
			assert(_param);
			*couples=new CONFcouple(9);

			CSET(_fontsize);
			CSET(_subname);
			CSET(_fontname);
			CSET(_charset);
			CSET(_baseLine);
			CSET(_Y_percent);
			CSET(_U_percent);
			CSET(_V_percent);
			CSET(_selfAdjustable);

		return 1;

}
//__________________________________________________________________

ADMVideoSubtitle::~ADMVideoSubtitle()
{

		if(_bitmapBuffer)
			{
				delete [] _bitmapBuffer;
				_bitmapBuffer=0;
			}
		if(_maskBuffer)
			{
				delete [] _maskBuffer;
				_maskBuffer=0;
			}

		if(_fd)
		{
				fclose(_fd);
				_fd=NULL;
		}
		if(_subs)
		{
				for(uint32_t i=0;i<_line;i++)
				{
						if(_subs[i].string)
							delete [] _subs[i].string;
						_subs[i].string=NULL;
				}
				delete [] _subs;
				_subs=NULL;
		}
	if(_conf)
	{
			free(_conf->_subname);
			free(_conf->_fontname);
			free(_conf->_charset);

		 	DELETE(_conf);
	}
	if(_font)
	{
		delete _font;
		_font=NULL;
	}
}
#endif

