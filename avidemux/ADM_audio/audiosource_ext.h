/***************************************************************************
                          audiosource_ext.h  -  description
                             -------------------
    begin                : Fri May 31 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr

    Basic class for handling external audio source (wav/mp3/ac3)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef ADMSOURCEX
 #define  ADMSOURCEX


class AVDMFileStream : public AVDMGenericAudioStream
{
protected:
				FILE *fd;	
				uint32_t _offset;
				uint32_t _totallen;
				uint32_t _audiolen;
				uint32_t	read32(void);
				void			abort(void);	
public:
								
        		AVDMFileStream(void);
	  			~AVDMFileStream();
				//~AVDMWavAudioStream() { abort();};
				virtual uint8_t open(char *name)=0;
        		virtual uint32_t read(uint32_t len,uint8_t *buffer);
        		virtual uint8_t  goTo(uint32_t newoffset);     		
};
#endif
