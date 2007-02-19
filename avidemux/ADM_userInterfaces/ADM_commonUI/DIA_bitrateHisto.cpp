/***************************************************************************
                          ADM_guiBitrate  -  description
                             -------------------

	Simple bitrate analyzer

    begin                : Mon Aug 31 2003
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "default.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "DIA_factory.h"
#include "ADM_assert.h"

/**
      \fn GUI_displayBitrate
      \brief display a bargraph with bitrates
*/

void GUI_displayBitrate( void )
{

  int32_t total,max;
 uint32_t len,idx,nb_frame,k,changed;
 uint32_t round,sum,average[60];;

 float f;

 float display[20];
uint32_t medium=0;

	// 1st compute the total
	
	max=0;
	if(!(frameEnd>frameStart) || abs(frameStart-frameEnd)<5)
	{
          GUI_Error_HIG(_("No data"), NULL);
		return ;
	}

	round=((avifileinfo->fps1000+990)/1000);
	if(!round) return;

	video_body->getFrameSize (frameStart, &len);
	for(k=0;k<round;k++) average[k]=0;

	sum=0;
	changed=0;
        total=0;
	// 1 st pass, compute max
	for( k=frameStart;k<frameEnd;k++)
	{
 		video_body->getFrameSize (k, &len);
                total+=len;
		sum-=average[changed];
		average[changed]=len;
		sum+=average[changed];
		//printf("Frame %lu Br: %lu\n",k,(sum*8)/1000);
		if(sum>max) max=sum;
		changed++;
		changed%=round;
	}
        
        float g=total;
        g/=(frameEnd-frameStart);
        g*=avifileinfo->fps1000;
        g/=(1000.*1000.);
        medium=(uint32_t)(g*8);
        printf("Average :%u max%u\n",medium,max);
#if 0
#define ROUNDUP 20*1000
        uint32_t s;
	s=(max+ROUNDUP -1)/ROUNDUP;
	max=s*ROUNDUP;
#endif

	printf("\n Total : %lu bytes, max br %d bytes round %d\n",total,max,round);
	if(!max)
	{
          GUI_Error_HIG(_("No data"), NULL);
		return ;
	}
	nb_frame=frameEnd-frameStart;
	// and now build an histogram
	for(k=0;k<20;k++) display[k]=0;

	video_body->getFrameSize (frameStart, &len);
	for(k=0;k<round;k++) average[k]=0;

	sum=0;
	changed=0;
        uint32_t step=(max*8)/20000;

	for(uint32_t k=frameStart;k<frameEnd;k++)
	{
		video_body->getFrameSize (k, &len);
		// which case ?
                if(sum>average[changed])
		  sum-=average[changed];
                else
                   sum=0;
		average[changed]=len;
		sum+=average[changed];
		changed++;
		changed%=round;

		f=sum;
		f=f/max;
		f=f*20.;
		idx=(uint32_t )floor(f+0.5);
	//	printf("idx : %d\n",idx);
		if(idx>19) idx=19;
		display[idx]++;
	}


	for(k=0;k<20;k++) display[k]=(100*display[k])/nb_frame;
	for(k=0;k<20;k++)
	{
		printf(" %d , %04d -- %04d percent -> %02.1f\n",k,(max*k*8)/20000,(8*max*(k+1))/20000,display[k]);

	}
        
        diaElemBar *bar[20];
        char str[256];
        for(int i=0;i<20;i++)
        {
          sprintf(str,"%03u--%03u",step*i,step*(i+1));
          bar[19-i]=new  diaElemBar((uint32_t)display[i],str);
            
        }
        
        diaElemUInteger med(&medium,"Average bitrate",0,9999999);
        diaElemBar foo(0,"foo");
#define P(X) bar[X] 
  
      diaElem *elems[21]={
            &med,
            P(0),P(1),P(2),P(3),P(4),P(5),P(6),P(7),P(8),P(9),
            P(10),P(11),P(12),P(13),P(14),P(15),P(16),P(17),P(18),P(19)
          };
    
        diaFactoryRun(_("Bitrate Histogram"),21,elems);
        
        
        for(int i=0;i<20;i++)
        {
          delete bar[i];
        }
}

