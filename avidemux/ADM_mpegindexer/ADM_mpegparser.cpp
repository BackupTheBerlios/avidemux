/***************************************************************************
                          ADM_mpegparser.cpp  -  description
                             -------------------
    begin                : Tue Oct 15 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This just handles mpeg sync search and little endian/big endin integer reading
    
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
#include <string.h>
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
#include <ADM_assert.h>

#include "ADM_mpegindexer/ADM_mpegparser.h"

 
mParser::mParser( void )
{
		_fd=NULL;
		_sizeFd=NULL;
		_off=0;
		_nbFd=0;
		_curFd=0;
}

mParser::~mParser()
{
	if(_nbFd)
	{
		 for(uint32_t i=0;i<_nbFd;i++)
	  	 {
		 		if(_fd[i])
					fclose(_fd[i]);	
		 }
		delete [] _fd;
		delete [] _sizeFd;
		 
		
	}
	
}


uint8_t mParser::open( char *filename )
{
	char *dot = NULL;                   // pointer to the last dot in filename
	uint8_t decimals = 0;               // number of decimals
	char *left = NULL, *number = NULL, *right = NULL; // parts of filename (after splitting)

	char *followup = new char[ strlen(filename) + 1 ]; // possible follow-up filename
	uint8_t first_followup = 1;         // indicates first follow-up
	uint8_t last_followup = 0;          // indicates last follow-up (by number: 99)
	uint8_t count = 0;                  // number of follow-ups

	FILE **buffer_fd = NULL;            // _fd buffer
	uint64_t *buffer_sizeFd = NULL;     // _sizeFd buffer

	int i = 0;                          // index (general use)


	// find the last dot
	dot = strrchr( filename, '.' );

	// count the decimals before the dot
	decimals = 1;
	while( (dot != NULL) && ((dot - decimals) != filename) &&
	       (dot[0 - decimals] >= '0') && (dot[0 - decimals] <= '9') )
		{ decimals++; }
	decimals--;

        // Nuv files can have 20 decimals
        // Keep it down to 10000
        if(decimals>4) decimals=4;
        
	// no number sequence
	if( decimals == 0 )
	{
		printf( "\nSimple loading: \n" );
		delete [] followup;
		_nbFd = 1;
		_curFd = 0;
		_fd = new FILE * [_nbFd];
		_sizeFd = new uint64_t [_nbFd];

		// open file
		if(! (_fd[0] = fopen(filename, "rb")) )
		  { return 0; }

		// calculate file-size
		fseeko( _fd[0], 0, SEEK_END );
		_sizeFd[0] = ftello( _fd[0] );
		fseeko( _fd[0], 0, SEEK_SET );

		printf( " file: %s, size: %u \n", filename, _sizeFd[0] );
		printf( " found 1 files \n" );
		printf( "Done \n" );
		return 1;
	}

	// possible number sequence
	else
	{
		// split the filename in <left>, <number> and <right>
		// -----
		
		// <left> part
		left = new char[(dot - filename - decimals) + 1];
		strncpy( left, filename, (dot - filename - decimals) );
		left[(dot - filename - decimals)] = '\0';

		// <number> part
		number = new char[decimals + 1];
		strncpy( number, (dot - decimals), decimals );
		number[decimals] = '\0';

		// <right> part
		right = new char[ strlen(dot) ];
		strcpy( right, dot );

		// add the file, and all existing follow-ups
		// -----
                uint32_t tabSize;
                
                tabSize=(uint32_t)pow(10,decimals);
                buffer_fd = new FILE * [tabSize];
                buffer_sizeFd = new uint64_t [tabSize];

		printf( "\nAuto adding: \n" );
		while( last_followup == 0 )
		{
			strcpy( followup, left );
			strcat( followup, number );
			strcat( followup, right );

			// open file
			buffer_fd[count] = fopen(followup, "rb");
			if(! buffer_fd[count] )
			{
				// we need at least one file!
				if( first_followup == 1 )
				  { return 0; }
				else
				  { printf( " file: %s not found. \n", followup ); break; }
			}

			// calculate file-size
			fseeko( buffer_fd[count], 0, SEEK_END );
			buffer_sizeFd[count] = ftello( buffer_fd[count] );
			fseeko( buffer_fd[count], 0, SEEK_SET );

			printf( " file %d: %s, size: %u \n", (count + 1), followup, buffer_sizeFd[count] );

			// increase number
			number[decimals - 1] = number[decimals - 1] + 1;
			for( i = decimals - 1; i >= 0; i-- )
			{
				if( number[i] > '9' )
				{
					if( i == 0 )
					  { last_followup = 1; break; }
					number[i] = '0';
					number[i - 1] = number[i - 1] + 1;
				}
			}

			first_followup = 0;
			count++;
		} // while( last_followup == 0 )

		// copy from buffer
		_nbFd = count;
		_curFd = 0;
		_fd = new FILE * [_nbFd];
		_sizeFd = new uint64_t [_nbFd];

		for( i = 0; i < count; i++ )
		{
			_fd[i] = buffer_fd[i];
			_sizeFd[i] = buffer_sizeFd[i];
		}

		// clean up
		delete [] followup;
		delete [] left;
		delete [] number;
		delete [] right;
		delete [] buffer_fd;
		delete [] buffer_sizeFd;

		printf( " found %d files \n", count );
		printf( "Done \n" );
	} // if( decimals == 0 )
		return 1;
} // mParser::open()


/*----------------------------------------

------------------------------------------*/
  
