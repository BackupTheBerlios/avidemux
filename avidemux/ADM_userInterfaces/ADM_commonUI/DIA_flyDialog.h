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
class ADM_flyDialog
{
  protected:
          uint32_t _w,_h, _zoomW, _zoomH;
          AVDMGenericVideoStream *_in;
  
          ADMImage *_yuvBuffer;
          ADMImage *_yuvBufferOut;
          uint8_t  *_rgbBuffer;
          uint8_t  *_rgbBufferOut;
		  uint8_t  *_rgbBufferDisplay;
          uint8_t  _isYuvProcessing;

		  ADMImageResizer *_resizer;
  public:
          void    *_cookie; // whatever
          void    *_slider; // widget
          void    *_canvas; // Drawing zone
          ColYuvRgb *_rgb;
          /* Filter dependant */
  virtual uint8_t    process(void)=0;
  virtual uint8_t    download(void)=0;
  virtual uint8_t    upload(void)=0;
          /* /filte dependant */
  
  virtual uint8_t    sliderChanged(void);
        /* This is GTK/QT/whatever dependant */
          
          uint8_t  display(void);
          uint32_t sliderGet(void);
          uint8_t  sliderSet(uint32_t value);
          uint8_t  isRgbInverted(void);
  virtual uint8_t  update(void) {};
          uint8_t  cleanup(void);
		  void resizeImage(uint32_t width, uint32_t height);
         /*                               */
                  ADM_flyDialog(uint32_t width,uint32_t height,AVDMGenericVideoStream *in,
                                    void *canvas, void *slider,int yuv);
    virtual      ~ADM_flyDialog(void);
};

#endif
