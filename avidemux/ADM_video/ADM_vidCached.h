/***************************************************************************
                          ADM_vidCached.h  -  description
                             -------------------
    begin                : Tue Dec 31 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This class is to easy handle filters that need previous and next
    image (temporal smoother / stabilize / ...)
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __VCACHE__
#define __VCACHE__ 
class  ADMVideoCached:public AVDMGenericVideoStream
 {

 protected:

				uint8_t									*_buffer[3];
				uint32_t								_bufnum[3];       	    	
	     	int32_t									index_n,index_p,index_c;
	     
												 
				int32_t searchIndex(uint32_t frame);												 
        uint8_t fillCache(uint32_t frame);
 public:
 		
  					ADMVideoCached(  AVDMGenericVideoStream *in,CONFcouple *setup);
  					 ~ADMVideoCached();
		        virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          									uint8_t *data,uint32_t *flags);

			virtual uint8_t configure( AVDMGenericVideoStream *instream);
     			virtual char    *printConf(void);

 }     ;
#endif
