/***************************************************************************
                          DIA_flySwissArmyKnife.cpp  -  configuration dialog for
						   Swiss Army Knife filter
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

#ifndef FLY_SWISSARMYKNIFE_H
#define FLY_SWISSARMYKNIFE_H

class flySwissArmyKnife : public ADM_flyDialog
{
  
  private:
    ADMVideoSwissArmyKnife * sakp;
    const MenuMapping * menu_mapping;
    uint32_t menu_mapping_count;

  public:
    SWISSARMYKNIFE_PARAM param;

    uint8_t    process(void);
    uint8_t    download(void);
    uint8_t    upload(void);
    uint8_t    update(void);

    flySwissArmyKnife (uint32_t width, uint32_t height,
                       AVDMGenericVideoStream * in,
                       void * canvas, void * slider, void * dialog,
                       ADMVideoSwissArmyKnife * sakp,
                       const SWISSARMYKNIFE_PARAM * in_param,
                       const MenuMapping * menu_mapping,
                       uint32_t menu_mapping_count)
        : ADM_flyDialog (width, height, in, canvas, slider, 1, RESIZE_AUTO),
          sakp (sakp),
          param (*in_param),
          menu_mapping (menu_mapping),
          menu_mapping_count (menu_mapping_count)
    {
        _cookie = dialog;
        setupMenus (in_param, menu_mapping, menu_mapping_count);
        // printf ("flySwissArmyKnife::flySwissArmyKnife: "
        //         "in_param %p, &param %p\n", in_param, &param);
    };

    uint8_t sliderChanged (void);

    void getParam (SWISSARMYKNIFE_PARAM * outputParam)
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

    void updateConfigDescription (bool do_download);
    void wipeOutputBuffer ();
};

#endif
