/***************************************************************************
                          adm_encdivx.cpp  -  description
                             -------------------
    begin                : Sun Jul 14 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr


Derived from MLdonkey systray code
http://savannah.nongnu.org/cvs/?group=mldonkey
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"

#include <string.h>
#include <stdio.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

extern "C"
{
#include "ADM_toolkit/systray.h"
}
#include <ADM_assert.h>
#include "default.h"
#include "ADM_tray.h"


#include "ADM_gui2/support.h"

static int nbTray=0;

extern GdkPixbuf        *create_pixbuf                  (const gchar     *filename);
/*
class ADM_tray
{
protected:

public:
        ADM_tray(char *name);
        ~ADM_tray();
        setPercent(int percent);
        setStatus(int working);

};*/

ADM_tray::ADM_tray(char *name)
{
        ADM_assert(!nbTray);
        nbTray++;


  GtkWidget   *img;
  GdkPixbuf   *pixbuf;

  pixbuf=create_pixbuf("systray.xpm");
  sys=adm_new_systray(pixbuf,name);

  

}
ADM_tray::~ADM_tray()
{
        nbTray--;
        ADM_assert(!nbTray);
        if(sys)
                adm_delete_systray(sys);
        sys=NULL;
}
uint8_t ADM_tray::setPercent(int percent)
{
char percentS[40];
        sprintf(percentS,"%d %%",percent);
        if(sys)
                adm_change_tooltip(sys,percentS);
        return 1;
}
uint8_t ADM_tray::setStatus(int working)
{
        return 1;
}
