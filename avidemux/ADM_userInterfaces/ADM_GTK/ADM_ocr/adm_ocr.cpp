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
#include "ADM_ocrInternal.h"
/******************************/

#define TESTSUB "/home/fx/usbstick/subs/vts_01_0.idx"
#define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y,GTK_SIGNAL_FUNC(z),   NULL);


/* Minimal size of the current glyph display window */
#define MAX_W 65
#define MAX_H 65


extern  uint8_t DIA_vobsub(vobSubParam *param);
//********************************************
extern  GtkWidget *DIA_ocr(void);
static gboolean   gui_draw( void );
static gboolean   gui_draw_small( void );
static void       displaySmall( admGlyph *glyph );
static int        cb_accept(GtkObject * object, gpointer user_data);

static GtkWidget *dialog=NULL;
static GtkWidget *mainDisplay=NULL;
static GtkWidget *smallDisplay=NULL;
static uint32_t redraw_x,redraw_y;
static uint32_t clipW=0,clipH=0;
//********************************************
//ReplyType glyphToText(admGlyph *glyph,admGlyph *head);
static ReplyType setup(void);
//********************************************
static uint8_t ocrSaveGlyph(admGlyph *head);

static uint8_t *workArea;
static vobSubBitmap *bitmap;
static uint8_t *sdata=NULL;
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
   
