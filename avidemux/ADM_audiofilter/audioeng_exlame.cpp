/***************************************************************************
                          audiodeng_exlamecpp  -  description
                             -------------------

	Codec build on lame exec

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
#include <assert.h>
#include <math.h>

#include "config.h"


#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx"
#include "audioeng_toolame.h"


uint8_t AVDMProcessAudio_ExLame::init(char *outfile,char *extra)
{
UNUSED_ARG(extra);
	char string[2048];
	// char m;
	float f;

	// first we check toolame exists...
	FILE *tlme;
	tlme=fopen(EXLAME,"rb");
	if(!tlme)
	{
		printf("\n Mmm no toolame found...\n");
		return 0;
	}
	fclose(tlme);

	f=_wavheader->frequency;
	f=f/1000.;

	
	{
			sprintf(string,"%s  %s  -  %s",
					EXLAME,
					extra,
					outfile
					);

	}
	printf("\n Invoking toolame with %s\n",string);
	_pipe=popen(string,"w");
	if(_pipe<=0)
	{
		printf("Error invoking lame\n");
		return 0;
	}
	fwrite(_wavheader,1,sizeof(*_wavheader),_pipe);
	return 1;
}





