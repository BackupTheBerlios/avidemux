/***************************************************************************
                          ADM_vidChangeFPS.h  -  description
                             -------------------
    begin                : Sat Jan 22 2005
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _CHANGEFPS_H
#define _CHANGEFPS_H

class  ADMVideoChangeFPS:public AVDMGenericVideoStream {
public:
	ADMVideoChangeFPS(AVDMGenericVideoStream* in, CONFcouple* couples);
	virtual ~ADMVideoChangeFPS();
	virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t* len, ADMImage* data, uint32_t* flags);
	virtual uint8_t configure(AVDMGenericVideoStream* instream);
	virtual uint8_t getCoupledConf(CONFcouple **couples)		;

protected:
	AVDMGenericVideoStream* _in;
	virtual char* printConf();
	virtual void buildFramesArray();
	
	CHFPS_PARAMS *_param;
	uint32_t* frames_array;
};
#endif
