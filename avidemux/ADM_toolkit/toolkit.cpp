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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "config.h"

#include "avi_vars.h"
#include "toolkit.hxx"
#include "interface.h"
#include "toolkit.hxx"


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

//
//    Sleep for n ms
//
void GUI_Sleep(uint32_t ms)
{
    if (ms < 10)
	return;

    ms -= 10;
    timespec ts, tr;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000 * 1000;
    nanosleep(&ts, &tr);
    
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
        		usleep(  50000L);			// wait 50 ms
        	}
}


// Get tick (in ms)
// Call with a 0 to initialize
// Call with a 1 to read
//_____________________
uint32_t getTime(int called)
{

    static struct timeval timev_s;

    static struct timeval timev;
    static struct timezone timez;

    int32_t tt;

    if (!called)
      {
	  called = 1;
	  gettimeofday(&timev_s, &timez);
	  return 0;
      }
    gettimeofday(&timev, &timez);
    tt = timev.tv_usec - timev_s.tv_usec;
    tt /= 1000;
    tt += 1000 * (timev.tv_sec - timev_s.tv_sec);
    return (tt);
}
uint32_t getTimeOfTheDay(void)
{
  

     struct timeval timev;
     struct timezone timez;

    int32_t tt;

   
    gettimeofday(&timev, &timez);
    tt = timev.tv_usec;
    tt /= 1000;
    tt += 1000 * (timev.tv_sec);
    return (tt&0xffffff);

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

/// convert frame number and fps to hour/mn/sec/ms
void  frame2time(	uint32_t frame, uint32_t fps, uint16_t * hh, uint16_t * mm,
	 			uint16_t * ss, uint16_t * ms)
{
    UNUSED_ARG(fps);
    uint32_t len2;
    len2 = video_body->getTime(frame);
    ms2time(len2,hh,mm,ss,ms);
}
void ms2time(uint32_t len2,uint16_t * hh, uint16_t * mm,
	 			uint16_t * ss, uint16_t * ms)
{
    *hh = (uint32_t) floor(len2 / (3600.F * 1000.F));
    len2 -= (uint32_t) floor(*hh * 3600.F * 1000.F);
    *mm = (uint32_t) floor(len2 / (60.F * 1000.F));
    len2 -= (uint32_t) floor(*mm * 60.F * 1000.F);
    *ss = (uint32_t) floor(len2 / (1000.F));
    len2 -= (uint32_t) floor((*ss * 1000.F));
    *ms = (uint32_t) floor(len2);

}

void UI_purge( void )
{
	
 	while (gtk_events_pending())
      				{
						  gtk_main_iteration();
			      }

}
// swap BE/LE : Ugly
uint32_t swap32( uint32_t in)
{
	uint8_t r[4],u;
	memcpy(&r[0],&in,4);
	u=r[0];
	r[0]=r[3];
	r[3]=u;
	u=r[1];
	r[1]=r[2];
	r[2]=u;
	memcpy(&in,&r[0],4);
	return in;
}
// swap BE/LE : Ugly
uint16_t swap16( uint16_t in)
{
	return ( (in>>8) & 0xff) + ( (in&0xff)<<8);
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
		
		gtk_editable_delete_text(GTK_EDITABLE(entry), 0,-1);
		gtk_editable_insert_text(GTK_EDITABLE(entry), value, strlen(value), &r);
}
uint8_t 	identMovieType(uint32_t fps1000)
{
#define INRANGE(value,type)  \
	{\
		if((fps1000 > value-300) &&( fps1000 < value+300))\
		{\
		 r=type;\
		 printf("Looks like "#type" \n");\
		 }\
	}
	uint8_t r=0;
	INRANGE(25000,FRAME_PAL);
	INRANGE(23976,FRAME_FILM);
	INRANGE(29970,FRAME_NTSC);

	return r;
}
uint8_t ms2time(uint32_t ms, uint32_t *h,uint32_t *m, uint32_t *s)
{
	uint32_t sectogo;
   	int  mm,ss,hh;


     				// d is in ms, divide by 1000 to get seconds
	  			sectogo = (uint32_t) floor(ms / 1000.);
				hh=sectogo/3600;
				sectogo=sectogo-hh*3600;
     				mm=sectogo/60;
      				ss=sectogo%60;

				*h=hh;
				*m=mm;
				*s=ss;
	return 1;
}
// Convert everything to lowercase
void		LowerCase(char *string)
{
	for(int i=strlen(string)-1;i>=0;i--)
	{
		string[i]=tolower(string[i]);
	}

}
// Does not exist in cygwin
char 		*ADM_rindex(const char *s, int c)
{
	if(!s) return NULL;
	if(!*s) return NULL;
	uint32_t l=strlen(s)-1;
	while(l)
	{
		if(s[l]==c) return (char *)(s+l);
		l--;
	}
	return NULL;


}
// Does not exist in cygwin
char 		*ADM_index(const char *s, int c)
{
	if(!s) return NULL;
	if(!*s) return NULL;
	uint32_t m=strlen(s);
	uint32_t l=0;
	while(l<m)
	{
		if(s[l]==c) return (char *)(s+l);
		l++;
	}
	return NULL;
}
//EOF
