
/***************************************************************************
    Class to handle Xv accelerated renderer
    
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

#ifndef T_XVRENDER_H
#define T_XVRENDER_H
class XvAccelRender: public QTAccelRender
{
      public:
                              XvAccelRender( void ) ;
              virtual	uint8_t init( QWidget * window, uint32_t w, uint32_t h);
              virtual	uint8_t end(void);				
              virtual uint8_t display(uint8_t *ptr, uint32_t w, uint32_t h);
};
#endif




