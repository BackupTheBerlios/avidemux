/***************************************************************************
                          audiodeng_appcpp  -  description
                             -------------------

	pipe to an external apps

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


uint8_t AVDMProcessAudio_External::init(char *app,char *cmd)
{

	char string[2048];

	if(!app) return 0;
	if(!cmd) return 0;

	// first we check toolame exists...
	FILE *tlme;
	tlme=fopen(app,"rb");
	if(!tlme)
	{
		printf("\n Mmm no %s  found...\n",app);
		return 0;
	}
	fclose(tlme);


	sprintf(string,"%s  %s ",
					app,
					cmd
						);

	printf("\n Invoking  with %s\n",string);
	_pipe=popen(string,"w");
	if(_pipe<=0)
	{
		printf("Error invoking lame\n");
		return 0;
	}
	return 1;
}






