/***************************************************************************
                          audiodeng_toolamecpp  -  description
                             -------------------

	Codec build on toolame exec

    begin                : Mon Apr 24 2003
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
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"


#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx"
#include "audioeng_toolame.h"
#include "prefs.h"
#include "ADM_toolkit/toolkit.hxx"
// Ctor: Duplicate
//__________

AVDMProcessAudio_Piper::AVDMProcessAudio_Piper(AVDMGenericAudioStream * instream):
	AVDMProcessAudioStream(instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_MP2;
    strcpy(_name, "PROC:TOOLAME");
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
    _pipe=NULL;
    

};

uint8_t AVDMProcessAudio_Piper::init(char *outfile,
					uint32_t mode, uint32_t bitrate,char *extra)
{
	UNUSED_ARG(extra);
	UNUSED_ARG(outfile);
	UNUSED_ARG(mode);
	UNUSED_ARG(bitrate);
	ADM_assert(0);
	return 0;

}

uint8_t AVDMProcessAudio_TooLame::init(char *outfile,
					uint32_t mode, uint32_t bitrate,char *extra)
{

	char string[2048];
	char *toolamepath;
	char m;
	float f;
	UNUSED_ARG(extra);
	// first we check toolame exists...
	FILE *tlme;
	
	if(!prefs->get(TOOLAME_PATH, &toolamepath))
	{
		GUI_Alert("Toolame not defined in path!");
		return 0;
	}
	
	tlme=fopen(toolamepath,"rb");
	if(!tlme)
	{
		printf("\n Mmm no toolame found (%s)...\n",toolamepath);
		return 0;
	}
	fclose(tlme);


	 switch (mode)
	    {
	    case ADM_STEREO:
		m='s';
		break;
	    case ADM_JSTEREO:
		m='j';
		break;
	    default:
		printf("\n **** unknown mode ***\n");
		m='s';;
		break;

	    }

	   	if(_wavheader->channels==1) m='m';
	f=_wavheader->frequency;
	f=f/1000.;

	// to avoid conflict with locals we do it our way
	char st[100];
	uint32_t d,n;
		n=(uint32_t)floor(f);
		f=f-n;
		f=f*1000;
		d=(uint32_t)ceil(f);

		sprintf(st,"%d.%03d",n,d);

	sprintf(string,"%s -s %s -m %c -e -b %lu /dev/stdin %s",
					toolamepath,
					st,
					m,
					bitrate,
					outfile
					);
	printf("\n Invoking toolame with %s\n",string);
	_pipe=popen(string,"w");
	if(_pipe<=0)
	{
		printf("Error invoking toolame\n");
		return 0;
	}
	return 1;
}


AVDMProcessAudio_Piper::~AVDMProcessAudio_Piper()
{
    delete(_wavheader);
    if(_pipe)
    	{
		pclose(_pipe);
		_pipe=NULL;
	}
};

uint32_t AVDMProcessAudio_Piper::read(uint32_t len,uint8_t *buffer)
{
UNUSED_ARG(len);
UNUSED_ARG(buffer);
return 0;
};
uint32_t AVDMProcessAudio_Piper::readDecompress(uint32_t len,uint8_t *buffer)
{
UNUSED_ARG(len);
UNUSED_ARG(buffer);
return 0;
};

//_____________________________________________
uint8_t  AVDMProcessAudio_Piper::push( uint32_t *eaten )
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in;
    uint32_t asked;
    int r=0;
	*eaten=0;

#define ONE_CHUNK (8192)
//
// Read n samples
    // Go to the beginning of current block
    in = _bufferin;
    ADM_assert(_pipe);
    while (rdall < ONE_CHUNK)
      {
	  // don't ask too much front.
	  asked = ONE_CHUNK - rdall;
	  rd = _instream->read(asked, in + rdall);
	  rdall += rd;
	  if (rd == 0)
	      break;
      }
    // Block not filled
    if (rdall != ONE_CHUNK)
      {
	  printf("\n not enough...%lu\n", rdall);
	  if (rdall == 0)
	      return 0;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
	  memset(in + rdall, 0, ONE_CHUNK - rdall);
      }
    // input buffer is full , convert it
	*eaten=ONE_CHUNK;
	r=fwrite(_bufferin,1,ONE_CHUNK,_pipe);
	if(r!=8192)
	{
		printf("err: %d\n",r);
		return 0;
	}
	return 1;
}



