/***************************************************************************
                          ADM_vidload.cpp  -  description
                             -------------------

	Load the subtitles either in srt or sub format

	The structure is
		uint32_t 		startTime in ms from beginning
		uint32_t 		endTime  in ms from beginning
		char			*string      string, several lines are separated by |


    begin                : Thu Aug 09 2003
    copyright            : (C) 2003 by mean
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


uint8_t ADMVideoSubtitle::loadFont( void )
{
 if(!_font->initFreeType(_conf->_fontname))
	{
			printf("\n Free type init failed for font %s!",_conf->_fontname);
	}
	else
	{
		_font->fontSetSize(_conf->_fontsize);
		if(!_font->fontSetCharSet(_conf->_charset))
			{
				GUI_Alert("There was a problem with unicode/font\n continuing anyway...");
			}
	}
	return 1;
}
//
//	Load subtitles in .sub format i.e. {xx}{yy} |   |
//
// {3610}{3656}Pripravená uvíta» ich|v na¹om dome s primeranou úctou.
//__________________________________________________________________
uint8_t  ADMVideoSubtitle::loadSubTitle( void )
{
char string[500];
uint32_t current_line=0;
	// first cound how many line
	_line=0;
	while(fgets(string,200,_fd))
	{
		_line++;
	}
	printf("\n subs : %ld lines\n",_line);
	// rewind
	fseek(_fd,0,SEEK_SET);
	//
	_subs=new subLine[_line];
	if(!_subs) return 0;
	memset(_subs,0,sizeof(subLine)*_line);

	for(uint32_t i=0;i<_line;i++)
	{
		 fgets(string,200,_fd);
		 //printf("\n in : %s ",string);
		 if(string[0]!='{') continue;
		 subParse((subLine *)&(_subs[ current_line]),string);
		 current_line++;
	}
	// the effective number of line we have
	_line=current_line;
	return 1;
}
//
//	split the incoming string in start time/end time and string itself
//
uint8_t ADMVideoSubtitle::subParse( subLine *in, char *string )
{
	char *start,*end,*text;
	uint32_t j,textlen,totallen;
	float f;

		 j=1;
		 totallen=strlen(string);
		 assert(totallen);
		while(string[j]!='}' && j<totallen) j++;
		string[j]=0;
		start=&string[1];
		j+=2; // skip }{
		end=&string[j];
		while(string[j]!='}'&& j<totallen) j++;
		string[j]=0;

		text=&string[j+1];

		if(j==totallen-1)
		{
			printf("***ERR: Suspicious line !!!\n");
			return 0;

		}

		// convert frame -> time in ms
		 f=atoi(start);
		 f=f*1000000./_info.fps1000;
		in->startTime=(uint32_t)floor(f);

		 f=atoi(end);
		 f=f*1000000./_info.fps1000;
		 in->endTime=(uint32_t)floor(f);

		 textlen=strlen(text);
		 if(!textlen)
		 {
			in->string=new char[1];
 			strcpy(in->string,"");
		}
		else
		{


			end=text+textlen-1;
			while((*end==0x0a || *end==0x0d)&&end>text)
			{
				textlen--;
				*end=0;
				end--;
			}

		 in->string=new char[textlen+1];
		 strcpy(in->string,text);
		// we scrap \n\r at the end


		 }

	return 1;
}

//
//	Load subtitles in .srt format i.e. {xx}{yy} |   |
//__________________________________________________________________
uint8_t  ADMVideoSubtitle::loadSRT( void )
{
char string[500],text[500];
uint32_t line;
	// first cound how many line
	line=0;
	_line=0;
	while(fgets(string,300,_fd)) line++;
	printf("\n subs : %ld lines\n",line);
	// rewind
	fseek(_fd,0,SEEK_SET);
	//
	_subs=new subLine[line];

	if(!_subs) return 0;
	memset(_subs,0,sizeof(subLine)*line);
	// read and allocate
	//double f;
	uint32_t j;
	int state=0;

	for(uint32_t i=0;i<line;i++)
	{
//			printf(" %d - %d\n",i,line);
		 fgets(string,200,_fd);
//		 printf(" state : %d line : %s\n",state,string);
		 switch(state)
		 {
			 	case 0: // waiting for number

			      		j=atoi(string);
			          if(j==_line+1)
			          	{
									 	state=1;
									}
								break;
				case 1: // waiting for time
								uint32_t sh,sm,ss,ms;
								uint32_t dh,dm,ds,md;
								// 00:00:00,040 --> 00:00:00,120

			  				if(8!=sscanf(string,"%02lu:%02lu:%02lu,%03lu --> %02lu:%02lu:%02lu,%03lu",
			        						&sh,&sm,&ss,&ms,
			              						&dh,&dm,&ds,&md))
							{
			                 			sscanf(string,"%02lu:%02lu:%02lu.%03lu --> %02lu:%02lu:%02lu.%03lu",
			        						&sh,&sm,&ss,&ms,
			              						&dh,&dm,&ds,&md);
							}

			           _subs[_line].startTime=ms+1000*(ss+sm*60+sh*3600);
			           _subs[_line].endTime=md+1000*(ds+dm*60+dh*3600);
			         /*  printf(" start : %d end :%d \n",
							           _subs[_line].startTime,
							           _subs[_line].endTime);
				*/
			           state=2;
			           text[0]=0;
			           break;
			 case 2:// looking for text
	  //printf("[debug] State 2 line %d : %s\n",_line,string);
			 	       if(strlen(string)<3)
			         	{
										 _subs[_line].string=new char[strlen(text)+1];
										 memcpy(_subs[_line].string,text,strlen(text)+1);
              //printf("[debug] State 2 line %d added\n",_line);
										_line++;
										state=0;
										break;
								}
								else
								{
										uint32_t l=strlen(string)-1;

									while( string[l]==0xd || string[l]==0xa)
										 {
											 			string[l]=0;
											    	l--;
												};
              if(text[0]) {
                strcat(text,"|");
	      }
										strcat(text,string);
										if(strlen(text)>300)
										{
												assert(0);
										}
								}
			 
			 					break;
			 
			}
		
	}	

   if (line > 0 && text) {
    // Add last line
    //printf("[debug] Last line : %s\n",text);
    _subs[_line].string=new char[strlen(text)+1];
    memcpy(_subs[_line].string,text,strlen(text)+1);
    _line++;
  }


   // checks contents
  //  for(uint32_t i=0;i<line;i++) {
//      printf("[debug] (%d) At %d %d : %s\n",i, _subs[i].startTime,
// 	    _subs[i].endTime, _subs[i].string);
//    }



	return 1;
}
#endif
