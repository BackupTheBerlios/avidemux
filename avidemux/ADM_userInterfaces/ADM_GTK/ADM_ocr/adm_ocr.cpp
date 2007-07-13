/***************************************************************************
                         
        Very simple OCR engine

        We do it in 3 passes
        
                Ask the vobsub decoder for a bitmap
                Try to split the bitmap in lines
                For each lines try to split in glyph (i.e. horizontal line)
                Detour the glyph
                If the detoured glyph has a width much less inferiror to its width
                it probably means we have a italic or kerning.
                In that case use the detouring to isolate the glyphs


         A bit of warning. 
                        The UI code is ugly.
                        Bottom is the last actual line so to get height you have to to last-first +1 !

    begin                : Jan 2005
    copyright            : (C) 2002 by mean
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
#include "gtk/gtk.h"
#include <ADM_assert.h>


#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"

//#include "unistd.h"

#include "ADM_toolkit/toolkit.hxx"
#include "../ADM_GTK/ADM_toolkit_gtk/toolkit_gtk.h"
#include "../ADM_GTK/ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"


#include "ADM_toolkit/filesel.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_video/ADM_vobsubinfo.h"

#include "ADM_video/ADM_vidVobSub.h"
#include "ADM_leftturn.h"
#include "DIA_enter.h"
#include "ADM_ocr.h"
/******************************/

#define TESTSUB "/home/fx/usbstick/subs/vts_01_0.idx"
#define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y,GTK_SIGNAL_FUNC(z),   NULL);







extern  uint8_t DIA_vobsub(vobSubParam *param);
//********************************************
extern  GtkWidget *DIA_ocr(void);
static gboolean   gui_draw( void );
static gboolean   gui_draw_small( void );
static void       displaySmall( admGlyph *glyph );
static int        cb_accept(GtkObject * object, gpointer user_data);

static GtkWidget *dialog;
static GtkWidget *mainDisplay;
static GtkWidget *smallDisplay;
static uint32_t redraw_x,redraw_y;
//********************************************
ReplyType glyphToText(admGlyph *glyph);
static ReplyType setup(void);
//********************************************
static uint8_t ocrSaveGlyph(void);
static  vobSubParam subparam={NULL,0,0};
static uint8_t *workArea;
static vobSubBitmap *bitmap;
static admGlyph head(250,4);
static uint32_t lang_index=0;
static uint32_t nbGlyphs;
static char decodedString[1024];

