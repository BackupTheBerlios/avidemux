/***************************************************************************
                          ADM_pics.h  -  description
                             -------------------
    begin                : Mon Jun 3 2002
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



#ifndef __PICHEADER__
#define __PICHEADER__
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

enum
{
	PIC_BMP=1,
	PIC_JPEG=2,
	PIC_BMP2=3,
	PIC_PNG=4
};

class picHeader         :public vidHeader
{
protected:
					uint32_t 		_nb_file;
       					uint32_t 		_first,_offset,_w,_h;

            				FILE 		**_fd;
					uint32_t		*_imgSize;

					uint32_t 		_type;

					uint32_t    read32(FILE *fd);
					uint16_t    read16(FILE *fd);
					uint8_t     read8(FILE *fd);

public:
//  static int checkFourCC(uint8_t *in, uint8_t *fourcc);

virtual   void 				Dump(void) {};
virtual   uint32_t 			getNbStream(void) { return 1;};
virtual   uint8_t 			needDecompress(void) { return 1;};

			picHeader( void ) {_nb_file=0;_fd=NULL;_imgSize=NULL;};
       		    ~picHeader(  ) { };
// AVI io
virtual 	uint8_t			open(char *name);
virtual 	uint8_t			close(void) ;
  //__________________________
  //				 Info
  //__________________________

  //__________________________
  //				 Audio
  //__________________________

virtual 	WAVHeader *getAudioInfo(void ) { return NULL ;} ;
virtual 	uint8_t			getAudioStream(AVDMGenericAudioStream **audio)
										{  *audio=NULL;return 0;};

// Frames
  //__________________________
  //				 video
  //__________________________

virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags);
virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags);
virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,ADMCompressedImage *);

};


#endif


