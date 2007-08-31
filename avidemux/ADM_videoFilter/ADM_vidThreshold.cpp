/***************************************************************************
                          ADM_vidThreshold.cpp  -  do thresholding
                              -------------------
                          Chris MacGregor, 2005, 2007
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
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_filter/video_filters.h"

#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"

#include "ADM_vidThreshold.h"

// #define THRESHOLD_HISTO 1

static FILTER_PARAM thresholdParam={3,{"min", "max", "debug"}};

SCRIPT_CREATE(threshold_script,ADMVideoThreshold,thresholdParam);

BUILD_CREATE(threshold_create,ADMVideoThreshold);

ADMVideoThreshold::ADMVideoThreshold(AVDMGenericVideoStream *in,CONFcouple *couples)
			
{
    printf ("ADMVideoThreshold ctor (%p)\n", this);
    _in=in;
    memcpy(&_info,in->getInfo(),sizeof(_info));
    _info.encoding=1;	
    _uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
    ADM_assert(_uncompressed);    	  	
    _param=NEW( THRESHOLD_PARAM );
    if(couples)
    {
        GET(min);
        GET(max);
        GET(debug);
    }
    else
    {
        _param->min = 100;
        _param->max = 200;
        _param->debug = 0;
    }
}

uint8_t	ADMVideoThreshold::getCoupledConf( CONFcouple **couples)
{

    ADM_assert(_param);
    *couples=new CONFcouple(3);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
    CSET(min);
    CSET(max);
    CSET(debug);

    return 1;

}

uint8_t ADMVideoThreshold::configure(AVDMGenericVideoStream *in)
{
    UNUSED_ARG(in);

    diaElemUSlider minslide(&(_param->min), _("Mi_nimum value to be non-zero in output:"), 0, 255);
    diaElemUSlider maxslide(&(_param->max), _("Ma_ximum value to be non-zero in output:"), 0, 255);
    diaElemUInteger debug(&(_param->debug), _("_Debugging settings (bits):"),
                          0, 0x7fffffff);
    diaElem * elems[] = { &minslide, &maxslide, &debug };

    uint8_t ret = diaFactoryRun("Threshold", sizeof (elems) / sizeof (diaElem *), elems);
    return ret;
}

ADMVideoThreshold::~ADMVideoThreshold()
{
    printf ("ADMVideoThreshold dtor (%p)\n", this);
    DELETE(_param);
    delete  _uncompressed;	
    _uncompressed=NULL;
}

char	*ADMVideoThreshold::printConf( void) 
{
    static char conf[100];

    sprintf(conf,"Threshold: Min = %3u  Max = %3u  Debug=0x%06x",
            _param->min, _param->max, _param->debug);
    return conf;
	
}
uint8_t ADMVideoThreshold::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          			ADMImage *data,uint32_t *flags)
{
    if (frame>= _info.nb_frames)
        return 0;

    uint32_t debug = _param->debug;

    if (debug & 1)
        printf ("in ADMVideoThreshold::getFrameNumberNoAlloc(%d, ...)\n",
                frame);

    uint32_t planesize = _info.width * _info.height;
    uint32_t size = (planesize*3)>>1;
    *len = size;
			
    if (!_in->getFrameNumberNoAlloc (frame, len, _uncompressed, flags))
        return 0;
    ADMImage * image = _uncompressed;

    // HERE: for speed, we do luma (Y plane) only.  However, some
    // users might want chroma, too... we should make that
    // an option or something.

    uint8_t * currp = YPLANE (image) + planesize;
    uint8_t * destp = YPLANE (data) + planesize;
    uint32_t pixremaining = planesize + 1;

#ifdef THRESHOLD_HISTO
    uint32_t histo_before [256];
    uint32_t histo_after [256];

    memset (histo_before, 0, 256 * sizeof (uint32_t));
    memset (histo_after, 0, 256 * sizeof (uint32_t));
#endif

    uint32_t min = _param->min;
    uint32_t max = _param->max;

    uint8_t in_range_value;
    uint8_t out_of_range_value;
    if (min <= max)
    {
        in_range_value = 255;
        out_of_range_value = 0;
    }
    else
    {
        in_range_value = 0;
        out_of_range_value = 255;
        uint32_t tmp = min;
        min = max;
        max = tmp;
    }

#ifdef DUMP_FRAME_DATA
    FILE * fp = fopen ("/tmp/framedump.new.out", "a");
    ADM_assert(fp);
#endif

    while (--pixremaining)
    {
        int32_t curr = *--currp;

#ifdef THRESHOLD_HISTO
        histo_before [curr]++;
#endif

#ifdef DUMP_FRAME_DATA
        fprintf (fp, "%x\n", curr);
#endif

        if (curr < min || curr > max)
            *--destp = out_of_range_value;
        else
            *--destp = in_range_value;

#ifdef THRESHOLD_HISTO
        histo_after [*destp]++;
#endif
    }

#ifdef DUMP_FRAME_DATA
    fclose (fp);
#endif

#ifdef THRESHOLD_HISTO
    if (debug & 0x800)
    {
        printf ("    Histogram:\n\tValue\tBefore\tAfter\tValue\tBefore\tAfter"
                "\tValue\tBefore\tAfter\tValue\tBefore\tAfter\n");
        for (int val = 0; val <= 63; val++)
            printf ("\t%3d:\t%6d\t%6d\t%3d:\t%6d\t%6d\t%3d:\t%6d\t%6d\t%3d:\t%6d\t%6d\n",
                    val, histo_before [val], histo_after [val],
                    val + 64, histo_before [val + 64], histo_after [val + 64],
                    val + 128, histo_before [val + 128], histo_after [val + 128],
                    val + 192, histo_before [val + 192], histo_after [val + 192]);
    }
#endif

    // HERE: the following two lines do a luma-only-ize

    memset (UPLANE (data), 128, planesize >> 2);
    memset (VPLANE (data), 128, planesize >> 2);

    data->copyInfo(image);
    return 1;
}	                           