uint8_t ADM_ocr_engine(   ADM_OCR_SOURCE & source,const char *labelSrt,admGlyph *head)
{
// 
    uint32_t nbSub=0;
    FILE *out=NULL;
    ADM_BitmapSource *bitmapSource=NULL;
    uint32_t startTime,endTime;
    uint32_t w,h,oldw=0,oldh=0;
    uint32_t oldbitmapw=0;
    uint32_t oldbitmaph=0;
    uint32_t first,last;
    uint32_t seqNum;
    char     text[1024];
    nbGlyphs=0;
    ReplyType reply;
    
    dialog=NULL;
    mainDisplay=NULL;
    smallDisplay=NULL;
    workArea=NULL;
    bitmap=NULL;
    sdata=NULL;
    clipW=0;
    clipH=0;

    
// Create UI && prepare callback
    
    dialog=DIA_ocr();
    gtk_register_dialog(dialog);
#define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)
    
    ASSOCIATE(buttonOk,   actionAccept);
    ASSOCIATE(buttonSkip,     actionSkip);
    ASSOCIATE(buttonSkipAll,     actionSkipAll);
    ASSOCIATE(buttonIgnore,   actionIgnore);
    ASSOCIATE(buttonCalibrate,   actionCalibrate);
    
   
    gtk_widget_show(dialog);
    //
    
//  disable
    
    mainDisplay=WID(drawingareaBitmap);
    smallDisplay=WID(drawingareaSmall);
    
   
    
    CONNECT(drawingareaBitmap,expose_event,gui_draw);
    CONNECT(drawingareaSmall,expose_event,gui_draw_small);

    CONNECT(entry,activate,cb_accept);
_again:    
    
    printf("Go go go\n");
    
    // Everything ready go go go 
         
    redraw_x=redraw_y=0;
    UI_purge();
//  Time to go
    
    gtk_widget_set_sensitive(WID(frameBitmap),1);
 
    out=fopen(labelSrt,"wb");
    if(!out)
    {
       GUI_Error_HIG(_("Output file error"), _("Could not open \"%s\" for writing."), labelSrt);
       goto endIt;
    }
    bitmapSource=ADM_buildBitmapSource(&source);
    if(!bitmapSource)
    {
    	goto _again;
    }
    if(!bitmapSource->init(&source))
    {
    		printf("[OCR] Bitmap source failed\n");
    		goto _again;
    }
    
    nbSub=bitmapSource->getNbImages();
   
    if(!nbSub)
    {
      GUI_Error_HIG(_("Problem loading sub"), NULL);
        delete bitmapSource;
        bitmapSource=NULL;
        goto _again;
     }
    seqNum=1;   // Sub number in srt file
    oldw=oldh=0;
    uint32_t eos;
    //******************    
    // Load all bitmaps
    //******************
    for(uint32_t i=0;i<nbSub;i++)
    {
            first=last=0;
            bitmap=bitmapSource->getBitmap(i,&startTime, &endTime,&first,&last,&eos);
            if(eos) break;
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
              reply=ocrBitmap(workArea,w,h,decodedString,head);
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
             
             // Update Timecode
             char timeCode[100];
             snprintf(timeCode,100,"%02d:%02d:%02d,%03d",hh,mm,ss,ms);
             gtk_label_set_text(GTK_LABEL(WID(labelTime)),timeCode);
             
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
    gtk_widget_set_sensitive(WID(frameBitmap),0);
   // gtk_widget_set_sensitive(WID(Current_Glyph),0);     
    if(bitmapSource)
        delete bitmapSource;
    bitmapSource=NULL;
    if(out) 
        fclose(out);
    out=NULL;
    gtk_unregister_dialog(dialog);
    gtk_widget_destroy(dialog);

    return 1;

}
/**
    \fn ocrSaveGlyph
    \brief bridge to saveGlyph
*/
uint8_t ocrSaveGlyph(admGlyph *head)
{
  char *name=NULL;
          GUI_FileSelWrite(_("Select Glyphfile to save to"), &name);
        if(!name)
            return 0;
        saveGlyph(name,head,nbGlyphs);
        ADM_dealloc(name);
        return 1;

}

/**
        Search throught the existing glyphs , if not present create it
        and append the text to decodedString
*/
ReplyType glyphToText(admGlyph *glyph,admGlyph *head)
{
 admGlyph *cand;
            //printf("2t: %d x %d\n",glyph->width,glyph->height);
            if(glyph->width<2 && glyph->height<2)
            {
                delete glyph;
                return ReplyOk;
            }
            cand=searchGlyph(head,glyph);
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
                        insertInGlyphTree(head,glyph);
                        nbGlyphs++;
                        break;
                case actionCalibrate: return ReplyCalibrate;
                case actionAccept:
                    string =gtk_editable_get_chars(GTK_EDITABLE (WID(entry)), 0, -1);
                    if(string&& strlen(string))
                    {
                        glyph->code=ADM_strdup(string);
                        insertInGlyphTree(head,glyph);
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

void displaySmall( admGlyph *glyph)
{
    if(sw!=glyph->width || sh!=glyph->height)
    {
    	
        if(sdata) delete [] sdata;
        sdata=NULL;
        sw=glyph->width;
        sh=glyph->height;
        sdata=new uint8_t[(sw*2+2)*(sh*2+2)];
        clipW= sw*2+2;
        clipH=  sh*2+2;
#ifndef MAX
        #define MAX(a,b) a>b?a:b
#endif
        clipW=MAX(MAX_W,clipW);
        clipH=MAX(MAX_H,clipH);
        gtk_widget_set_usize(smallDisplay, clipW,clipH);
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
	if(clipW && clipH)
	{
		 gdk_draw_rectangle(smallDisplay->window,
				 				smallDisplay->style->fg_gc[GTK_STATE_NORMAL],
				 				1, // Filled
		                        0,                          // X
		                        0,                          // y
		                        clipW,
		                        clipH);
		
	
	}
 if(sw && sh && sdata)
 {
    gdk_draw_gray_image(smallDisplay->window, smallDisplay->style->fg_gc[GTK_STATE_NORMAL],
                        1,                          // X
                        1,                          // y
                        sw*2+1,                          //width
                        sh*2+1,                          //h*2, // heigth
                        GDK_RGB_DITHER_NONE,
                        sdata, // buffer
                        sw*2+2 );
 }
    return true;
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
