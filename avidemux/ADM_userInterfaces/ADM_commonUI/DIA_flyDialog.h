/****************************************************************************
 copyright            : (C) 2007 by mean
 email                : fixounet@free.fr
 
 Simple class to do filter that have a configuration that have real time effect
 
 Case 1: The filter process YUV
 
 YUV-> Process->YUVOUT->YUV2RGB->RGBUFFEROUT
 
 Case 2: The filter process RGV
 
 YUV-> YUV2RGB->RGB->Process->RGBUFFEROUT
 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef ADM_FLY_DIALOG_H
#define ADM_FLY_DIALOG_H
enum ResizeMethod {
    RESIZE_NONE = 0,	// No automatic resize
	RESIZE_AUTO = 1,	// Resize image when convenient (YUV: after filter, RGB: before applying filter)
	RESIZE_LAST = 2		// Resize image after filter has been applied (slower for RGB)
};

class diaMenuEntry;  // defined in DIA_factory.h; only need pointer here

struct MenuMapping
{
    const char * widgetName; // name of the combo box widget or equivalent
    uint32_t paramOffset;   // offsetof(FOO_PARAM, menu_option_member)
    uint32_t count;
    const diaMenuEntry * menu;
};

typedef float gfloat;

class ADM_flyDialog
{
  protected:
          uint32_t _w,_h, _zoomW, _zoomH;
          float _zoom;
          uint32_t _zoomChangeCount;
          AVDMGenericVideoStream *_in;
  
          ADMImage *_yuvBuffer;
          ADMImage *_yuvBufferOut;
          uint8_t  *_rgbBuffer;
          uint8_t  *_rgbBufferOut;
		  uint8_t  *_rgbBufferDisplay;
          uint8_t  _isYuvProcessing;
		  ResizeMethod _resizeMethod;
		  ADMImageResizer *_resizer;

  virtual void postInit(uint8_t reInit);
		  float calcZoomFactor(void);
          void copyYuvFinalToRgb(void);
          void copyYuvScratchToRgb(void);
		  void copyRgbFinalToDisplay(void);
          void setupMenus (const void * params,
                           const MenuMapping * menu_mapping,
                           uint32_t menu_mapping_count);
          void getMenuValues (void * params,
                              const MenuMapping * menu_mapping,
                              uint32_t menu_mapping_count);
  public:
          void recomputeSize(void);
          uint32_t getMenuValue (const MenuMapping * mm);
          const MenuMapping * lookupMenu (const char * widgetName,
                                          const MenuMapping * menu_mapping,
                                          uint32_t menu_mapping_count);
  public:
          void    *_cookie; // whatever
          void    *_slider; // widget
          void    *_canvas; // Drawing zone
          ColYuvRgb *_rgb;

          /* Filter dependant */
  virtual uint8_t    process(void)=0;
  virtual uint8_t    download(void)=0;
  virtual uint8_t    upload(void)=0;
          /* /filter dependant */
  
  virtual uint8_t    sliderChanged(void);
        /* This is GTK/QT/whatever dependant */
          
          uint8_t  display(void);
          uint32_t sliderGet(void);
          uint8_t  sliderSet(uint32_t value);
          uint8_t  isRgbInverted(void);
  virtual uint8_t  update(void) {};
          uint8_t  cleanup(void);
  virtual uint8_t  cleanup2(void);

#ifdef USE_JOG
  static void jogDial (void * my_data, signed short offset);
  static void jogRing (void * my_data, gfloat angle);
#endif

          ADM_flyDialog(uint32_t width, uint32_t height, AVDMGenericVideoStream *in,
                             void *canvas, void *slider, int yuv, ResizeMethod resizeMethod);
  virtual ~ADM_flyDialog(void);
};

#endif
