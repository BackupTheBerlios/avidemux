/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ADM_ASS_H
#define _ADM_ASS_H

#ifdef USE_FREETYPE
#include "libass/ass.h"

typedef struct {
	float font_scale, line_spacing;
	int top_margin, bottom_margin;
	ADM_filename *subfile, *fonts_dir;
	int extract_embedded_fonts;
} ASSParams;

class ADMVideoSubASS : public AVDMGenericVideoStream {
protected:
	virtual char* printConf(void);
	ASSParams* _params;
	
	ass_track_t *_ass_t;
	ass_instance_t *_ass_i;
	
public:
	ADMVideoSubASS(AVDMGenericVideoStream *in, CONFcouple *conf);
	virtual ~ADMVideoSubASS();
	virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, ADMImage *data,uint32_t *flags);
	uint8_t configure(AVDMGenericVideoStream *instream);
	uint8_t	getCoupledConf(CONFcouple **conf);
};
#else

#endif /* USE_FREETYPE */
#endif