typedef enum
{
    actionLoadVob=10,
    actionSaveSub,
    actionGo,
    actionLoadGlyph,
    actionSaveGlyph,
    actionSkip,
    actionSkipAll,
    actionAccept,
    actionIgnore,
    actionCalibrate
}ocrAction;
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Main
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
uint8_t ADM_ocr_engine( void)
{
// 
    uint32_t nbSub=0;
    FILE *out=NULL;
    head.next=NULL;
    ADMVideoVobSub *vobsub=NULL;
    uint32_t startTime,endTime;
    uint32_t w,h,oldw=0,oldh=0;
    uint32_t oldbitmapw=0;
    uint32_t oldbitmaph=0;
    uint32_t first,last;
    uint32_t seqNum;
    char     text[1024];
    lang_index=0;
    nbGlyphs=0;
    ReplyType reply;
    
// Create UI && prepare callback
    
    dialog=DIA_ocr();
    gtk_register_dialog(dialog);
#define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)
    ASSOCIATE(buttonStart,actionGo);
    ASSOCIATE(buttonOk,   actionAccept);
    ASSOCIATE(buttonSkip,     actionSkip);
    ASSOCIATE(buttonSkipAll,     actionSkipAll);
    ASSOCIATE(buttonIgnore,   actionIgnore);
    ASSOCIATE(buttonCalibrate,   actionCalibrate);
    
    ASSOCIATE(buttonGlyphLoad,   actionLoadGlyph);
    ASSOCIATE(buttonGlyphSave,   actionSaveGlyph);
    
    ASSOCIATE(buttonVobsub,   actionLoadVob);
    ASSOCIATE(buttonSrt,   actionSaveSub);
   
    gtk_widget_show(dialog);
//  disable
    
    mainDisplay=WID(drawingareaBitmap);
    smallDisplay=WID(drawingareaSmall);
    
    CONNECT(drawingareaBitmap,expose_event,gui_draw);
    CONNECT(drawingareaSmall,expose_event,gui_draw_small);

    CONNECT(entry,activate,cb_accept);
_again:    
    reply=setup();
    if(reply==ReplyClose) goto endIt;
    
    printf("Go go go\n");
    
    // Everything ready go go go 
         
    redraw_x=redraw_y=0;
    UI_purge();
//  Time to go
    // Inactivate frame1=glyph    frame2=in/out  buttonStart
    gtk_widget_set_sensitive(WID(buttonStart),0);
    gtk_widget_set_sensitive(WID(frameGlyph),0);
    gtk_widget_set_sensitive(WID(frameLoad),0);
    gtk_widget_set_sensitive(WID(frameBitmap),1);

    gtk_widget_set_sensitive(WID(buttonStart),0);
   
  
   char *fileout;
   fileout=(char *)gtk_label_get_text(GTK_LABEL(WID(labelSrt)));
   if(!fileout)
    {
      GUI_Error_HIG(_("Incorrect output file"), NULL);
        goto _again;
    }
    out=fopen(fileout,"wb");
    if(!out)
    {
      GUI_Error_HIG(_("Output file error"), _("Could not open \"%s\" for writing."), fileout);
        goto _again;
    }
     
    vobsub=new ADMVideoVobSub(subparam.subname,subparam.index);
    nbSub=vobsub->getNbImage();
   
    if(!nbSub)
    {
      GUI_Error_HIG(_("Problem loading sub"), NULL);
        delete vobsub;
        vobsub=NULL;
        goto _again;
     }
    seqNum=1;   // Sub number in srt file
    oldw=oldh=0;

    //******************    
    // Load all bitmaps
    //******************
    for(uint32_t i=0;i<nbSub;i++)
    {
            first=last=0;
            bitmap=vobsub->getBitmap(i,&startTime, &endTime,&first,&last);
            ADM_assert(last>=first);
            
            // something ?
            if(!bitmap) continue;
            if(first==last) continue;

            // If the bitmap size changed or does not exist yet...
            if(!workArea || oldbitmapw!=bitmap->_width || oldbitmaph!=bitmap->_height)
            {
              if(workArea) 
              {
                delete [] workArea;
                workArea=NULL; 
              }
              // Workarea is actually bigger than what we use
              workArea=new uint8_t[bitmap->_width*(bitmap->_height)];
              memset(workArea,0,bitmap->_width*(bitmap->_height));
            }
            oldbitmaph=bitmap->_height;
            oldbitmapw=bitmap->_width;
           // 
           w=bitmap->_width;
           h=last-first+1;
           redraw_x=w;
           redraw_y=h;
           //**
           
           // Build
againPlease:
           mergeBitmap(bitmap->_bitmap+first*w, workArea, bitmap->_alphaMask+first*w,  w,   h);
           if(oldw!=w || oldh !=h)
           {                
                UI_purge();  // Force redaw
           }
           // Merge
             UI_purge();
             gui_draw();
             UI_purge();
             // OCR
              reply=ocrBitmap(workArea,w,h,decodedString);
              if(reply==ReplyClose) goto endIt;
              if(reply==ReplyCalibrate)
                {
                        //
                        //printf("TADA!!!!\n");
                        ocrUpdateMinThreshold();
                        goto againPlease;
                }
             
             //
             gtk_label_set_text(GTK_LABEL(WID(labelText)),decodedString);
             fprintf(out,"%d\n",seqNum++);
             uint16_t hh,mm,ss,ms;
             ms2time(startTime, &hh, &mm, &ss, &ms);
             fprintf(out,"%02d:%02d:%02d,%03d --> ",hh,mm,ss,ms);
             ms2time(endTime, &hh, &mm, &ss, &ms);
             fprintf(out,"%02d:%02d:%02d,%03d\n",hh,mm,ss,ms);
             fprintf(out,"%s\n\n",decodedString);
             //             
             oldw=w;
             oldh=h;
             // Update infos
             sprintf(text,"%03d/%03d",i+1,nbSub);
             gtk_label_set_text(GTK_LABEL(WID(labelNbLines)),text);
             sprintf(text,"%03d",nbGlyphs);
             gtk_label_set_text(GTK_LABEL(WID(labelNbGlyphs)),text);
    }

endIt:
    // Final round
    gtk_widget_set_sensitive(WID(frameGlyph),1);
    gtk_widget_set_sensitive(WID(frameLoad),0);
    gtk_widget_set_sensitive(WID(buttonStart),0);  
    gtk_widget_set_sensitive(WID(frameBitmap),0);
   // gtk_widget_set_sensitive(WID(Current_Glyph),0); 
    
    if(nbGlyphs && actionSaveGlyph==gtk_dialog_run(GTK_DIALOG(dialog)))
    {
      ocrSaveGlyph();
    }
    if(vobsub)
        delete vobsub;
    vobsub=NULL;
    if(out) 
        fclose(out);
    out=NULL;
    gtk_unregister_dialog(dialog);
    gtk_widget_destroy(dialog);
    if(head.next)
        destroyGlyphTree(&head);
    head.next=NULL;
    return 1;

}
/**
    \fn ocrSaveGlyph
    \brief bridge to saveGlyph
*/
uint8_t ocrSaveGlyph(void)
{
  char *name=NULL;
          GUI_FileSelWrite(_("Select Glyphfile to save to"), &name);
        if(!name)
            return 0;
        saveGlyph(name,&head,nbGlyphs);
        ADM_dealloc(name);
        return 1;

}

