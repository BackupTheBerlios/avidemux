/***************************************************************************
                          toolkit.cpp  -  description
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
#include "toolkit_gtk.h"

#ifndef __WIN32
#include <unistd.h>
#endif




GtkWidget *GUI_PixmapButtonDefault(GdkWindow * window, const gchar ** xpm,
                                  const gchar * tooltip);
GtkWidget *GUI_PixmapButton(GdkWindow * window, const gchar ** xpm,
                            const gchar * tooltip, gint border);




GtkTooltips *tooltips = NULL;


//
//_______________________________

//
GtkWidget *GUI_PixmapButtonDefault(GdkWindow * window, const gchar ** xpm,
                                  const gchar * tooltip)
{
    return GUI_PixmapButton(window, xpm, tooltip, 2);
}

//____________________________________________
//  Used to pack an icon onto a button
//  Adapted from GTK API tutorial
//____________________________________________
GtkWidget *GUI_PixmapButton(GdkWindow * window, const gchar ** xpmd,
                            const gchar * tooltip, gint border)
{

    //
    GtkWidget *button;
    GtkWidget *pixmap;
    GdkPixmap *xpm;
    GdkBitmap *mask;

    xpm = gdk_pixmap_create_from_xpm_d(window, &mask, NULL, (gchar **)xpmd);
    pixmap = gtk_pixmap_new(xpm, mask);
    button = gtk_button_new();

    gtk_container_set_border_width(GTK_CONTAINER(button), border);
    gtk_container_add(GTK_CONTAINER(button), pixmap);

    if (tooltips == NULL)
        tooltips = gtk_tooltips_new();
    if (tooltip != NULL)
        gtk_tooltips_set_tip(tooltips, button, tooltip, NULL);

    gtk_widget_show_all(button);

    return button;
}


void MenuAppend(GtkMenu * menu, const char *text)
{
    GtkWidget *glade_menuitem;

    glade_menuitem = gtk_menu_item_new_with_label(text);
    gtk_widget_show(glade_menuitem);
    gtk_menu_append(GTK_MENU(menu), glade_menuitem);

}

void runDialog(volatile int *lock)
{
        
              while (!*lock)
              {
                                while (gtk_events_pending())    		  	gtk_main_iteration();         
                        ADM_usleep(  50000L);			// wait 50 ms
                }
}


/// gtk add on to get the rank in a pull down menu
uint8_t getRangeInMenu(GtkWidget * Menu)
{
    GtkWidget *br, *active;
    int mode;

    br = gtk_option_menu_get_menu(GTK_OPTION_MENU(Menu));
    active = gtk_menu_get_active(GTK_MENU(br));
    mode = g_list_index(GTK_MENU_SHELL(br)->children, active);
    return (uint8_t) mode;
}
/*
        Change the text of the nth entry in the menu

*/
void changeTextInMenu(GtkWidget *menu,int range,const char *text)
{
    GtkWidget *br, *active;
    int mode;

    br = gtk_option_menu_get_menu(GTK_OPTION_MENU(menu));
        
    //gtk_label_set_text(GTK_LABEL(br),text);

}

void UI_purge( void )
{
        
        while (gtk_events_pending())
                                {
                                                  gtk_main_iteration();
                              }

}
// read an entry as an integer

int gtk_read_entry(GtkWidget *entry)
{
char *str;
int value;
                str =		  gtk_editable_get_chars(GTK_EDITABLE ((entry)), 0, -1);
                value = (int) atoi(str);
                return value;
}
float gtk_read_entry_float(GtkWidget *entry)
{
char *str;
float value;
                str =		  gtk_editable_get_chars(GTK_EDITABLE ((entry)), 0, -1);
                value = (int) atof(str);
                return value;
}

void gtk_write_entry(GtkWidget *entry, int value)
{
char string[400];
gint r;
                sprintf(string,"%d",value);
                gtk_editable_delete_text(GTK_EDITABLE(entry), 0,-1);
                gtk_editable_insert_text(GTK_EDITABLE(entry), string, strlen(string), &r);
}
void gtk_write_entry_float(GtkWidget *entry, float value)
{

gint r;
char string[400];
                sprintf(string,"%f",value);		
                gtk_editable_delete_text(GTK_EDITABLE(entry), 0,-1);
                gtk_editable_insert_text(GTK_EDITABLE(entry), string, strlen(string), &r);
}

void gtk_write_entry_string(GtkWidget *entry, char *value)
{

gint r;
                if(!value) return;
                
                gtk_editable_delete_text(GTK_EDITABLE(entry), 0,-1);
                gtk_editable_insert_text(GTK_EDITABLE(entry), value, strlen(value), &r);
}

/**
    \fn UI_getPhysicalScreenSize
    \brief return the physical size of display in pixels
*/
uint8_t UI_getPhysicalScreenSize(uint32_t *w, uint32_t *h)
{
#ifdef __WIN32
	getWorkingArea(w, h);
#else
	static int inited = 0;
	static int ww,hh;

    if (!inited)
    {      
      GdkScreen* sc = gdk_screen_get_default();
      ww = gdk_screen_get_width(sc);
      hh = gdk_screen_get_height(sc);
      inited = 1;
    }

    *w=ww;
    *h=hh;
#endif

	return 1;
}
//EOF
