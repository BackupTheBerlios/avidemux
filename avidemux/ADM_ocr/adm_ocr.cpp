/*
    Very simple OCR code to transform vobsub into srt file
*/
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

#include "unistd.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFlipV.h"
#include "ADM_filter/video_filters.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFlipV.h"
#include "ADM_filter/video_filters.h"

#include "ADM_mpeg2dec/ADM_mpegpacket.h"
#include "ADM_mpeg2dec/ADM_mpegpacket_PS.h"

#include "ADM_toolkit/filesel.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_video/ADM_vobsubinfo.h"

#include "ADM_video/ADM_vidVobSub.h"
#include "ADM_leftturn.h"
/******************************/
#define GTK_PURGE {while (gtk_events_pending())                gtk_main_iteration(); }    
#define TESTSUB "/home/fx/usbstick/subs/vts_01_0.idx"
#define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y,GTK_SIGNAL_FUNC(z),   NULL);
#define SUB_THRESH 0x80
static uint8_t mergeBitmap(uint8_t *bitin, uint8_t *bitout, uint8_t *maskin,uint32_t w, uint32_t h);
static uint8_t ocrBitmap(uint8_t *data,uint32_t w,uint32_t h);
static uint8_t handleGlyph(uint32_t start, uint32_t end,uint32_t w,uint32_t h,uint32_t base);
static uint8_t lineEmpty(uint8_t *base, uint32_t stride, uint32_t width, uint32_t line);
static uint8_t columnEmpty(uint8_t *base, uint32_t stride, uint32_t height);
static uint8_t glyphToText(admGlyph *glyph);
static uint8_t saveGlyph(void);
static uint8_t loadGlyph(char *name);
static void displaySmall( admGlyph *glyph );
static uint8_t setup(void);
static  vobSubParam subparam={NULL,0,0};
/*++++++++++++++++++++++++++++++++++++++++*/
extern  GtkWidget	*DIA_ocr(void);
extern  uint8_t DIA_vobsub(vobSubParam *param);
static gboolean gui_draw( void );
static gboolean gui_draw_small( void );
static char decodedString[1024];
/*-------------------------*/
static GtkWidget *dialog;
static GtkWidget *mainDisplay;
static GtkWidget *smallDisplay;
static vobSubBitmap *bitmap;

static uint8_t *workArea;
static admGlyph head(250,4);
static uint32_t lang_index=0;


static uint32_t redraw_x,redraw_y;

static uint32_t nbGlyphs;


