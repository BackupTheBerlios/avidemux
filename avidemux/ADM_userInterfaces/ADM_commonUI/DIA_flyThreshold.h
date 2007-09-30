/***************************************************************************
                          DIA_flyThreshold.cpp  -  configuration dialog for
						   threshold filter
                              -------------------
                         Chris MacGregor, September 2007
                         chris-avidemux@bouncingdog.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FLY_THRESHOLD_H
#define FLY_THRESHOLD_H

class flyThreshold : public ADM_flyDialog
{
  
  private:
    THRESHOLD_PARAM param;

  public:
    uint8_t    process(void);
    uint8_t    download(void);
    uint8_t    upload(void);
    uint8_t    update(void);

    flyThreshold (uint32_t width, uint32_t height,
                  AVDMGenericVideoStream * in,
                  void * canvas, void * slider,
                  const THRESHOLD_PARAM * in_param)
        : ADM_flyDialog (width, height, in, canvas, slider, 1, RESIZE_AUTO),
          param (*in_param)
    {
    };

    void getParam (THRESHOLD_PARAM * outputParam)
    {
        *outputParam = param;
    }

    ADMImage * getInputImage ()
    {
        return _yuvBuffer;
    }

    ADMImage * getOutputImage ()
    {
        return _yuvBufferOut;
    }
};

#endif
