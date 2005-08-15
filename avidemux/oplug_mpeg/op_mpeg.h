/***************************************************************************
                          op_mpeg.h  -  description
                             -------------------
    begin                : Fri Aug 7 2009
    copyright            : (C) 2009 by mean
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


uint8_t oplug_mpeg_vcd(char *name);
uint8_t oplug_mpeg_vcd_ps(char *name);
uint8_t oplug_mpeg_svcd(char *name=NULL);
uint8_t oplug_mpeg_dvd(char *name=NULL);
uint8_t oplug_mpeg_dvd_ps(char *name=NULL);
uint8_t oplug_mpeg_svcd_ps(char *inname=NULL);
uint8_t oplug_mpeg_ts(char *inname=NULL);


