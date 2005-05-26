/***************************************************************************
                          Base class for Mpeg Demuxer
                             -------------------
                
    copyright            : (C) 2005 by mean
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

 #ifndef DMX_INDX
 #define DMX_INDX
 
uint8_t dmx_indexer(char *mpeg,char *file,uint16_t videoPid,uint16_t videoPesPid,
                                        uint16_t audioPid, uint16_t audioPesId);
        

#endif
