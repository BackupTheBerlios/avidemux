/***************************************************************************
    copyright            : (C) 2001/2005 by mean
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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dirent.h>

#include "default.h"
#include "ADM_assert.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit.hxx"

#define MAX_JOBS 120
static uint8_t buildJobList(uint32_t *outnb);
static char *jobName[MAX_JOBS];
extern uint8_t DIA_job(uint32_t nb,char **name);
uint8_t GUI_jobs(void)
{
uint32_t nb;
char     *name;
        memset(jobName,0,sizeof(jobName));

        if(!buildJobList(&nb))
        {
                GUI_Error_HIG("Oops","Something very wrong happened when buildint joblist");
                return 0;
        }
        if(!nb)
        {
                GUI_Info_HIG(ADM_LOG_IMPORTANT,"Nothing to do.","There is no jobs stored, nothing to do");
                return 1;
        }
        DIA_job(nb,jobName);
        for(int i=0;i<nb;i++)
        {
                //printf("List : %s\n",jobName[i]);
                ADM_dealloc(jobName[i]);
                jobName[i]=NULL;
        }
        return 1;
}
uint8_t buildJobList(uint32_t *outnb)
{
char *base=getBaseDir();
DIR *dir;
struct dirent *direntry;
int dirmax=0,len;
         dir=opendir(base);
        if(!dir)
        {
                return 0;
        }
        while((direntry=readdir(dir)))
        {
                len=strlen(direntry->d_name);
                if(len<4) continue;
                if(direntry->d_name[len-1]!='s' || direntry->d_name[len-2]!='j' || direntry->d_name[len-3]!='.')
                {
                        printf("ignored:%s\n",direntry->d_name);
                        continue;
                }
                jobName[dirmax]=(char *)ADM_alloc(strlen(base)+strlen(direntry->d_name)+2);
                strcpy(jobName[dirmax],base);
                strcat(jobName[dirmax],"/");
                strcat(jobName[dirmax],direntry->d_name);
                dirmax++;
                if(dirmax>=MAX_JOBS)
                {
                        printf("[jobs]: Max # of jobs exceeded\n");
                         break;
                }
        }
        closedir(dir);
        *outnb=dirmax;
        return 1;
}
//EOF