/**
        Search throught the existing glyphs , if not present create it
        and append the text to decodedString
*/
ReplyType glyphToText(admGlyph *glyph)
{
 admGlyph *cand;
            //printf("2t: %d x %d\n",glyph->width,glyph->height);
            if(glyph->width<2 && glyph->height<2)
            {
                delete glyph;
                return ReplyOk;
            }
            cand=searchGlyph(&head,glyph);
            if(!cand) // New glyph
            {
                char *string;
                // Draw it
                displaySmall(glyph); 
                gtk_label_set_text(GTK_LABEL(WID(labelText)),decodedString);
                gtk_editable_delete_text(GTK_EDITABLE(WID(entry)), 0,-1);
                
                //gtk_widget_set_sensitive(WID(buttonAccept),1);
                //gtk_widget_set_sensitive(WID(buttonSkip),1);
                //gtk_widget_set_sensitive(WID(entryEntry),1);
                
                gtk_widget_grab_focus (WID(entry));
                gtk_widget_grab_default (WID(buttonOk));
                
                //printf("i\n");
                switch(gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                case actionIgnore:
                        glyph->code=NULL;
                        insertInGlyphTree(&head,glyph);
                        nbGlyphs++;
                        break;
                case actionCalibrate: return ReplyCalibrate;
                case actionAccept:
                    string =gtk_editable_get_chars(GTK_EDITABLE (WID(entry)), 0, -1);
                    if(string&& strlen(string))
                    {
                        glyph->code=ADM_strdup(string);
                        insertInGlyphTree(&head,glyph);
                        //printf("New glyph:%s\n",glyph->code);
                        strcat(decodedString,glyph->code);
                        nbGlyphs++;
                       
                    }
                    else delete glyph;
                    break;
                case actionSkip: //SKIP
                    return ReplySkip;
                    break;
                case actionSkipAll:
                    return ReplySkipAll;
                    break;
                case GTK_RESPONSE_CLOSE:
                  if(GUI_Question(_("Sure ?"))) return ReplyClose;
                    break; // Abort
                    
                }
                gtk_editable_delete_text(GTK_EDITABLE(WID(entry)), 0,-1);
                //gtk_widget_set_sensitive(WID(buttonAccept),0);
                //gtk_widget_set_sensitive(WID(buttonSkip),0);
                //gtk_widget_set_sensitive(WID(entryEntry),0);
            }
            else
            {
                //printf("Glyph known :%s \n",cand->code);
                if(cand->code)
                    strcat(decodedString,cand->code);
                delete glyph;
            }
           return ReplyOk;  

}

//*****************************************************************************************
gboolean gui_draw( void )
{
static int lastx=0,lasty=0;
    if(lastx!=redraw_x || lasty!=redraw_y)
        gtk_widget_set_usize(mainDisplay, redraw_x, redraw_y);
    lastx=redraw_x;
    lasty=redraw_y;
    
    gdk_draw_gray_image(mainDisplay->window, mainDisplay->style->fg_gc[GTK_STATE_NORMAL],
                        0,                          // X
                        0,                          // y
                        redraw_x,                          //width
                        redraw_y,                          //h*2, // heigth
                        GDK_RGB_DITHER_NONE,
                        workArea, // buffer
                        redraw_x );
    return true;
}
//*****************************************************************************************
 static int sx=0,sy=0,sw=0,sh=0;
 uint8_t *sdata=NULL;
