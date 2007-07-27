/**/
/***************************************************************************
                          DIA_hue
                             -------------------

                           Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/5 by mean
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



#include <config.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "default.h"


#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"

#include "ADM_video/ADM_vobsubinfo.h"
#include "ADM_video/ADM_vidVobSub.h"
#include "ADM_ocr/ADM_ocr.h"
#include "ADM_osSupport/ADM_misc.h"
#include "ADM_toolkit/toolkit.hxx"
#include "prefs.h"
#include "ADM_assert.h"

#include "ADM_ocr/ADM_ocr.h"

extern uint8_t DIA_vobsub(vobSubParam *param);

static void cb_idx(void *foo);
static void cleanupSub(vobSubParam *p);
/**
    \fn DIA_ocrGen
    \brief Dialog to select input & output files before calling the actual ocr engine
*/
uint8_t DIA_ocrGen(void)
{

  vobSubParam subparam={NULL,0,0};
  char *srtFileName=NULL;
  char *glyphFileName=NULL;
  admGlyph head(16,16);
  char *globalGlyph=NULL;
  uint32_t globalGlyphOn=0;
  prefs->get(FEATURE_GLOBAL_GLYPH_ACTIVE,&globalGlyphOn);
  if(globalGlyphOn)
  {
     prefs->get(FEATURE_GLOBAL_GLYPH_NAME,&globalGlyph);
     if(!*globalGlyph)
     {
        ADM_dezalloc(globalGlyph);
        globalGlyph=NULL; 
     }
  }

  if(globalGlyph)
  {
    glyphFileName=globalGlyph;
  }
_again:  
  // Fist build a dialogFactory to get input and output files
  diaElemButton   selectIdx(_("Select idx file:"), cb_idx,&subparam,NULL);
  diaElemFile     selectGlyph(1,&glyphFileName,_("Use glyphset (optional):"),NULL);  
  diaElemFile     selectSrt(1,&srtFileName,_("Output SRT file"),NULL);
  
  diaElem *elems[]={&selectIdx,&selectSrt,&selectGlyph};
  
  
   uint32_t n=3;
   if(globalGlyph)
   {
     n--; // Remove glyph from dialog
   }
  
        if( !diaFactoryRun(_("Select input and ouput files"),n,elems))
        {
          cleanupSub(&subparam);
          if(srtFileName )ADM_dezalloc(srtFileName);
          srtFileName=NULL;
          destroyGlyphTree(&head);
          return 0;
        }
        if(!ADM_fileExist(subparam.subname))
        {
          GUI_Error_HIG(_("File error"),_("The idx/sub file does not exist."));
          goto _again; 
        }
        if(!srtFileName || !*srtFileName)
        {
          GUI_Error_HIG(_("File error"),_("Please Select a valid output SRT file."));
          goto _again; 
        }
         if(glyphFileName && *glyphFileName)
         {
           if(!ADM_fileExist(glyphFileName))
            {
              GUI_Error_HIG(_("File error"),_("The idx/sub file does not exist."));
              goto _again; 
            }
            // Purge previous glyph set if any
            destroyGlyphTree(&head);
            uint32_t nb;
            printf("[OCR] Loading glyphset :<%s>\n",glyphFileName);
            if(!loadGlyph(glyphFileName,&head,&nb))
            {
              GUI_Error_HIG(_("File error"),_("Cannot load the glyphset file."));
              goto _again;               
            }
            printf("[GLYPH] Found %u glyph\n");
         }
        // We have our SRT and our idx/sub files : Go go go
         ADM_OCR_SOURCE source;
         source.type=ADM_OCR_TYPE_VOBSUB;
         source.subparam=&subparam;
        ADM_ocr_engine(source,srtFileName,&head);
        
        // Save glyph set 
        if(globalGlyph)
        {
          uint32_t nb=1;
           saveGlyph(globalGlyph,&head,nb);
        }else
        {
            char *save=NULL;
            uint32_t nb=1;
              diaElemFile     selectSave(1,&save,_("Glyphset filename"),NULL);
              diaElem *elems2[]={&selectSave};
            if( diaFactoryRun(_("Save Glyph"),1,elems2))
            {
              saveGlyph(save,&head,nb);
            }
            if(save) ADM_dezalloc(save);
        }

  cleanupSub(&subparam);
  if(srtFileName )ADM_dezalloc(srtFileName);
  srtFileName=NULL;
  destroyGlyphTree(&head);
  return 1;  
}
/**
    \fn DIA_ocrDvb
    \brief Dialog to select input & output files before calling the actual ocr engine
*/
uint8_t DIA_ocrDvb(void)
{

  vobSubParam subparam={NULL,0,0};
  char *srtFileName=NULL;
  char *glyphFileName=NULL;
  admGlyph head(16,16);
  char *globalGlyph=NULL;
  uint32_t globalGlyphOn=0;
  prefs->get(FEATURE_GLOBAL_GLYPH_ACTIVE,&globalGlyphOn);
  if(globalGlyphOn)
  {
     prefs->get(FEATURE_GLOBAL_GLYPH_NAME,&globalGlyph);
     if(!*globalGlyph)
     {
        ADM_dezalloc(globalGlyph);
        globalGlyph=NULL; 
     }
  }

  if(globalGlyph)
  {
    glyphFileName=globalGlyph;
  }
_againX:  
  // Fist build a dialogFactory to get input and output files
  diaElemFile     selectGlyph(1,&glyphFileName,_("Use glyphset (optional):"),NULL);  
  diaElemFile     selectSrt(1,&srtFileName,_("Output SRT file"),NULL);
  
  diaElem *elems[]={&selectSrt,&selectGlyph};
  
  
   uint32_t n=2;
   if(globalGlyph)
   {
     n--; // Remove glyph from dialog
   }
  
        if( !diaFactoryRun(_("Select input and ouput files"),n,elems))
        {
          cleanupSub(&subparam);
          if(srtFileName )ADM_dezalloc(srtFileName);
          srtFileName=NULL;
          destroyGlyphTree(&head);
          return 0;
        }
       // TODO Check input TS file
        if(!srtFileName || !*srtFileName)
        {
          GUI_Error_HIG(_("File error"),_("Please Select a valid output SRT file."));
          goto _againX; 
        }
         if(glyphFileName && *glyphFileName)
         {
           if(!ADM_fileExist(glyphFileName))
            {
              GUI_Error_HIG(_("File error"),_("The idx/sub file does not exist."));
              goto _againX; 
            }
            // Purge previous glyph set if any
            destroyGlyphTree(&head);
            uint32_t nb;
            printf("[OCR] Loading glyphset :<%s>\n",glyphFileName);
            if(!loadGlyph(glyphFileName,&head,&nb))
            {
              GUI_Error_HIG(_("File error"),_("Cannot load the glyphset file."));
              goto _againX;               
            }
            printf("[GLYPH] Found %u glyph\n");
         }
        // We have our SRT and our idx/sub files : Go go go
         ADM_OCR_SOURCE source;
         source.type=ADM_OCR_TYPE_TS;
         source.TsFile=NULL;
         ADM_ocr_engine(source,srtFileName,&head);
        
        // Save glyph set 

        if(globalGlyph)
        {
          uint32_t nb=1;
           saveGlyph(globalGlyph,&head,nb);
        }else
        {
            char *save=NULL;
            uint32_t nb=1;
              diaElemFile     selectSave(1,&save,_("Glyphset filename"),NULL);
              diaElem *elems2[]={&selectSave};
            if( diaFactoryRun(_("Save Glyph"),1,elems2))
            {
              saveGlyph(save,&head,nb);
            }
            if(save) ADM_dezalloc(save);
        }

  cleanupSub(&subparam);
  if(srtFileName )ADM_dezalloc(srtFileName);
  srtFileName=NULL;
  destroyGlyphTree(&head);
  return 1;  
}
/**

*/
void cleanupSub(vobSubParam *p)
{
  if(p->subname) ADM_dezalloc(p->subname);
  p->subname=NULL; 
  
}
/**

*/
void cb_idx(void *foo)
{
   vobSubParam *bar=(vobSubParam *)foo;
   
    cleanupSub(bar);
    DIA_vobsub(bar);
}
//EOF
