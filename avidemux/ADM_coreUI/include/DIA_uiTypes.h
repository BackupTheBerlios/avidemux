/***************************************************************************
                          DIA_factoryStubs.h
  
  (C) Mean 2008 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DIA_UITYPES_H
#define DIA_UITYPES_H
typedef enum
{
        ADM_UI_CLI=1,
        ADM_UI_GTK=2,
        ADM_UI_QT4=4,
        ADM_UI_ALL=ADM_UI_CLI+ADM_UI_GTK+ADM_UI_QT4
}ADM_UI_TYPE;

#endif

