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
#include <ADM_assert.h>
#include <ADM_assert.h>

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
#define FILTERDECX(x) extern   AVDMGenericVideoStream *x(AVDMGenericVideoStream *in, int n,Arg *args)
#define REGISTER(e,a,b,c,d) {FILTERDEC(d);registerFilterEx(a,b,c,d,e,NULL);}
#define REGISTERX(e,a,b,c,d,f) {\
FILTERDECX(f);\
FILTERDEC(d);registerFilterEx(a,b,c,d,e,f);\
}

void registerVideoFilters( void )
{
printf("\n Registering Filters\n");
printf(  "*********************\n");

	registerFilter("---- Image size/border alteration----",VF_INVALID,2,NULL,NULL);
	REGISTERX("crop","Crop",VF_CROP,1,crop_create,crop_script);
	REGISTERX("mpresize","Mplayer Resize",VF_MPLAYERRESIZE,1,mpresize_create,mpresize_script);
	REGISTERX("resize","Resize",VF_RESIZE,1,res_create,resize_script);
	REGISTERX("blacken","Blacken Borders",VF_BLACKEN,1,bsmear_create,bsmear_script);
  	REGISTERX("addblack","Add black border",VF_ADDBORDER,1,addBorder_create,addBorder_script);
  	REGISTERX("vflip","Flip Vertical",VF_FLIP,1,flipv_create,flipv_script);
	REGISTERX("rotate","Rotate",VF_ROTATE,1,rotate_create,rotate_script);
	REGISTERX("whirl","Whirl",VF_WHIRL,1,whirl_create,whirl_script);
	registerFilter("----------- Interlacing -------------",VF_INVALID,2,NULL,NULL);
	// Buggy : Removed REGISTER("IVTC",VF_IVTC,1,ivtc_create);
//	REGISTERX("mpivtc","Mplayer ivtc",VF_MPDETC,1,mpdetc_create,mpdetc_script);	
	REGISTERX("telecide","Decomb telecide",VF_DECOMB,1,decomb_create,decomb_script);
	REGISTERX("decimate","Decomb decimate",VF_DECIMATE,1,decimate_create,decimate_script);
	
	REGISTER("palsmart","PAL-SMART",VF_TELECIDE,1,telecide_create);
	REGISTERX("deinterlace","Deinterlace",VF_DEINTERLACE,1,deinterlace_create,deinterlace_script);
	REGISTERX("lavdeint","Lavcodec Deinterlacer",VF_LAVDEINT,1,lavdeint_create,lavdeint_script);
	REGISTERX("palfieldshift","Pal Field shift",VF_PALSHIFT,1,addPALShift_create,addPALShift_script);
	
	REGISTERX("drop","Drop",VF_DROPOUT,1,dropout_create,dropout_script);
	REGISTERX("swapfields","SwapFields",VF_SWAPFIELDS,1,swapfield_create,swapfield_script);
	REGISTERX("smartswapfield","Smart SwapFields",VF_SMARTSWAPFIELDS,1,swapsmart_create,swapsmart_script);

	REGISTERX("keepevenfield","Keep even fields",VF_KEEPEVEN,1,keepeven_create,keepeven_script);
	REGISTERX("keepoddfield","Keep odd fields",VF_KEEPODD,1,keepodd_create,keepodd_script);
	REGISTERX("separatefields","Separate Fields",VF_SEPARATEFIELDS,1,
							separatefield_create,separatefield_script);
	REGISTERX("stackfield","Stack Fields",VF_STACKFIELD,1,stackfield_create,stackfield_script);
	REGISTERX("mergefield","Merge Fields",VF_MERGEFIELDS,1,mergefield_create,mergefield_script);
	REGISTERX("pulldown","PullDown",VF_PULLDOWN,1,pulldown_create,pulldown_script);
	
	REGISTERX("kerneldeint","D. Graft Kernel deint",VF_KRNDEINT,1,kerneldeint_create,kerneldeint_script);
	

	REGISTER("","Partial",VF_PARTIAL,0,partial_create);
	registerFilter("-------- Convolution Kernel ---------",VF_INVALID,2,NULL,NULL);
	REGISTERX("sharpen","Sharpen",VF_SHARPEN,1,sharpen_create,sharpen_script);
   	REGISTERX("mean","Mean",VF_MEAN,1,mean_create,mean_script);
   	REGISTERX("median","Median",VF_MEDIAN,1,median_create,median_script);
	REGISTERX("largemedian","Median (5x5)",VF_LARGEMEDIAN,1,largeMedian_create,largeMedian_script);
    	REGISTERX("gaussian","Gauss Smooth",VF_GAUSSIAN,1,Gaussian_create,gaussian_script);
       	
    
	registerFilter("------------ Luma/Chroma ------------",VF_INVALID,2,NULL,NULL);
	      
 	REGISTERX("swapuv","Swap u & v",VF_SWAPUV,1,swapuv_create,swapuv_script);
	REGISTERX("chromashift","Chroma shift",VF_CHROMASHIFT,1,create_chromashift,chromashift_script);

	REGISTERX("contrast","Contrast",VF_CONTRAST,1,contrast_create,contrast_script);
	REGISTERX("equalizer","Luma equalizer",VF_EQUALIZER,1,equalizer_create,equalizer_script);

	REGISTERX("lumaonly","Luma only",VF_LUMA,1,luma_create,luma_script);
	REGISTERX("chromauonly","Chroma U only",VF_CHROMAU,1,chromaU_create,chromaU_script);
	REGISTERX("chromavonly","Chroma V only",VF_CHROMAV,1,chromaV_create,chromaV_script);
		
	registerFilter("-------------- Smoother -------------",VF_INVALID,2,NULL,NULL);
	//   registerFilter("Temporal smoother",tempsmooth_create);	
	REGISTERX("stabilize","Stabilize",VF_STABILIZE,1,stabilize_create,stabilize_script);
	REGISTERX("denoise","Denoise",VF_DENOISE,1,denoise_create,denoise_script);
	REGISTERX("fluxsmooth","FluxSmooth",VF_FLUXSMOOTH,1,fluxsmooth_create,fluxsmooth_script);
	
#ifdef USE_MMX
    	REGISTERX("temporalcleaner","Temporal Cleaner",VF_VLADSMOOTH,1,vladsmooth_create,vladsmooth_script);
#endif

	REGISTERX("mpdenoise3d","Mplayer Denoise3D",VF_MPLLQD3D,1,MPD3Dlow_create,MPD3Dlow_script);
	REGISTERX("mphqdenoise3d","Mplayer HQDenoise3D",VF_MPLHQD3D,1,MPD3D_create,MPD3D_script);
	REGISTERX("msmooth","Msmooth by Donald Graft",VF_MSMOOTH,1,create_msmooth,msmooth_script);
	REGISTERX("forcedpp","Forced PostProcessing",VF_FORCEDPP,1,forcedpp_create,forcedpp_script);
	REGISTERX("soften","Soften",VF_SOFTEN,1,soften_create,soften_script);
	REGISTERX("smoothclean","Smooth Clean",VF_SMOOTHCLEAN,0,smooth_create,smooth_script);
#ifdef USE_FREETYPE   
	registerFilter("----------------- Misc --------------",VF_INVALID,2,NULL,NULL);
	REGISTERX("subtitle","Subtitler",VF_SUBTILE,1,subtitle_create,subtitle_script);
#endif
        REGISTERX("vobsub","VobSub",VF_VOBSUB,1,vobsub_create,vobsub_script);

//	REGISTER("","Remove Salt",VF_SALT,0,salt_create);
 printf("\n");
}		


//


#endif