void displaySmall( admGlyph *glyph)
{
    if(sw!=glyph->width || sh!=glyph->height)
    {
        if(sdata) delete [] sdata;
        sdata=NULL;
        sw=glyph->width;
        sh=glyph->height;
        sdata=new uint8_t[(sw*2+2)*(sh*2+2)];
        gtk_widget_set_usize(smallDisplay, sw*2+2, sh*2+2);
    }
    uint32_t stride=sw*2+2;
    uint8_t *in=glyph->data;
    uint8_t *out=sdata;
    
    memset(out,0,stride);
    out+=stride;
    for(uint32_t y=0;y<sh;y++)
    {
      *(out++)=0;
      for(uint32_t x=0;x<sw;x++)
      {
        out[1]=out[0]=out[stride]=out[stride+1]=*in;
        out+=2; 
        in++;
        
      } 
      *(out++)=0;
      out+=stride;      
    }
    memset(out,0,stride);
    //memcpy(sdata,glyph->data,sw*sh);
    gui_draw_small();
} 

gboolean gui_draw_small(void)
{ 
 if(sw && sh && sdata)
    gdk_draw_gray_image(smallDisplay->window, smallDisplay->style->fg_gc[GTK_STATE_NORMAL],
                        0,                          // X
                        0,                          // y
                        sw*2+2,                          //width
                        sh*2+2,                          //h*2, // heigth
                        GDK_RGB_DITHER_NONE,
                        sdata, // buffer
                        sw*2+2 );
    return true;
}

/******************************************************************************************
 Setup (input/output files etc..)
*****************************************************************************************/
ReplyType setup(void)
{
int sel;
char text[1024];
    while(1)
    {
    //gtk_widget_set_sensitive(WID(buttonAccept),0);
    //gtk_widget_set_sensitive(WID(buttonSkip),0);
    //gtk_widget_set_sensitive(WID(entryEntry),0);
    UI_purge();
    // Main loop : Only accept glyph load/save
    // Sub & srt select & start ocr
    gtk_widget_set_sensitive(WID(frameGlyph),1);
    gtk_widget_set_sensitive(WID(frameLoad),1);
    gtk_widget_set_sensitive(WID(buttonStart),1);
    
    gtk_widget_set_sensitive(WID(frameBitmap),0);
    //gtk_widget_set_sensitive(WID(Current_Glyph),0); 
     switch(sel=gtk_dialog_run(GTK_DIALOG(dialog)))
     {
        case actionLoadVob:
                {
                   
                        subparam.index=lang_index;
                        subparam.subname=NULL;
                        if(DIA_vobsub(&subparam))
                        {
                            lang_index=subparam.index;
                            gtk_label_set_text(GTK_LABEL(WID(labelVobsub)),subparam.subname);
                        }
                        
                    
                }
                break;
        case actionSaveSub:
                {
                    char *srt=NULL;
                    GUI_FileSelWrite(_("Select SRT to save"), &srt);
                    if(srt)
                    {
                        gtk_label_set_text(GTK_LABEL(WID(labelSrt)),srt);
                    }
                }
                break;
        
        case actionLoadGlyph:
            {
                 char *gly=NULL;
                    
                    GUI_FileSelRead(_("Select Glyoh to save"), &gly);
                    if(gly)
                    {
                            loadGlyph(gly,&head,&nbGlyphs);
                            sprintf(text,"%03d",nbGlyphs);
                            gtk_label_set_text(GTK_LABEL(WID(labelNbGlyphs)),text);
                    }
            }
                break;
        
        case actionSaveGlyph:
            
                    if(!nbGlyphs)
                    {
                      GUI_Error_HIG(_("No glyphs to save"), NULL);
                        break;
                    }                  
                    ocrSaveGlyph();
                        break;
        
        case GTK_RESPONSE_CLOSE: 
            printf("Close req\n");
            return ReplyClose;
        default:
            printf("Other input:%d\n",sel);
     }
    // Everything selected, check
    if(sel==actionGo) return ReplyOk;
    }
}
/**
    \fn cb_accept
    \brief Bridge between dialog/Accept and gtk signals
*/
int cb_accept(GtkObject * object, gpointer user_data)
{
        //printf("Hopla\n");
        gtk_signal_emit_by_name(GTK_OBJECT(WID(buttonOk)),"clicked",(gpointer)1);
        return 0;
}

//;
