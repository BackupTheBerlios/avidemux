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
#include "ADM_toolkit/eggtrayicon.h"
}
#include <ADM_assert.h>
#include "default.h"
#include "ADM_tray.h"


#include "ADM_gui2/support.h"

static int nbTray=0;

static GtkWidget   *systray_icon = NULL;
static GtkWidget   *evbox = NULL;
static GtkTooltips *systray_icon_tooltips = NULL;
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
  systray_icon = GTK_WIDGET (egg_tray_icon_new ("Avidemux"));
  img = gtk_image_new_from_pixbuf (pixbuf);
  evbox = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (systray_icon), evbox);
  gtk_container_add (GTK_CONTAINER (evbox), img);
  gtk_widget_show_all (systray_icon);
  gtk_widget_realize (systray_icon);
  gtk_widget_show (systray_icon);

  systray_icon_tooltips = gtk_tooltips_new ();
  gtk_tooltips_enable (systray_icon_tooltips);

  gtk_tooltips_set_tip (systray_icon_tooltips, systray_icon, "avidemux", "");

  

}
ADM_tray::~ADM_tray()
{
        nbTray--;
        ADM_assert(!nbTray);
        gtk_widget_destroy(systray_icon);
        systray_icon=NULL;
}
uint8_t ADM_tray::setPercent(int percent)
{
char percentS[40];
        sprintf(percentS,"%d %%",percent);
         gtk_tooltips_set_tip (systray_icon_tooltips, systray_icon, percentS, "");
        return 1;
}
uint8_t ADM_tray::setStatus(int working)
{
        return 1;
}
