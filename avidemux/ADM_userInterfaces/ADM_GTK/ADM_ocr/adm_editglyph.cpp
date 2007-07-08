/***************************************************************************
    copyright            : (C) 2007 by mean
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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "default.h"
#include "gtk/gtk.h"
#include <ADM_assert.h>



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"

#include "../ADM_GTK/ADM_toolkit_gtk/toolkit_gtk.h"
#include "../ADM_GTK/ADM_toolkit_gtk/toolkit_gtk_include.h"

#include "ADM_ocr/adm_glyph.h"

static GtkWidget *create_dialog1 (void);
static admGlyph *currentGlyph=NULL;
static GtkWidget *dialog;

static uint8_t loadGlyph(char *name,admGlyph *head,uint32_t *outNb);
static gboolean glyphDraw( void );
static void glyphUpdate(void );
/**
    \fn DIA_glyphEdit
    \brief Dialog to edit glyph
*/
uint8_t DIA_glyphEdit(void)
{
  char *glyphName;
  admGlyph head(1,1);
  
  
  uint32_t nbGlyph=0;
  uint8_t ret=0;
  // First select a file
  
   GUI_FileSelRead(_("Select GlyphFile to edit"), &glyphName);
  if(!glyphName) return 0;
  
  // Try to load it
  if(!loadGlyph(glyphName,&head,&nbGlyph) || !nbGlyph)
  {
    destroyGlyphTree(&head);
    return 0;
  }
  // Convert the linear glyph to glyph array
  admGlyph *glyphArray[nbGlyph];
  admGlyph *cur=head.next;
  uint32_t idx=0;
  while(cur)
  {
     glyphArray[idx++]=cur;
     cur=cur->next;
  }
  ADM_assert(idx<=nbGlyph);
  nbGlyph=idx;
  // Glyph loaded, here we go
  currentGlyph=head.next;
  dialog=create_dialog1 ();
  gtk_register_dialog(dialog);
  
  // Register callbacks
  #define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)
  #define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y,GTK_SIGNAL_FUNC(z),   NULL);
#define ACTION_PREV 10
#define ACTION_NEXT 20
  ASSOCIATE(buttonPrev,ACTION_PREV);
  ASSOCIATE(buttonNext,ACTION_NEXT);
  
  CONNECT(drawingarea1,expose_event,glyphDraw);
  gtk_widget_show(dialog);
  glyphUpdate();
  while(1)
  {
   gint b=gtk_dialog_run(GTK_DIALOG(dialog));
    switch(b)
    {
      case ACTION_PREV: 
                        printf("PREV\n");
                        if(currentGlyph!=head.next)
                        {
                           admGlyph *father;
                           father=glyphSearchFather(currentGlyph,&head);
                           if(father)
                           {
                               currentGlyph=father;
                               glyphUpdate();
                           }
                          
                        }
                        continue;break;
      case ACTION_NEXT: 
                        printf("NEXT\n");
                        if(currentGlyph->next)
                        {
                          currentGlyph=currentGlyph->next; 
                          glyphUpdate();
                        }
                        continue;break; 
    }
    break; // exit while(1)
  }
  
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  
  destroyGlyphTree(&head);
  return ret;

}
/**
    \fn glyphUpdate
    \brief Update all fields in the dialog wrt currentGlyph
*/
void glyphUpdate(void )
{
  if(!currentGlyph) return;
  gtk_widget_set_usize(WID(drawingarea1), currentGlyph->width+4, currentGlyph->height+4);
  glyphDraw();
  if(currentGlyph->code)
  {
    gtk_editable_delete_text(GTK_EDITABLE(WID(entry1)), 0,-1); 
    // Set our text
    gtk_entry_set_text (GTK_ENTRY (WID(entry1)), currentGlyph->code);
  }
}
/**
    \fn     glyphDraw
    \brief  display glyph
*/
gboolean glyphDraw( void )
{
    if(!currentGlyph) return true;
    gdk_draw_gray_image(WID(drawingarea1)->window, WID(drawingarea1)->style->fg_gc[GTK_STATE_SELECTED /*GTK_STATE_NORMAL*/],
                        2,                          // X
                        2,                          // y
                        currentGlyph->width,                          //width
                        currentGlyph->height,                          //h*2, // heigth
                        GDK_RGB_DITHER_NONE,
                        currentGlyph->data, // buffer
                        currentGlyph->width );
    return true;
}

/**
    \fn loadGlyph
    \brief Load a glyph set
*/
uint8_t loadGlyph(char *name,admGlyph *head,uint32_t *outNb)
{
  FILE *out;
  admGlyph *glyph,*nw;
  uint32_t N,w,h,slen;
  uint32_t nbGlyphs;
 
  glyph=head;
  out=fopen(name,"rb");
  if(!out)
  {
    GUI_Error_HIG(_("File error"), _("Could not read \"%s\"."), name);
    return 0;
  }
#define READ(x) fread(&(x),sizeof(x),1,out);
    nbGlyphs=0;
    READ(N);
    while(N--)
    {
        
      READ(w);
      READ(h);
      nw=new admGlyph(w,h);
      fread(nw->data,w*h,1,out);
      READ(slen);
      if(slen)
      {
        nw->code=new char[slen+1];
        fread(nw->code,slen,1,out);
        nw->code[slen]=0;
      }
      glyph->next=nw;
      glyph=nw;
      nbGlyphs++;
    }
    
    fclose(out);
    *outNb=nbGlyphs;
    return 1;

}


//**********************************
GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox2;
  GtkWidget *drawingarea1;
  GtkWidget *entry1;
  GtkWidget *hbox1;
  GtkWidget *buttonPrev;
  GtkWidget *buttonNext;
  GtkWidget *hseparator1;
  GtkWidget *button3;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("dialog1"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox2, TRUE, TRUE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox2), drawingarea1, TRUE, TRUE, 0);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (vbox2), entry1, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  buttonPrev = gtk_button_new_from_stock ("gtk-media-previous");
  gtk_widget_show (buttonPrev);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonPrev, TRUE, TRUE, 0);

  buttonNext = gtk_button_new_from_stock ("gtk-media-next");
  gtk_widget_show (buttonNext);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonNext, TRUE, TRUE, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (hbox1), hseparator1, TRUE, TRUE, 0);

  button3 = gtk_button_new_from_stock ("gtk-save-as");
  gtk_widget_show (button3);
  gtk_box_pack_start (GTK_BOX (hbox1), button3, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT (dialog1, entry1, "entry1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonPrev, "buttonPrev");
  GLADE_HOOKUP_OBJECT (dialog1, buttonNext, "buttonNext");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (dialog1, button3, "button3");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  return dialog1;
}


