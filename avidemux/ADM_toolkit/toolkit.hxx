/***************************************************************************
                          toolkit.hxx  -  description
                             -------------------
    begin                : Fri Dec 14 2001
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

#ifndef __TOOLKIT__
#define __TOOLKIT__

void            GUI_Alert(const char *alertstring);
void            GUI_Info(const char *alertstring);
void            GUI_Info_HIG(const ADM_LOG_LEVEL level,const char *primary, const char *secondary_format, ...);
void            GUI_Error_HIG(const char *primary, const char *secondary_format, ...);
int             GUI_Confirmation_HIG(const char *button_confirm, const char *primary, const char *secondary_format, ...);
int             GUI_YesNo(const char *primary, const char *secondary_format, ...);
int             GUI_Question(const char *alertstring);
void            GUI_Sleep(uint32_t ms);
int             GUI_Alternate(char *title,char *choice1,char *choice2);
uint32_t        getTime( int called );;
void            runDialog(volatile int *lock);
// warning : they are in GUI_enter
uint8_t  		GUI_getDoubleValue(double *valye, float min, float max, const char *title);
uint8_t  		GUI_getIntegerValue(int *valye, int min, int max, const char *title);

uint8_t		isQuiet(void);
void            GUI_Verbose(void);
void            GUI_Quiet(void);
#endif
