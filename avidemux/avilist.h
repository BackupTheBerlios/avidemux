/***************************************************************************
                          avilist.h  -  description
                             -------------------
    begin                : Wed Feb 20 2002
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

 /***************************************************************************
                          avilist.h  -  description
                             -------------------
    begin                : Thu Nov 15 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr

Deals with LIST in RIFF structured file
Especially AVI in our case
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __AVILIST__
#define __AVILIST__
class AviList
{
protected:
		FILE 		*_ff;
		uint32_t _fcc,_begin_tell;
		fpos_t _begin,_end;

public:


uint8_t Write32(uint32_t val) ;
uint8_t Write32(uint8_t *c);
uint8_t Write32(const char  *c) {Write32((uint8_t *)c);return 1;};
uint8_t Write(uint8_t *p,uint32_t len);
uint8_t WriteChunk(uint8_t *chunkid,uint32_t len,uint8_t *p);


					AviList(const char *name,FILE *ff);

uint32_t  Tell(void );    		// glue for index.... ugly
uint32_t  TellBegin(void );   // same story here

uint8_t		Begin( const char *subchunk );
uint8_t		End (void );

};

#endif
