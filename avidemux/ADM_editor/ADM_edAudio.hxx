/***************************************************************************
                          ADM_edAudio.hxx  -  description
                             -------------------
    begin                : Wed Mar 13 2002
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
class AVDMEditAudioStream : public AVDMGenericAudioStream
{
protected:    	
				ADM_Composer *_father;	
				uint8_t _vbr;				
public:
								
        			AVDMEditAudioStream(ADM_Composer *father);
        virtual uint32_t 	read(uint32_t len,uint8_t *buffer);
        virtual uint8_t  	goTo(uint32_t newoffset);
	virtual uint8_t		buildAudioTimeLine( void );
	virtual uint8_t		isVBR(void )   { return _vbr;}
	virtual uint8_t 	goToTime(uint32_t mstime);
	virtual	uint8_t		getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
	virtual	uint8_t		flushPacket(void);
};
