/***************************************************************************
                          ADM_genvideo.cpp  -  description
                             -------------------
    begin                : Sun Apr 14 2002
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

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "config.h"

#include "fourcc.h"
#include "avio.hxx"

#include "ADM_video/ADM_confCouple.h"
static char tmpstring[1024]; // should be enougth
CONFcouple::CONFcouple(uint32_t nub)
{
	nb=nub;
	name=new char *[nb];
	value=new char *[nb];

	for(uint32_t i=0;i<nb;i++)
		{
			name[i]=NULL;
			value[i]=NULL;
		}

	cur=0;
};

CONFcouple::~CONFcouple()
{
	for(uint32_t i=0;i<nb;i++)
		{
			if(name[i]) delete [] name[i];
			if(value[i]) delete [] value[i];
		}
		delete [] name;
		delete [] value;


};

uint8_t CONFcouple::setCouple(char *myname,uint32_t val)
{
	assert(cur<nb);

	name[cur]=strdup(myname);
	sprintf(tmpstring,"%lu",val);
	value[cur]=strdup(tmpstring);
	cur++;
	return 1;
}
uint8_t CONFcouple::setCouple(char *myname,float val)
{
	assert(cur<nb);

	name[cur]=strdup(myname);
	sprintf(tmpstring,"%f",val);
	value[cur]=strdup(tmpstring);
	cur++;
	return 1;
}
uint8_t CONFcouple::setCouple(char *myname,double val)
{
	assert(cur<nb);

	name[cur]=strdup(myname);
	sprintf(tmpstring,"%f",val);
	value[cur]=strdup(tmpstring);
	cur++;
	return 1;
}
uint8_t CONFcouple::setCouple(char *myname,int32_t val)
{
	assert(cur<nb);

	name[cur]=strdup(myname);
	sprintf(tmpstring,"%ld",val);
	value[cur]=strdup(tmpstring);
	cur++;
	return 1;
}
uint8_t CONFcouple::setCouple(char *myname,char *val)
{
	assert(cur<nb);

	name[cur]=strdup(myname);
	value[cur]=strdup(val);
	cur++;
	return 1;
}


uint8_t CONFcouple::getCouple(char *myname,uint32_t *val)
{
	int32_t index=lookupName(myname);

	assert(index!=-1);
	assert(index<(int)nb);
	*val=(int)atoi(value[index]);
	return 1;
}
uint8_t CONFcouple::getCouple(char *myname,int32_t *val)
{
	int32_t index=lookupName(myname);

	assert(index!=-1);
	assert(index<(int)nb);
	*val=(int)atoi(value[index]);
	return 1;
}
uint8_t CONFcouple::getCouple(char *myname,char **val)
{
	int32_t index=lookupName(myname);

	assert(index!=-1);
	assert(index<(int)nb);
	*val=value[index];
	return 1;
}
uint8_t CONFcouple::getCouple(char *myname,float *val)
{
int32_t index=lookupName(myname);

	assert(index!=-1);
	assert(index<(int)nb);
	sscanf(value[index],"%f",val);;
	return 1;
}
uint8_t CONFcouple::getCouple(char *myname,double *val)
{
	int32_t index=lookupName(myname);

	assert(index!=-1);
	assert(index<(int)nb);
	sscanf(value[index],"%lf",val);;
	return 1;
}

int32_t CONFcouple::lookupName(char *myname)
{
	for(uint32_t i=0;i<nb;i++)
	{
		if(!strcmp(name[i],myname)) return i;

	}
	return -1;

}
void CONFcouple::dump(void )
{
	for(uint32_t i=0;i<nb;i++)
	{
		if(name[i]) printf ("nm:%s ",name[i]); else printf("!! no name !! ");
		if(value[i]) printf ("val:%s ",value[i]); else printf("!! no value !! ");
	}
}

