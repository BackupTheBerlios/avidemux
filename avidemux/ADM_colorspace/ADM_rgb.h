/***************************************************************************
                         ADM_Rgb : wrapper for yv12->RGB display
                            using mplayer postproc


    begin                : Thu Apr 16 2003
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
 #ifndef COLYUVRGB_H
 #define COLYUVRGB_H

#define ADM_COLOR_IS_YUV 0x1000
 typedef enum 
 {
    ADM_COLOR_RGB24,
    ADM_COLOR_BGR24,
    ADM_COLOR_BGR32A,
    ADM_COLOR_RGB32A,
    ADM_COLOR_RGB16,
    ADM_COLOR_RGB555,
    ADM_COLOR_BGR555,
    ADM_COLOR_YV12 = ADM_COLOR_IS_YUV,
    ADM_COLOR_YUV422,
    ADM_COLOR_YUV411
 } ADM_colorspace;
#define ADM_COLOR_BACKWARD 0x8000
#define ADM_COLOR_MASK     0x7FFF
 /* Convert YV12 to RGB32, the reset must be called at least once before using scale */
 class ColBase
 {
  protected:
    void        *_context;
    uint32_t    w,h;  
    uint8_t     clean(void);
     
  public:
                ColBase(uint32_t w, uint32_t h);
                ~ColBase();
     virtual   uint8_t reset(uint32_t neww, uint32_t newh) {return 0;};
     virtual   uint8_t scale(uint8_t *src, uint8_t *target){return 0;};  
 };
 //************ YV12 to RB32********************************
 class ColYuvRgb : public ColBase
 {
  protected:
              uint32_t _inverted;
  public:
                ColYuvRgb(uint32_t w, uint32_t h,uint32_t inv=0): ColBase(w,h) {_inverted=inv;};
                ~ColYuvRgb(){clean();};
      virtual  uint8_t reset(uint32_t neww, uint32_t newh);
      virtual  uint8_t scale(uint8_t *src, uint8_t *target);
      virtual  uint8_t scale(uint8_t *src, uint8_t *target,uint32_t startx,uint32_t starty, uint32_t w,uint32_t h,uint32_t totalW,uint32_t totalH);    
 };
 //********************************************

  /* Convert RGB24/32 to YV12, the reset must be called at least once before using scale */
  class ColRgbToYV12  : public ColBase
  {
  protected:
                int             _bmpMode;
                ADM_colorspace  _colorspace;
                uint32_t        _backward;
  public:
                ColRgbToYV12(uint32_t w, uint32_t h,ADM_colorspace col) : ColBase(w,h) 
                    {
                            _colorspace=col;
                            _bmpMode=0;
                    };
                ~ColRgbToYV12(){clean();};
      virtual  uint8_t reset(uint32_t neww, uint32_t newh);
      virtual  uint8_t scale(uint8_t *src, uint8_t *target);  
               uint8_t changeColorSpace(ADM_colorspace col);   
               uint8_t setBmpMode(void);
      
  };
//********************************************
/* Convert RGB24 to YV12, the reset must be called at least once before using scale */
  class ColYv12Rgb24  : public ColBase
  {
  protected:
    
  public:
                ColYv12Rgb24(uint32_t w, uint32_t h) : ColBase(w,h) {};
                ~ColYv12Rgb24(){clean();};
      virtual  uint8_t reset(uint32_t neww, uint32_t newh);
      virtual  uint8_t scale(uint8_t *src, uint8_t *target);     
      
  };
//*************************************
class COL_Generic2YV12 
{
protected:
                void        *_context;
                uint32_t    w,h;  
                uint8_t     clean(void);
                ADM_colorspace _colorspace;
                uint32_t       _backward;
    
public:
        
                COL_Generic2YV12(uint32_t w, uint32_t h,ADM_colorspace) ;
                ~COL_Generic2YV12(){clean();};
                uint8_t transform(uint8_t **planes, uint32_t *strides,uint8_t *target);
                
};
 #endif
