/***************************************************************************
                          ADM_vidSalt.h  -  description
                             -------------------
    begin                : Wed Jan 1 2003
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
 /***************************************************************************
                          ADM_vidStabilize.h  -  description
                             -------------------
    begin                : Mon Oct 7 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/
#ifndef __SALT__
#define __SALT__   
#include "ADM_video/ADM_vidCached.h" 
 class  ADMVideoSalt:public ADMVideoCached
 {

 protected:
    		
        uint32_t						*_param;
        virtual char 					*printConf(void);
	       

 public:
 		
  					ADMVideoSalt(  AVDMGenericVideoStream *in,CONFcouple *setup);
  					 ~ADMVideoSalt();
		          virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									uint8_t *data,uint32_t *flags);
					virtual uint8_t	*getBinaryConf( uint16_t *size) { *size=sizeof(uint32_t);return (uint8_t *)_param;}
					virtual uint8_t 	configure( AVDMGenericVideoStream *instream);
					virtual uint8_t 	getCoupledConf( CONFcouple **couples);
 }     ;
#endif
