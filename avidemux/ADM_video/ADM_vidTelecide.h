/***************************************************************************
                          ADM_vidTelecide.h  -  description
                             -------------------
    begin                : Sun Aug 25 2002
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
#include "ADM_video/ADM_vidField.h"
 
class  ADMVideoTelecide:public ADMVideoFields
 {

 protected:
    		uint32_t 									getMatch(void);
             	uint8_t 									interleave(	uint8_t *in,uint8_t odd)  ;
              	uint8_t									*_next;
              	uint32_t									_instock;
 public:
 		
  					
					ADMVideoTelecide(  AVDMGenericVideoStream *in,CONFcouple *setup);
  					virtual ~ADMVideoTelecide();
		        		virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags);
				   																	
					 virtual char 									*printConf(void);		
 }     ;


 
