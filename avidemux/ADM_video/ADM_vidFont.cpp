/***************************************************************************
                          ADM_vidFont.cpp  -  description
                             -------------------
    begin                : Sun Dec 15 2002
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
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <string.h> // thanks !
#include "config.h"
#include "ADM_library/default.h"

 #ifdef USE_FREETYPE

#include "ADM_vidFont.h"

static    FT_Library   	library;   		/* handle to library     */
static    int 			initialized=0; 	// 0 No init at all, 1 engine inited


ADMfont::ADMfont ( void )
{
	_conv=(iconv_t)-1;;
	_faceAllocated=0;

}

/**
	Deallocate font allocated stuff
*/
 ADMfont::~ADMfont( )
{
	if((int)_conv!=-1)
	{
		iconv_close(_conv);
		_conv=(iconv_t)-1;
	}
	if(_faceAllocated)
	{
		//
		FT_Done_Face(_face);
		_faceAllocated=0;

	}
}


int ADMfont::initFreeType( char *fontname )
{
int error;

	printf("\n ** Initializing FreeType **\n");
	if(initialized==0)
	{

    		error = FT_Init_FreeType( &library );
    		if ( error )
    		{
			  printf("\n Error Initializing Free Type (%d)\n",error);
			  return 0;
		  }
		initialized=1;
	}
  	error = FT_New_Face( library,
                         fontname,
                         0,
                         &_face );
    	if ( error == FT_Err_Unknown_File_Format )
    	{
   	   	printf("\n Error unknown font format (%d)\n",error);
			  return 0;

    	}
    	else if ( error )
    	{
     		printf("\n Error unknown error (font %d)\n",error);
		 return 0;
    	}
	_faceAllocated=1;

	error = FT_Set_Pixel_Sizes(
              _face,   /* handle to face object            */
              0,      /* pixel_width                      */
              16 );   /* pixel_height                     */

	printf("\n **  FreeType Initialized **\n");
   	return 1;
}
//____________________________________________________
int ADMfont::fontSetCharSet (char *charset)
{
//int error;

	//
	if(!strcmp(charset,"UNICODE")
	//||!strcmp(charset,"UTF-8")
	
	)
		{
			printf("\n Ascii, nothing to do\n");
			return 1;
		}
// for now hardcoded KOI8-R ->unicode
	_conv=iconv_open("UNICODE",charset); //"WINDOWS-1251");
	if((int)_conv==-1)
	{
		printf("\n Error initializing iconv...\n");
		return  0;
	}

// do a dummy conversion

	char *in,*out;
	size_t sin,sout,sz;
	uint32_t cin=65,cout;

	in=(char *)&cin;
	out=(char *)&cout;

	sin=1;
	sout=4;
#if  defined(ICONV_NEED_CONST)
 	sz=iconv(_conv,(const char **)&in,&sin,&out,&sout);
 #else
 	sz=iconv(_conv,&in,&sin,&out,&sout);
#endif
	if(sz==-1)
		printf("Something went wrong with iconv...\n");
// -- dummy
	if(charset) printf("Iconv initialized, using charset :%s \n",charset);
	return 1;


}
//---------------------------------
int ADMfont::fontSetSize ( int size)
{
int error;
	if(!_faceAllocated)
		{
				printf("\n not initialized");
				return 0;
		}
	   error = FT_Set_Pixel_Sizes(
              _face,   /* handle to face object            */
              0,      /* pixel_width                      */
              size );   /* pixel_height                     */

	return 1;
}
//____________________________________________________

int ADMfont::fontDraw(char *target, int  c, int stride, int size,int *ww)
{


			if(!_faceAllocated) return 0;
FT_GlyphSlot  slot = _face->glyph;  // a small shortcut
int  glyph_index,error;
int carac=0;
size_t sz;

       if((unsigned char)(c)==0x92) c='\''; // replace '
	// special french stuff
	/*
	if(( char)(c)=='é') c='e'; // replace '
	if(( char)(c)=='è') c='e'; // replace '
	if(( char)(c)=='à') c='a'; // replace '
	if(( char)(c)=='ç') c='c'; // replace ' */

	if((int)_conv!=-1)
	{
	char *in,*out;
	size_t sin,sout;

	in=(char *)&c;
	out=(char *)&carac;

	sin=1;
	sout=4;
#if  defined(ICONV_NEED_CONST)
       sz=iconv(_conv,(const char **)&in,&sin,&out,&sout);
#else
	sz=iconv(_conv, &in,&sin,&out,&sout);
#endif	
	}
	else
	{
		carac=c;
		//printf("\n no iconv initialized..\n");
	}
	//carac=0x0414; //c+256*3; //0x470; // russian phic;
	//printf("%c->%d\n",c,carac);
	    	glyph_index = FT_Get_Char_Index( _face, carac );
   		 	error = FT_Load_Glyph(
           				   _face,          /* handle to face object */
         				     glyph_index,   /* glyph index           */
        				      0 );  /* load flags, see below */
			if(error) return 0;

		   error = FT_Render_Glyph(
                		  slot,      /* glyph slot  */
                  		  ft_render_mode_normal);    /* render mode */

      	if(error) return 0;

       // now, draw to our target surface
       // inspired from MPlayer font rendering

	FT_Bitmap *bitmap=&(slot->bitmap);
	int heigh;
	int srow=0;

		
			heigh=bitmap->rows;
			target+=stride*(size-slot->bitmap_top);
			
	for (int h = heigh; h>0 ; h-- )
	{
							
	    for (int w =0;w< bitmap->width;  w++ )
	    {
				if(bitmap->buffer[srow+w])
			    *(target+w) = bitmap->buffer[srow+w];
		    
		 }  
		 target+=stride;
		 srow+=bitmap->pitch ;		
	}

			*ww=bitmap->width;
			return 1;
}
#endif