uint8_t mParser::forward(uint64_t of)
{
uint64_t k;
				if(_off+of>_sizeFd[_curFd])
				{
					if(_curFd>=_nbFd-1)
					{
						 printf("\n Trying to go out of bound\n"); 
						 return 0;
					}
					k=_sizeFd[_curFd]-_off;
					_curFd++; // next seg
					k=of-k;
					fseeko(_fd[_curFd],k,SEEK_SET); 
					_off=k;
					return 1;
				}
				fseeko(_fd[_curFd],of,SEEK_CUR);
				_off+=of;
				return 1;	
}

uint8_t mParser::setpos(uint64_t o)
{
//uint32_t idx;
uint64_t sum=0;

			for(uint32_t i=0;i<_nbFd;i++)
			{
				if( (sum+_sizeFd[i])>o) // right seg
					{
							_curFd=i;
							_off=o-sum;
							fseeko(_fd[_curFd],_off,SEEK_SET);
						  return 1;
					}
					sum+=_sizeFd[i];
			}
			printf("\n cannot seek to %llu !\n",o);
			return 0;		
}
//
//	Search packet signature and return packet type
//_______________________________________
uint8_t mParser::sync(uint8_t *stream)
{
uint32_t val,hnt;

				val=0;
				hnt=0;			
			// preload
				hnt=(read8i()<<16) + (read8i()<<8) +read8i();								;				
				while((hnt!=0x00001))
				{
						
						hnt<<=8;
						val=read8i();					
						hnt+=val;
						hnt&=0xffffff;
						if(_curFd==_nbFd-1)
						{	
							if((4+_off)>=_sizeFd[_curFd]) return 0;
						}
				}
				
	      *stream=read8i();
	      return 1;
	
}

uint8_t mParser::getpos(uint64_t *o)
{
uint8_t i;
uint64_t sum=0;
				for(i=0;i<_curFd;i++) sum+=_sizeFd[i];
			*o=_off; //ftello(_fd[_curFd]);
			*o+=sum;
			return 1;
	
	
}


//
uint8_t mParser::nearEnd(uint32_t w)
{
//	uint64_t pos;
				if(_curFd!=_nbFd-1) return 0;
				
				if((_sizeFd[_curFd]-_off)<w) return 1;
				return 0;
	
}
 uint64_t mParser::getSize( void ) 
 {
	 	uint64_t r=0;
	  
	  for(uint32_t i=0;i<_nbFd;i++)
	  		r+=_sizeFd[i];
	    
		return r;	    
	 
	}
/*--------------------------------------------------
		Read l bytes from file
----------------------------------------------------*/
uint8_t mParser::read32(uint32_t l, uint8_t *buffer)
{
uint32_t r;

	if(_off+l<_sizeFd[_curFd])
	{
		r=fread(buffer,1,l,_fd[_curFd]);
		_off+=r;
		if(r!=l)
		{
		   printf("\n not enought read seg:%ld wanted:%ld got :%ld!\n",_curFd,l,r); 
			 return 0;
		}
		return 1;	
	}
	// else we split
	if(_curFd>=_nbFd-1)
	{
		_off=_sizeFd[_nbFd-1];
		return 0;
	}
	
	// read whatever from current
	uint32_t left;
	left=_sizeFd[_curFd]-_off;
	r=fread(buffer,1,left,_fd[_curFd]);
	_off+=r;
	if(r!=left)
	{ printf("\n not enought read (tail) !\n"); 
		 return 0;
	}
	// switch seg
	buffer+=left;
	l-=left;
	_curFd++;
	_off=0;
	fseeko(_fd[_curFd],0,SEEK_SET);
	return read32(l,buffer);
	
}	
#ifdef MP_NOINLINE
uint32_t mParser::read32i(void )
			{
					uint32_t v;
					uint8_t c[4];
					read32(4,c);

					v= (c[0]<<24)+(c[1]<<16)+(c[2]<<8)+c[3];
					return v;
			}
			uint16_t mParser::read16i(void )
			{
					uint16_t v;
					uint8_t c[2];

					read32(2,c);
					v= (c[0]<<8)+c[1];
					return v;
			}
			uint8_t mParser::read8i(void )
			{
					uint8_t u;
					read32(1,(uint8_t *)&u);
					return u;
			}
#endif
