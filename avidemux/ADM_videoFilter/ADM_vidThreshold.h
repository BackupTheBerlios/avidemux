/***************************************************************************
                          ADM_vidThreshold.h  -  do thresholding
                              -------------------
                          Chris MacGregor, 2005, 2007
                         chris-avidemux@bouncingdog.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef __THRESHOLD__
#define __THRESHOLD__   
#include "ADM_video/ADM_cache.h"
typedef struct THRESHOLD_PARAM
{

    uint32_t min;
    uint32_t max;
    uint32_t debug;

} THRESHOLD_PARAM;

class  ADMVideoThreshold:public AVDMGenericVideoStream
 {

 protected:
    	
     THRESHOLD_PARAM *  _param;

 public:
 		

     ADMVideoThreshold(  AVDMGenericVideoStream *in,CONFcouple *setup);

     ~ADMVideoThreshold();

     virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                              ADMImage *data,uint32_t *flags);

     virtual uint8_t 	configure( AVDMGenericVideoStream *instream);
     virtual char 		   *printConf(void);
     virtual uint8_t 	getCoupledConf( CONFcouple **couples);
							
 };
#endif
