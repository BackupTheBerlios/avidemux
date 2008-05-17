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

#include "ADM_toolkitGtk.h"
#include "ADM_tray.h"
extern "C"
{
#include "systray.h"
}
static int nbTray=0;
static  GdkPixbuf   **pixbuf=NULL;

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
static char *animated[]={
    "film1.png","film3.png","film5.png","film7.png","film9.png",
    "film11.png","film13.png","film15.png","film17.png","film19.png",
    "film21.png","film23.png"};
ADM_tray::ADM_tray(char *name)
{
        ADM_assert(!nbTray);
        nbTray++;


  int nb=sizeof(animated)/sizeof(char *);
  if(!pixbuf)
  {
      pixbuf=new GdkPixbuf*[nb];
      for(int i=0;i<nb;i++)
      {
        pixbuf[i]=create_pixbuf(animated[i]);
        if(!pixbuf[i])
        {
          printf("Failed to create <%s>\n",animated[i]);
          ADM_assert(0);
        }
      }

  }
  sys=adm_new_systray(pixbuf,nb,name);

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
        {
                adm_changeIcon_systray();
                adm_change_tooltip(sys,percentS);
        }
        return 1;
}
uint8_t ADM_tray::setStatus(int working)
{
        return 1;
}
