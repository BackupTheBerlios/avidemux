/***************************************************************************
                          ADM_AC3.h  -  description
                             -------------------
    begin                : Sat May 25 2002
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
#ifndef          HAVE_LIBPTHREAD
          #error config.h not included
#endif
#ifdef USE_AC3

uint8_t 	    ADM_AC3Init(void );
uint8_t 	    ADM_AC3End(void );
uint8_t 		ADM_AC3Run(uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut);
uint32_t 		AC3_Resync(void);

#endif



