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


uint8_t oplug_mpeg_vcd(const char *name);
uint8_t oplug_mpeg_vcd_ps(const char *name);
uint8_t oplug_mpeg_svcd(const char *name=NULL);
uint8_t oplug_mpeg_dvd(const char *name=NULL);
uint8_t oplug_mpeg_dvd_ps(const char *name=NULL);
uint8_t oplug_mpeg_svcd_ps(const char *inname=NULL);
uint8_t oplug_mpeg_ts(const char *inname=NULL);


