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
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef __FreeBSD__
          #include <sys/types.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include <math.h>

#include "ADM_library/default.h"
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
uint8_t mParser::open( char *name)
{
uint8_t check=1;
uint8_t count=1;
uint32_t l=strlen(name);
char *tmp=new char[l+1];
	  // first we check if we search for more file
	  if(l<5) check=0;
	  else
	   if(name[l-4]!='.') check=0;
	   else
	   {
			 uint8_t a= name[l-5];
			 
			 	if(a>'9') check=0;
			  if(a<'0') check=0;
			 
			}
	 
	  if(check)
	  {	// check how many files..
	  	
	  	
	   	FILE *f;
	    count=0;
	   	strcpy(tmp,name);
	    printf("\n Auto adding :\n");
	    while(1)
	    {
					printf(" trying %s \n",tmp);
					if(!(f=fopen(tmp,"rb"))) 
					{
						printf("Not found \n");
						break;
					}
					fclose(f);
					count++;
					tmp[l-5]++;				
			}
		}
	
		if(!count)
		{
				delete [] tmp;
			 return 0;
		}                

	   _nbFd=count;
	 	 printf(" found %ld files\n",_nbFd);	   
	   _curFd=0;
		 _fd=new FILE*[_nbFd];
		 _sizeFd=new uint64_t[_nbFd];		 
	   strcpy(tmp,name);
	  // open each one
	  for(uint32_t i=0;i<_nbFd;i++)
	  {
	 		 _fd[i]=fopen(tmp,"rb");
			if(!_fd[i])
			{
				delete [] tmp;
				return 0;
			}
			// Get size
			fseeko( _fd[i],0,SEEK_END);
			_sizeFd[i]=ftello(_fd[i]);  
			fseeko(_fd[i],0,SEEK_SET);
			printf(" file %ld :%s ",i,tmp);
			printf(" size : %llu\n",_sizeFd[i]);
			
			tmp[l-5]++;					
		}	
		printf("Done\n");	
		delete [] tmp;		
		return 1;	
}
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
