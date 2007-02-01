/***************************************************************************
    
    Handle preview mode
    
    It is displayed in 3 layers
    
    
    Engine : Call setPreviewMode and amdPreview depending on the actions
            previewMode is the **current** preview mode
    
    admPreview : 
          Allocate/desallocate ressources
          Build preview window
          Call display properly to display it
          
    GUI_PreviewXXXX
          UI toolkit to actually display the window
          See GUI_ui.h to see them
                 
    
    
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <ADM_assert.h>
#include "config.h"

#include "default.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"


#include "GUI_render.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_PREVIEW
#include "ADM_osSupport/ADM_debug.h"


#include "GUI_ui.h"
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )

extern FILTER  videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;

static   AVDMGenericVideoStream *preview=NULL;
static ADMImage *previewImage;
static ADM_PREVIEW_MODE previewMode=ADM_PREVIEW_NONE;
static uint32_t _mainW=0,_mainH=0;
static uint32_t _displayW=0,_displayH=0;
/**
      \fn getPreviewMode
      \brief returns current preview mode
      @return current preview mode

*/

ADM_PREVIEW_MODE getPreviewMode(void)
{
  return previewMode; 
}
/**
      \fn setPreviewMode
      \brief set current preview mode an update UI
      @param mode  new preview mode

*/

 void setPreviewMode(ADM_PREVIEW_MODE mode)
{
  previewMode=mode; 
  UI_setCurrentPreview( (int)mode);
}


/**
      \fn admPreview::start
      \brief start preview

*/

void 	admPreview::start( void )
{
            aprintf("--killing\n");
            getFirstVideoFilter();
            
            preview=videofilters[  nb_active_filter-1].filter;
            aprintf("--spawning\n");
            ADM_assert(!previewImage)
                
            previewImage=new ADMImage(preview->getInfo()->width,preview->getInfo()->height);
            
            
            switch(previewMode)
            {
              case  ADM_PREVIEW_SEPARATE:
                  GUI_PreviewInit(preview->getInfo()->width,preview->getInfo()->height,0);
                  
                  /* no break here, not a mistake */
              case  ADM_PREVIEW_NONE:
              
                  _displayW=_mainW;
                  _displayH=_mainH;
                  break;
              case  ADM_PREVIEW_OUTPUT:
                  _displayW=preview->getInfo()->width;
                  _displayH=preview->getInfo()->height;
                  break;
              case  ADM_PREVIEW_SIDE:
              {
                  
                  _displayH=MAX(_mainH,preview->getInfo()->height);
                  _displayW=_mainW+preview->getInfo()->width;
                  break;
              }
              case  ADM_PREVIEW_TOP:
              {
                  
                  _displayW=MAX(_mainW,preview->getInfo()->width);
                  _displayH=_mainH+preview->getInfo()->height;
                  break;
              }
              default: ADM_assert(0);
            }
            renderResize(_displayW,_displayH,ZOOM_1_1);
}
/**
      \fn admPreview::stop
      \brief kill preview window and associated datas
*/

void admPreview::stop( void )
{
      if(previewMode==ADM_PREVIEW_SEPARATE)
                GUI_PreviewEnd();
      if(previewMode!=ADM_PREVIEW_NONE)
      {
          if(previewImage)
            delete previewImage;
          previewImage=NULL;
      }
      renderResize(_mainW,_mainH,ZOOM_1_1);
}
/**
      \fn admPreview::setMainDimension
      \brief Update width & height of input video
      @param w : width
      @param h : height
*/

void admPreview::setMainDimension(uint32_t w, uint32_t h)
{
  _mainW=w;
  _mainH=h;
  _displayW=w;
  _displayH=h;
  renderResize(w,h,ZOOM_1_1);
  
}
/**
      \fn admPreview::update
      \brief display data associated with framenum image
      @param image : current main image (input)
      @param framenum, framenumber
*/

void admPreview::update(uint32_t framenum,ADMImage *image)
{
    uint32_t fl,len;	

    switch(previewMode)
    {
      case ADM_PREVIEW_NONE:
        if(image) renderUpdateImage(image->data);
        break;
      case ADM_PREVIEW_OUTPUT:
            if(framenum<=preview->getInfo()->nb_frames-1)
                  {
                          preview->getFrameNumberNoAlloc(framenum,&len,previewImage,&fl);
                          renderUpdateImage(previewImage->data);
                  }
            break;
      case ADM_PREVIEW_SEPARATE:
            ADM_assert(preview);
            ADM_assert(previewImage);

          if(image) renderUpdateImage(image->data);
          if( GUI_PreviewStillAlive())
          {
                  aprintf("Preview: Ask for frame %lu\n",framenum);
                  if(framenum<=preview->getInfo()->nb_frames-1)
                  {
                          preview->getFrameNumberNoAlloc(framenum,&len,previewImage,&fl);
                          GUI_PreviewUpdate(previewImage->data);
                  }
          }
          break;
      case ADM_PREVIEW_SIDE:
              ADM_assert(preview);
              ADM_assert(previewImage);
  
              renderUpdateImageBlit(image->data,0,0,_mainW,_mainH); // Main
              preview->getFrameNumberNoAlloc(framenum,&len,previewImage,&fl);
              renderUpdateImageBlit(previewImage->data,_mainW,0,previewImage->_width,previewImage->_height);
              break;
        
      case ADM_PREVIEW_TOP:
              ADM_assert(preview);
              ADM_assert(previewImage);
  
              renderUpdateImageBlit(image->data,0,0,_mainW,_mainH); // Main
              preview->getFrameNumberNoAlloc(framenum,&len,previewImage,&fl);
              renderUpdateImageBlit(previewImage->data,0,_mainH,previewImage->_width,previewImage->_height);
              break;
      default: ADM_assert(0);
    }
}
