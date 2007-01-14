
/***************************************************************************
    copyright            : (C) 2006 by mean
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
#ifndef TSDLRENDER_H
#define TSDLRENDER_H
class sdlAccelRender: public AccelRender
{
  protected:
              int     useYV12;
              uint8_t *decoded;
      public:
                                sdlAccelRender( void ) ;
              virtual	uint8_t init( GUI_Info * window, uint32_t w, uint32_t h);
              virtual	uint8_t end(void);
              virtual   uint8_t display(uint8_t *ptr, uint32_t w, uint32_t h);
};

#endif