typedef enum
{
    actionLoadVob=10,
    actionSaveSub,
    actionGo,
    actionLoadGlyph,
    actionSaveGlyph,
    actionSkip,
    actionAccept,
    actionIgnore
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
    uint32_t first,last;
    uint32_t seqNum;
    char     text[1024];
    lang_index=0;
    nbGlyphs=0;
    
// Create UI && prepare callback
    
    dialog=DIA_ocr();
    gtk_register_dialog(dialog);
#define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)
    ASSOCIATE(buttonStart,actionGo);
    ASSOCIATE(buttonAccept,   actionAccept);
    ASSOCIATE(buttonSkip,     actionSkip);
    ASSOCIATE(buttonIgnore,   actionIgnore);
    
    ASSOCIATE(buttonLoad,   actionLoadGlyph);
    ASSOCIATE(buttonSave,   actionSaveGlyph);
    
    ASSOCIATE(buttonSelectVob,   actionLoadVob);
    ASSOCIATE(buttonSRT,   actionSaveSub);
   
    gtk_widget_show(dialog);
//  disable
    
    mainDisplay=WID(drawingareaVobSub);
    smallDisplay=WID(drawingareaMini);
    
    CONNECT(drawingareaVobSub,expose_event,gui_draw);
    CONNECT(drawingareaMini,expose_event,gui_draw_small);
_again:    
    if(!setup()) goto endIt;
    printf("Go go go\n");

    // Everything ready go go go 
         
    redraw_x=redraw_y=0;
    GTK_PURGE;
//  Time to go
    // Inactivate frame1=gliph    frame2=in/out  buttonStart
    gtk_widget_set_sensitive(WID(buttonStart),0);
    gtk_widget_set_sensitive(WID(frame1),0);
    gtk_widget_set_sensitive(WID(frame2),0);
    gtk_widget_set_sensitive(WID(buttonStart),0);
    // Activate output
    gtk_widget_set_sensitive(WID(Current_Glyph),1);
    //
  
   char *fileout;
   fileout=(char *)gtk_label_get_text(GTK_LABEL(WID(labelSRT)));
   if(!fileout)
    {
        GUI_Alert("Ouptut file incorrect!");
        goto _again;
    }
    out=fopen(fileout,"wb");
    if(!out)
    {
        GUI_Alert("Ouptut file incorrect!");
        goto _again;
    }
     
    vobsub=new ADMVideoVobSub(subparam.subname,subparam.index);
    nbSub=vobsub->getNbImage();
   
    if(!nbSub)
    {
        GUI_Alert("Problem loading sub!");
        delete vobsub;
        vobsub=NULL;
        goto _again;
     }
    seqNum=1;   // Sub number in srt file
    for(uint32_t i=0;i<nbSub;i++)
    {
            first=last=0;
            bitmap=vobsub->getBitmap(i,&startTime, &endTime,&first,&last);
            // Update display
            if(!bitmap) break;
            if(!workArea)
            {
                workArea=new uint8_t[bitmap->_width*bitmap->_height];
            }
           // 
           w=bitmap->_width;
           h=last-first+1;
           redraw_x=w;
           redraw_y=h;
           //**
           if(oldw!=w || oldh !=h)
           {                
                GTK_PURGE;
           }
           // Build
           mergeBitmap(bitmap->_bitmap+first*w, workArea, bitmap->_alphaMask+first*w,  w,   h);
           // Merge
             GTK_PURGE;
             gui_draw();
             GTK_PURGE; 
             // OCR
             if(!ocrBitmap(workArea,w,h)) goto endIt;
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
             gtk_label_set_text(GTK_LABEL(WID(labelNbGLyph)),text);
    }

endIt:
    // Final round
    gtk_widget_set_sensitive(WID(frame1),1);
    gtk_widget_set_sensitive(WID(frame2),0);
    gtk_widget_set_sensitive(WID(buttonStart),0);  
    gtk_widget_set_sensitive(WID(frame5),0);
    gtk_widget_set_sensitive(WID(Current_Glyph),0); 
    
    if(nbGlyphs && actionSaveGlyph==gtk_dialog_run(GTK_DIALOG(dialog)))
        saveGlyph();
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

//*****************************************************************************************
//  Separate the bitmap into glyph and handle each glyph on itw own
//
//*****************************************************************************************

uint8_t ocrBitmap(uint8_t *data,uint32_t w,uint32_t h)
{
uint8_t found;
uint32_t colstart=0,colend=0,oldcol;
uint32_t line=0,nbLine=1;
uint32_t base=0,bottom,top;    
    // Search First non nul colum
    decodedString[0]=0;
    // Search how much lines there is in the file
    //
    top=bottom=0;
    while(top<h)
    {
        // Search non empty line as top
        while(top<h && lineEmpty(workArea,w,w,top)) top++;
        // Nothing found
        if(top>=h-1) break;
        bottom=top+1;
        // Search empty line if any, bottom is the 1st line full of zero
        while(bottom<h && (!lineEmpty(workArea,w,w,bottom) || bottom-top<7))
        {
            bottom++;
        }
        
        //printf("\n Top:%lu bottom:%lu\n",top,bottom);
       
        // Scan a full line
        colstart=0;
        oldcol=0;
       
        // Split a line into glyphs
        while(colstart<w)
        {
            GTK_PURGE;
            oldcol=colstart;
            while( columnEmpty(workArea+colstart+top*w, w, bottom-top) && colstart<w) colstart++;
            if(colstart>=w) break;
            // if too far apart, it means probably a blank space
            if(colstart-oldcol>6)
            {
                strcat(decodedString," ");
            }
       
            // We have found a non null column
            // Seek the end now
            colend=colstart+1;
            while( !columnEmpty(workArea+colend+top*w, w, bottom-top) && colend<w) colend++;
         
         
            // printf("Found glyph: %lu %lu\n",colstart,colend);  
            
            if(!handleGlyph(colstart,colend,w,bottom,top)) return 0;
            colstart=colend;
      }
      line++;
      if(line<nbLine) strcat(decodedString,"\n");
      top=bottom;
      
    }
   
    return 1;
}
//*****************************************************************************************
/*
    We now have a good candidate for the glyph.
    We will do the following processing :
        - Clip the glyph to have it in its bounding box
        - extract its container. If the container is smaller than the glyph, it means
                that we have in fact several glyphs that overlaps slightly. In
                that case we use another method to extract the glyph.
                We split it using leftturn method and do it again.
*/
uint8_t handleGlyph(uint32_t start, uint32_t end,uint32_t w,uint32_t h,uint32_t base)
{
uint8_t found=0;
static int inc=1;
    
          
    
    // Ok now we have the cropped glyp
    
    admGlyph *glyph,*cand;
    uint32_t minx,maxx,miny,maxy;
    int     *raw=NULL;
            glyph=new admGlyph(end-start,h-base);
            glyph->create(workArea+start+base*w,w);
            glyph=clippedGlyph(glyph);
            if(!glyph->width) // Empty glyph
            {
                delete glyph;
                return 1;
            }
            // now we have our full glyph, try harder to split it
_nextglyph:
            raw=new int[glyph->height];            
            if(adm_estimate_glyphSize(glyph,&minx, &maxx,&miny,&maxy,raw))
            {
            //printf("Glyph width :%lu min:%lu max:%lu estimate width:%lu\n",glyph->width,minx,maxx,maxx-minx+1);
            if((maxx-minx+2)<glyph->width && (maxx-minx>2) && (maxy-miny>2))
            {
                // Suspicously too small
                // We have to split the glyph
                // recursively to extract each glyph
                uint32_t width=maxx-minx+1;
                uint32_t defStride=width+1;
                
                if(defStride>glyph->width) defStride=glyph->width;
                
                admGlyph *lefty=new admGlyph(defStride,glyph->height);
                for(int32_t i=miny;i<=maxy;i++)
                {
                    if(raw[i]!=-1) memcpy(&(lefty->data[0+i*defStride]),&(glyph->data[minx+i*glyph->width]),raw[i]+1-minx);
                    else
                            memcpy(&(lefty->data[0+i*defStride]),&(glyph->data[minx+i*glyph->width]),defStride);
                }
                lefty=clippedGlyph(lefty);
              
                {
                    // Remove that from the original
                    for(uint32_t i=0;i<glyph->height;i++)
                    {
                        //printf("%d:%d(%d)\n",i,raw[i],glyph->width);
                        if(raw[i]!=-1) memset(&(glyph->data[i*glyph->width]),0,raw[i]+1);
                        else           memset(&(glyph->data[i*glyph->width]),0,defStride); 
                    }
                    // Clip
                    glyph=clippedGlyph(glyph);
                
                    if(lefty->width)
                    {
                        if(!glyphToText(lefty)) 
                        {
                            printf("Glyph2text failed(1)\n");
                            return 0;
                        }
                    }
                    else
                        delete lefty;
                    if(glyph->width)
                    {
                        if(raw) delete [] raw;
                        goto _nextglyph;                    
                    } 
                 }           
            }
            }//If
            if(raw) delete [] raw;
            if(glyph->width)
            {
                if(!glyphToText(glyph)) 
                {
                    printf("Glyph2text failed(2)\n");
                    return 0;
                }
            }
            else 
            {
                delete glyph;
            }
            
    return 1;

}
/**
        Search throught the existing glyphs , if not present create it
        and append the text to decodedString
*/
uint8_t glyphToText(admGlyph *glyph)
{
 admGlyph *cand;
            //printf("2t: %d x %d\n",glyph->width,glyph->height);
            if(glyph->width<2 && glyph->height<2)
            {
                delete glyph;
                return 1;
            }
            cand=searchGlyph(&head,glyph);
            if(!cand) // New glyph
            {
                char *string;
                // Draw it
                displaySmall(glyph); 
                gtk_label_set_text(GTK_LABEL(WID(labelText)),decodedString);
                gtk_editable_delete_text(GTK_EDITABLE(WID(entryEntry)), 0,-1);
                
                //gtk_widget_set_sensitive(WID(buttonAccept),1);
                //gtk_widget_set_sensitive(WID(buttonSkip),1);
                //gtk_widget_set_sensitive(WID(entryEntry),1);
                
                gtk_widget_grab_focus (WID(entryEntry));
                gtk_widget_grab_default (WID(buttonAccept));
                
                //printf("i\n");
                switch(gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                case actionIgnore:
                        glyph->code=NULL;
                        insertInGlyphTree(&head,glyph);
                        nbGlyphs++;
                        break;
                case actionAccept:
                    string =gtk_editable_get_chars(GTK_EDITABLE (WID(entryEntry)), 0, -1);
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
                    break;
                case GTK_RESPONSE_CLOSE:
                    if(GUI_Question("Sure ?")) return 0;
                    break; // Abort
                    
                }
                gtk_editable_delete_text(GTK_EDITABLE(WID(entryEntry)), 0,-1);
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
           return 1;  

}
/**************************************************************************************
    returns 1 if the line is empty
    0 if not
*/

//*****************************************************************************************
uint8_t mergeBitmap(uint8_t *bitin, uint8_t *bitout, uint8_t *maskin,uint32_t w, uint32_t h)
{
// Merge with alpha channel
           uint8_t *in,*mask,*out;
           uint32_t alp,nw;
           in=bitin;
           out=bitout;
           mask=maskin;
           for(uint32_t y=0;y<h;y++)
           {
            for(uint32_t x=0;x<w;x++)
            {
                   
                    nw=in[x];
                    alp=mask[x];

                    if(alp) 
                    {
                         if(alp>7&& nw >SUB_THRESH)  nw=0xff;
                         else       nw=0;
                         out[x]=nw;                                         
                    }
                    else out[x]=0;
            }
            out+=w;
            in+=w;
            mask+=w;
           }    
    return 1;
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
        sdata=new uint8_t[(sw)*(sh)];
        gtk_widget_set_usize(smallDisplay, sw+2, sh+2);
    }
    memcpy(sdata,glyph->data,sw*sh);
    gui_draw_small();
} 

gboolean gui_draw_small(void)
{ 
 if(sw && sh && sdata)
    gdk_draw_gray_image(smallDisplay->window, smallDisplay->style->fg_gc[GTK_STATE_NORMAL],
                        1,                          // X
                        1,                          // y
                        sw,                          //width
                        sh,                          //h*2, // heigth
                        GDK_RGB_DITHER_NONE,
                        sdata, // buffer
                        sw );
    return true;
}

/******************************************************************************************
 Setup (input/output files etc..)
*****************************************************************************************/
uint8_t setup(void)
{
int sel;
char text[1024];
    while(1)
    {
    //gtk_widget_set_sensitive(WID(buttonAccept),0);
    //gtk_widget_set_sensitive(WID(buttonSkip),0);
    //gtk_widget_set_sensitive(WID(entryEntry),0);
    GTK_PURGE;   
    // Main loop : Only accept glyph load/save
    // Sub & srt select & start ocr
    gtk_widget_set_sensitive(WID(frame1),1);
    gtk_widget_set_sensitive(WID(frame2),1);
    gtk_widget_set_sensitive(WID(buttonStart),1);
    
    gtk_widget_set_sensitive(WID(frame5),0);
    gtk_widget_set_sensitive(WID(Current_Glyph),0); 
     switch(sel=gtk_dialog_run(GTK_DIALOG(dialog)))
     {
        case actionLoadVob:
                {
                   
                        subparam.index=lang_index;
                        subparam.subname=NULL;
                        if(DIA_vobsub(&subparam))
                        {
                            lang_index=subparam.index;
                            gtk_label_set_text(GTK_LABEL(WID(labelVobSub)),subparam.subname);
                        }
                        
                    
                }
                break;
        case actionSaveSub:
                {
                    char *srt=NULL;
                    GUI_FileSelWrite("Select SRT to save", &srt);
                    if(srt)
                    {
                        gtk_label_set_text(GTK_LABEL(WID(labelSRT)),srt);
                    }
                }
                break;
        
        case actionLoadGlyph:
            {
                 char *gly=NULL;
                    
                    GUI_FileSelRead("Select Glyoh to save", &gly);
                    if(gly)
                    {
                            loadGlyph(gly);
                            sprintf(text,"%03d",nbGlyphs);
                            gtk_label_set_text(GTK_LABEL(WID(labelNbGLyph)),text);
                    }
            }
                break;
        
        case actionSaveGlyph:
            
                    if(!nbGlyphs)
                    {
                        GUI_Alert("No glyphs to save!");
                        break;
                    }                  
                    saveGlyph();                                      
                    break;
        
        case GTK_RESPONSE_CLOSE: 
            printf("Close req\n");
            return 0;
        default:
            printf("Other input:%d\n",sel);
     }
    // Everything selected, check
    if(sel==actionGo) return 1;
    }
}
/**
*/
uint8_t saveGlyph(void)
{
    FILE *out;
    char *name=NULL;
    uint32_t slen;
    
    admGlyph *glyph=head.next;
    
    GUI_FileSelWrite("Select Glyphfile to save to", &name);
    if(!name)
        return 0;
    
    out=fopen(name,"wb");
    ADM_dealloc(name);
    if(!out)
    {
        GUI_Alert("Problem writing");
        return 0;
    }
    #define WRITE(x) fwrite(&(x),sizeof(x),1,out);
    WRITE(nbGlyphs);
    
    while(glyph)
    {
        WRITE(glyph->width);
        WRITE(glyph->height);
        fwrite(glyph->data,glyph->width*glyph->height,1,out);
        if(glyph->code) slen=strlen(glyph->code);
             else slen=0;
        WRITE(slen);
        fwrite(glyph->code,slen,1,out);
        glyph=glyph->next;
    }
    
    fclose(out);
    return 1;

}
/**
*/
uint8_t loadGlyph(char *name)
{
    FILE *out;
    admGlyph *glyph,*nw;
    uint32_t N,w,h,slen;
    
    if(head.next)
    {
        if(!GUI_Question("Erase current glyphs ?"))
            return 0;
        destroyGlyphTree(&head);
        nbGlyphs=0;
    }
    glyph=&head;
    out=fopen(name,"rb");
    if(!out)
    {
        GUI_Alert("Problem reading");
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
    return 1;

}
//;
