/***************************************************************************
                          ADM_Atom  -  description
                             -------------------

	Helper class to deal with atom

    begin                : Mon Jul 21 2003
    copyright            : (C) 2001 by mean
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


#include <stdio.h>
#include <stdlib.h>
#include <ADM_assert.h>
#include <string.h>
#include "config.h"
#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_3gp/ADM_atom.h"

#define aprintf printf
//#define ATOM_DEBUG
adm_atom::adm_atom(adm_atom *atom)
{
	_fd=atom->_fd;
	_atomStart=ftell(_fd);
	_atomSize=read32();
	_atomFCC=read32();
#ifdef ATOM_DEBUG
	dumpAtom();
#endif

}
adm_atom::adm_atom(FILE *fd )
{

	_fd=fd;
	fseek(_fd,0,SEEK_END);
	_atomFCC=fourCC::get((uint8_t *)"MOVI");
	_atomSize=ftell(_fd);

	fseek(_fd,0,SEEK_SET);
	_atomStart=0;
#ifdef ATOM_DEBUG
	dumpAtom();
#endif

}
uint8_t adm_atom::skipBytes( uint32_t nb )
{
uint32_t pos;
	fseek(_fd,nb,SEEK_CUR);
	pos=ftell(_fd);
	if(pos>_atomStart+_atomSize+1) ADM_assert(0);	
	return 1;
}

uint8_t adm_atom::read( void )
{
	uint8_t a1;

		a1=fgetc(_fd);
	return a1;

}

uint16_t adm_atom::read16( void )
{
	uint8_t a1,a2;

		a1=fgetc(_fd);
		a2=fgetc(_fd);
	return (a1<<8)+(a2);

}


uint32_t adm_atom::read32( void )
{
	uint8_t a1,a2,a3,a4;

		a1=fgetc(_fd);
		a2=fgetc(_fd);
		a3=fgetc(_fd);
		a4=fgetc(_fd);
	return (a1<<24)+(a2<<16)+(a3<<8)+(a4);

}

uint32_t adm_atom::getFCC( void )
{
	return _atomFCC;
}
uint32_t adm_atom::getSize( void )
{
	return _atomSize-8;
}
uint8_t adm_atom::readPayload( uint8_t *whereto, uint32_t rd)
{
	uint32_t pos;

	pos=ftell(_fd);
	if(pos+rd>_atomSize+_atomStart)
	{
		printf("\n Going out of atom's bound!! (%ld  / %ld )\n",pos+rd,_atomSize+_atomStart);
		dumpAtom();
		exit(0);
	}
	uint32_t i;
	i=fread(whereto,rd,1,_fd);
	if(i!=1)
	{
		printf("\n oops asked %lu got %lu \n",rd,i);
	return 0;
	}
	return 1;

}
uint8_t adm_atom::dumpAtom( void )
{

	aprintf("Atom :");
	fourCC::print(_atomFCC);
	aprintf(" starting at pos %lu, size %lu\n",_atomStart,_atomSize);
	return 1;

}

uint8_t adm_atom::skipAtom( void )
{
	fseek(_fd,_atomStart+_atomSize,SEEK_SET);
	return 1;


}
uint8_t adm_atom::isDone( void )
{
	uint32_t pos=ftell(_fd);

	if(pos>=(_atomStart+_atomSize)) return 1;
	return 0;

}




//EOF

