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
#include <ADM_assert.h>
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
#include "ADM_filter/video_filters.h"

#include <prefs.h>

static FILTER_PARAM subParam={14,{"_fontsize","_subname","_fontname","_charset",
				"_baseLine","_Y_percent","_U_percent","_V_percent",
				"_selfAdjustable","_delay","_useBackgroundColor","_bg_Y_percent",
      				"_bg_U_percent","_bg_V_percent"}};

SCRIPT_CREATE(subtitle_script,ADMVideoSubtitle,subParam);

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
ADMVideoSubtitle::ADMVideoSubtitle(AVDMGenericVideoStream *in,CONFcouple *couples)
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
			GET(_delay);
			GET(_useBackgroundColor);
			GET(_bg_Y_percent);
      			GET(_bg_U_percent);
      			GET(_bg_V_percent);

			if(_conf->_baseLine>_info.height-_conf->_fontsize*SRT_MAX_LINE)
					_conf->_baseLine=_conf->_fontsize*SRT_MAX_LINE;

			char *ft,*sub;
			ft=(char *)ADM_alloc(500);
			sub=(char *)ADM_alloc(500);
			strcpy(ft,_conf->_subname); _conf->_subname=ft;
			strcpy(sub,_conf->_fontname); _conf->_fontname=sub;

			sub=(char *)ADM_alloc(500);
			strcpy(sub,_conf->_charset); _conf->_charset=sub;
			loadSubtitle();
			loadFont();

	}
	else
	{
			_conf=NEW( SUBCONF);

			_conf->_fontname=(char *)ADM_alloc(500);
			_conf->_subname=(char *)ADM_alloc(500);
			_conf->_charset=(char *)ADM_alloc(500);
			_conf->_fontname[0]=0;
			_conf->_subname[0]=0;
			_conf->_charset[0]=0;
			
			
			_conf->_baseLine=_info.height-24*SRT_MAX_LINE;
			_conf->_Y_percent=255;
			_conf->_U_percent=0;
			_conf->_V_percent=0;
                        _conf->_fontsize=24; 
			_conf->_selfAdjustable=0;
			_conf->_delay=0;
			_conf->_useBackgroundColor=0;
			
			_conf->_bg_Y_percent=0;
			_conf->_bg_U_percent=0;
			_conf->_bg_V_percent=0;


			prefs->get(FILTERS_SUBTITLE_FONTSIZE,&(_conf->_fontsize));
			prefs->get(FILTERS_SUBTITLE_YPERCENT,&(_conf->_Y_percent));
			prefs->get(FILTERS_SUBTITLE_UPERCENT,&(_conf->_U_percent));
			prefs->get(FILTERS_SUBTITLE_VPERCENT,&(_conf->_V_percent));
			prefs->get(FILTERS_SUBTITLE_SELFADJUSTABLE,&(_conf->_selfAdjustable));			
			prefs->get(FILTERS_SUBTITLE_USEBACKGROUNDCOLOR,&(_conf->_useBackgroundColor));
			
			// _conf->_fontname, ... are used as memory for a dialog
			// later. we can't used the length of the current string
			{ char *tmp;
			   prefs->get(FILTERS_SUBTITLE_FONTNAME,&tmp);
			   strcpy(_conf->_fontname,tmp);
			   free(tmp);
			   
			   prefs->get(FILTERS_SUBTITLE_CHARSET,&tmp);
			   strcpy(_conf->_charset,tmp); 
                           free(tmp);
			}
			
	}

  
  	_info.encoding=1;


	_bitmap=0xffffffff;
#define BITMAP_SIZE _info.width*_conf->_fontsize*SRT_MAX_LINE
	_bitmapBuffer=new uint8_t[_info.width*_info.height];
	_maskBuffer=new uint8_t[_info.width*_info.height];
  _bgBitmapBuffer=new uint8_t[(_info.width*_info.height)>>1];
	_bgMaskBuffer=new uint8_t[_info.width*_info.height];
	ADM_assert(_bitmapBuffer);
	ADM_assert(_maskBuffer);
	ADM_assert(_bgBitmapBuffer);
	ADM_assert(_bgMaskBuffer);
}
uint8_t	ADMVideoSubtitle::loadSubtitle( void )
{
unsigned char c,d;
			_fd=fopen(_conf->_subname,"rt");
			if(!_fd)
			{
				GUI_Alert("Could not open subtitle file");
				return 0;
			}
			// Try to detect utf16 files			
			c=fgetc(_fd);
			// Skip utf identifier if any
			if(c==0xff)
			{
				c=fgetc(_fd);
				if(c==0xfe)
				{
					c=fgetc(_fd);
				}
			}
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

  // Apply delay to subtitles
  
  aprintf("[debug] DELAY %d\n", _conf->_delay);
  if(_conf->_delay)
  {
  	int32_t newStartTime;
	int32_t newEndTime;
	for(uint32_t i=0;i<_line;i++) 
	{
		aprintf("[debug] BEFORE DELAY (%d) %d %d\n",i, _subs[i].startTime,_subs[i].endTime);
		newStartTime=_subs[i].startTime;
		newStartTime+=_conf->_delay;
		_subs[i].startTime=(newStartTime);
		newEndTime=_subs[i].endTime;
		newEndTime+=_conf->_delay;
		_subs[i].endTime=(newEndTime);
		// put them at infinit display time
		// should get rid of them
		if(newEndTime<0 || newStartTime<0)
		{
			_subs[i].startTime=_subs[i].endTime=0;
		}
		aprintf("[debug] AFTER DELAY (%d) %d %d\n",i, _subs[i].startTime,_subs[i].endTime);
  	}
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
			ADM_assert(_param);
			*couples=new CONFcouple(14);

			CSET(_fontsize);
			CSET(_subname);
			CSET(_fontname);
			CSET(_charset);
			CSET(_baseLine);
			CSET(_Y_percent);
			CSET(_U_percent);
			CSET(_V_percent);
			CSET(_selfAdjustable);
			CSET(_delay);
			CSET(_useBackgroundColor);
			CSET(_bg_Y_percent);
			CSET(_bg_U_percent);
			CSET(_bg_V_percent);

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

 		if(_bgBitmapBuffer) 
			{
				delete [] _bgBitmapBuffer;
				_bgBitmapBuffer=0;
			}
 
		if(_bgMaskBuffer) 
			{
				delete [] _bgMaskBuffer;
				_bgMaskBuffer=0;
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
					if(_subs[i].nbLine)
					{
					for(uint32_t k=0;k<_subs[i].nbLine;k++)
					{					
						if(_subs[i].string[k])
						{
							delete [] _subs[i].string[k];
							_subs[i].string[k]=NULL;
						}
					}					
					
					delete _subs[i].string;
					_subs[i].string=NULL;
					delete _subs[i].lineSize;
					_subs[i].lineSize=NULL;
					}
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

