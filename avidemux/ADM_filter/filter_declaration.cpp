/***************************************************************************
                          filter_declaration.cpp  -  description

                          Declare filters, should be automatic but it does not work

                             -------------------
    begin                : Fri Mar 29 2002
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"
#ifdef HAVE_ENCODER

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"

#define FILTERDEC(x) extern   AVDMGenericVideoStream *x(AVDMGenericVideoStream *in, CONFcouple *param)
#define REGISTER(a,b,c,d) {FILTERDEC(d);registerFilter(a,b,c,d);}

void registerVideoFilters( void )
{
printf("\n Registering Filters\n");
printf(  "*********************\n");

	registerFilter("---- Image size/border alteration----",VF_INVALID,2,NULL);
	REGISTER("Crop",VF_CROP,1,crop_create);
	REGISTER("Resize",VF_RESIZE,1,res_create);

	REGISTER("Mplayer Resize",VF_MPLAYERRESIZE,1,mpresize_create);

    	REGISTER("Blacken Borders",VF_BLACKEN,1,bsmear_create);
  	REGISTER("Add black border",VF_ADDBORDER,1,addBorder_create);
  	REGISTER("Flip Vertical",VF_FLIP,1,flipv_create);
	REGISTER("Rotate",VF_ROTATE,1,rotate_create);
	registerFilter("----------- Interlacing -------------",VF_INVALID,2,NULL);
	// Buggy : Removed REGISTER("IVTC",VF_IVTC,1,ivtc_create);
	REGISTER("Mplayer ivtc",VF_MPDETC,1,mpdetc_create);	
	REGISTER("Decomb telecide",VF_DECOMB,1,decomb_create);
	REGISTER("Decomb decimate",VF_DECIMATE,1,decimate_create);
	
	REGISTER("PAL-SMART",VF_TELECIDE,1,telecide_create);
	REGISTER("Deinterlace",VF_DEINTERLACE,1,deinterlace_create);
	REGISTER("Lavcodec Deinterlacer",VF_LAVDEINT,1,lavdeint_create);
   	REGISTER("Pal Field shift",VF_PALSHIFT,1,addPALShift_create);
	
        REGISTER("Drop",VF_DROPOUT,1,dropout_create);
	REGISTER("SwapFields",VF_SWAPFIELDS,1,swapfield_create);
	REGISTER("Smart SwapFields",VF_SMARTSWAPFIELDS,1,swapsmart_create);

	REGISTER("Keep even fields",VF_KEEPEVEN,1,keepeven_create);
	REGISTER("Keep odd fields",VF_KEEPODD,1,keepodd_create);
	REGISTER("Separate Fields",VF_SEPARATEFIELDS,1,separatefield_create);
	REGISTER("Merge Fields",VF_MERGEFIELDS,1,mergefield_create);
	REGISTER("PullDown",VF_PULLDOWN,1,pulldown_create);
	
	REGISTER("D. Graft Kernel deint",VF_KRNDEINT,1,kerneldeint_create);
	

	REGISTER("Partial",VF_PARTIAL,0,partial_create);
	registerFilter("-------- Convolution Kernel ---------",VF_INVALID,2,NULL);
	REGISTER("Sharpen",VF_SHARPEN,1,sharpen_create);
   	REGISTER("Mean",VF_MEAN,1,mean_create);
   	REGISTER("Median",VF_MEDIAN,1,median_create);
	REGISTER("Median (5x5)",VF_LARGEMEDIAN,1,largeMedian_create);
    	REGISTER("Gauss Smooth",VF_GAUSSIAN,1,Gaussian_create);
       	
    //   registerFilter("Temporal smoother",tempsmooth_create);
	registerFilter("------------ Luma/Chroma ------------",VF_INVALID,2,NULL);
	      
 	REGISTER("Swap u & v",VF_SWAPUV,1,swapuv_create);
	REGISTER("Chroma shift",VF_CHROMASHIFT,1,create_chromashift);

	REGISTER("Contrast",VF_CONTRAST,1,contrast_create);

	REGISTER("Luma only",VF_LUMA,1,luma_create);
	REGISTER("Chroma U only",VF_CHROMAU,1,chromaU_create);
	REGISTER("Chroma V only",VF_CHROMAV,1,chromaV_create);
		
	registerFilter("-------------- Smoother -------------",VF_INVALID,2,NULL);
		
	REGISTER("Stabilize",VF_STABILIZE,1,stabilize_create);
 	REGISTER("Denoise",VF_DENOISE,1,denoise_create);
	REGISTER("FluxSmooth",VF_FLUXSMOOTH,1,fluxsmooth_create);
    	REGISTER("Smooth Clean",VF_SMOOTHCLEAN,0,smooth_create);
#ifdef USE_MMX
    	REGISTER("Temporal Cleaner",VF_VLADSMOOTH,1,vladsmooth_create);
#endif

	REGISTER("Mplayer Denoise3D",VF_MPLLQD3D,1,MPD3Dlow_create);

	REGISTER("Mplayer HQDenoise3D",VF_MPLHQD3D,1,MPD3D_create);
	REGISTER("Forced PostProcessing",VF_FORCEDPP,1,forcedpp_create);
	REGISTER("Soften",VF_SOFTEN,1,soften_create);
#ifdef USE_FREETYPE   
	registerFilter("----------------- Misc --------------",VF_INVALID,2,NULL);
	REGISTER("Subtitler",VF_SUBTILE,1,subtitle_create);
#endif
	REGISTER("Remove Salt",VF_SALT,0,salt_create);
 printf("\n");
}		


//


#endif
