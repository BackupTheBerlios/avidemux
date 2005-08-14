
/***************************************************************************
    copyright            : (C) 2001/2005 by mean
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

#ifndef FILESEL_H
#define FILESEL_H
typedef void SELFILE_CB(char *);
void GUI_FileSelRead(const char *label,SELFILE_CB cb) ;
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) ;
void GUI_FileSelRead(const char *label, char * * name);
void GUI_FileSelWrite(const char *label, char * * name);

uint8_t FileSel_SelectRead(const char *title,char *target,uint32_t max, const char *source);

char *getBaseDir(void);
#endif
//EOF
