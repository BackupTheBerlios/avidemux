/***************************************************************************
                          ADM_vidFont.h  -  description
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
 #ifdef USE_FREETYPE

 #include <ft2build.h>
 #include FT_FREETYPE_H

 class ADMfont
 {
 private:
 		  
		   FT_Face    _face;
		   int			_faceAllocated;
		   uint8_t		_use2bytes;
		   uint8_t		_hold;
		   int			_value;
 public :
		ADMfont(void );
		~ADMfont();

	int initFreeType( char *fontname );
	int fontDraw(char *target, int c, int stride, int size,int *ww);
	int fontSetSize ( int size);
	int fontSetCharSet (char *code);

};

#endif
