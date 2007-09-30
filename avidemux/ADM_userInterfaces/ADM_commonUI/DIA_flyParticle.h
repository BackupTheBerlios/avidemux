/***************************************************************************
                          DIA_flyParticle.cpp  -  configuration dialog for
						   particle filter
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

#ifndef FLY_PARTICLE_H
#define FLY_PARTICLE_H

class flyParticle : public ADM_flyDialog
{
  
  private:
    PARTICLE_PARAM param;
    const MenuMapping * menu_mapping;
    uint32_t menu_mapping_count;
    bool input_buffer_valid;

  public:
    uint8_t    process(void);
    uint8_t    download(void);
    uint8_t    upload(void);
    uint8_t    update(void);

    flyParticle (uint32_t width, uint32_t height,
                 AVDMGenericVideoStream * in,
                 void * canvas, void * slider, void * dialog,
                 const PARTICLE_PARAM * in_param,
                 const MenuMapping * menu_mapping,
                 uint32_t menu_mapping_count)
        : ADM_flyDialog (width, height, in, canvas, slider, 1, RESIZE_AUTO),
          param (*in_param),
          menu_mapping (menu_mapping),
          menu_mapping_count (menu_mapping_count),
          input_buffer_valid (false)
    {
        _cookie = dialog;
        setupMenus (in_param, menu_mapping, menu_mapping_count);
    };

    void getParam (PARTICLE_PARAM * outputParam)
    {
        *outputParam = param;
    }

    ADMImage * getInputImage ()
    {
        if (!input_buffer_valid)
        {
            uint32_t len, flags;
            uint32_t frameNum = sliderGet();
            if (!_in->getFrameNumberNoAlloc(frameNum, &len,
                                            _yuvBuffer, &flags))
            {
                printf ("flyParticle::getInputImage(): Cannot get frame %u\n",
                        frameNum);
                return 0;
            }
            input_buffer_valid = true;
        }

        return _yuvBuffer;
    }

    ADMImage * getOutputImage ()
    {
        return _yuvBufferOut;
    }

    const MenuMapping * lookupMenu (const char * widgetName)
    {
        return ADM_flyDialog::lookupMenu (widgetName, menu_mapping,
                                          menu_mapping_count);
    }

    void getMenuValues ()
    {
        ADM_flyDialog::getMenuValues (&param, menu_mapping,
                                      menu_mapping_count);
    }
};

#endif
