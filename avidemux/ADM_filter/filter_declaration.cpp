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
#define REGISTER(e,a,g,b,c,d) {FILTERDEC(d);registerFilterEx(a,b,c,d,e,NULL,NULL);}
#define REGISTERX(e,a,g,b,c,d,f) {\
FILTERDECX(f);\
FILTERDEC(d);registerFilterEx(a,b,c,d,e,f,g);\
}

void registerVideoFilters( void )
{
printf("\n Registering Filters\n");
printf(  "*********************\n");

	
	registerFilter("---- Image size/border alteration----",VF_INVALID,2,NULL,NULL);
	REGISTERX("crop","Crop","Crop the image by removing some pixels\n from top/bottom/left/right",VF_CROP,1,crop_create,crop_script);
	REGISTERX("mpresize","Mplayer Resize","Mplayer picture resizer ",VF_MPLAYERRESIZE,1,mpresize_create,mpresize_script);
	REGISTERX("resize","Resize","Picture resizer ported from avisynth", VF_RESIZE,1,res_create,resize_script);
	REGISTERX("blacken","Blacken Borders","Fill borders with pure b lack",VF_BLACKEN,1,bsmear_create,bsmear_script);
  	REGISTERX("addblack","Add black border","Add pure black border(s) to the picture",VF_ADDBORDER,1,addBorder_create,addBorder_script);
  	REGISTERX("vflip","Flip Vertical","Vertically flip the picture",VF_FLIP,1,flipv_create,flipv_script);
	REGISTERX("rotate","Rotate","Rotate the picture by 90,180 or 270 degrees",VF_ROTATE,1,rotate_create,rotate_script);
        //REGISTERX("chfps","Change FPS",VF_CHFPS,1,chfps_create,chfps_script);
        REGISTERX("resamplefps","Resample FPS","Change the fps of a movie while keeping the duration",VF_RESAMPLE_FPS,1,resamplefps_create,resamplefps_script);                


	registerFilter("----------- Interlacing -------------",VF_INVALID,2,NULL,NULL);
	// Buggy : Removed REGISTER("IVTC",VF_IVTC,1,ivtc_create);
//	REGISTERX("mpivtc","Mplayer ivtc",VF_MPDETC,1,mpdetc_create,mpdetc_script);	
        REGISTERX("tdeint","TDeint by Tritical","Motion adaptative deinterlacer",VF_TDEINT,1,tdeint_create,tdeint_script);
	REGISTERX("telecide","Decomb telecide","Great deinterlacing package including ivtc",VF_DECOMB,1,decomb_create,decomb_script);
	REGISTERX("decimate","Decomb decimate","Useful to remove dupes left by telecide",VF_DECIMATE,1,decimate_create,decimate_script);
	
	REGISTER("palsmart","PAL-SMART","Smartly revert non constant Pal field shift",VF_TELECIDE,1,telecide_create);
	REGISTERX("deinterlace","Deinterlace","Port of smart deinterlace",VF_DEINTERLACE,1,deinterlace_create,deinterlace_script);
//	REGISTERX("lavdeint","Lavcodec Deinterlacer",VF_LAVDEINT,1,lavdeint_create,lavdeint_script);
        
        REGISTERX("lavcppdeint","Lavcodec PP Deinterlacer","All lavcodec deinterlacers (bicubic,median...)",VF_LAVPP_DEINT,1,lavppdeint_create,lavppdeint_script);
        
        REGISTERX("palfieldshift","Pal Field shift","Shift the fields by one, useful for some Pal movie",VF_PALSHIFT,1,addPALShift_create,addPALShift_script);
	
	REGISTERX("drop","Drop","Drop a field if it is damaged (for example VHS capture)",VF_DROPOUT,1,dropout_create,dropout_script);
	REGISTERX("swapfields","SwapFields","Swap Top and Bottom fields",VF_SWAPFIELDS,1,swapfield_create,swapfield_script);
	REGISTERX("smartswapfield","Smart SwapFields","Smartly swap fields. Needed when field order changes.",VF_SMARTSWAPFIELDS,1,swapsmart_create,swapsmart_script);

	REGISTERX("keepevenfield","Keep even fields","Keep the Even field (top). That gives a half height picture",VF_KEEPEVEN,1,keepeven_create,keepeven_script);
	REGISTERX("keepoddfield","Keep odd fields","Keep the Odd field (bottom). That gives a half height picture",VF_KEEPODD,1,keepodd_create,keepodd_script);
	REGISTERX("separatefields","Separate Fields","Each field becomes a full picture, half sized.",VF_SEPARATEFIELDS,1, separatefield_create,separatefield_script);
	REGISTERX("mergefield","Merge Fields","Merge two picture as if they were the two fields \nof a picture twice as big picture",VF_MERGEFIELDS,1,mergefield_create,mergefield_script);

	REGISTERX("stackfield","Stack Fields","Put the two fields on top of one another",VF_STACKFIELD,1,stackfield_create,stackfield_script);
        REGISTERX("unstackfield","UnStack Fields","Interleave top and bottom part of the picture",VF_UNSTACK_FIELD,1,unstackfield_create,unstackfield_script);
	
	REGISTERX("pulldown","PullDown","Convert 24 fps to 30 fps by repeating fields",VF_PULLDOWN,1,pulldown_create,pulldown_script);
	
	REGISTERX("kerneldeint","D. Graft Kernel deint","Kernel deinterlacer",VF_KRNDEINT,1,kerneldeint_create,kerneldeint_script);
	REGISTERX("dgbob","DGBob","Bobber, convert each field into a full sized frame",VF_DGBOB,1,dgbob_create,dgbob_script);

	registerFilter("------------ Luma/Chroma ------------",VF_INVALID,2,NULL,NULL);
              
        REGISTERX("swapuv","Swap u & v","Invert chroma U and chroma V",VF_SWAPUV,1,swapuv_create,swapuv_script);
        REGISTERX("chromashift","Chroma shift","Shift chroma u/v to revert badly synced luma/chroma",VF_CHROMASHIFT,1,create_chromashift,chromashift_script);

        REGISTERX("contrast","Contrast","Contrast adjuster",VF_CONTRAST,1,contrast_create,contrast_script);
        REGISTERX("equalizer","Luma equalizer","Luma correction filter with histogram",VF_EQUALIZER,1,equalizer_create,equalizer_script);

        REGISTERX("lumaonly","Luma only","Convert picture to grey/black and white",VF_LUMA,1,luma_create,luma_script);
        REGISTERX("chromauonly","Chroma U only","Convert picture to grey, keeping only chroma u",VF_CHROMAU,1,chromaU_create,chromaU_script);
        REGISTERX("chromavonly","Chroma V only","Convert picture to grey, keeping only chroma V",VF_CHROMAV,1,chromaV_create,chromaV_script);
        REGISTERX("hue","Mplayer Hue","Color equalizer ported from mplayer",VF_HUE,1,hue_create,hue_script);
        REGISTERX("eq2","Mplayer Eq2","Color equalizer2 ported from mplayer",VF_EQ2,1,Eq2_create,Eq2_script);
       
        REGISTERX("delta","Luma delta","Replace a  picture by the difference between that \npicture and the previous one",VF_DELTA,1,delta_create,delta_script);
                


       
    
	
	registerFilter("-------------- Smoother -------------",VF_INVALID,2,NULL,NULL);
	//   registerFilter("Temporal smoother",tempsmooth_create);	
	REGISTERX("stabilize","Stabilize","",VF_STABILIZE,1,stabilize_create,stabilize_script);
	REGISTERX("denoise","Denoise","",VF_DENOISE,1,denoise_create,denoise_script);
	REGISTERX("fluxsmooth","FluxSmooth","",VF_FLUXSMOOTH,1,fluxsmooth_create,fluxsmooth_script);
	
#if defined( ARCH_X86)  || defined(ARCH_X86_64)
    	REGISTERX("temporalcleaner","Temporal Cleaner","",VF_VLADSMOOTH,1,vladsmooth_create,vladsmooth_script);
#endif

	REGISTERX("mpdenoise3d","Mplayer Denoise3D","",VF_MPLLQD3D,1,MPD3Dlow_create,MPD3Dlow_script);
	REGISTERX("mphqdenoise3d","Mplayer HQDenoise3D","",VF_MPLHQD3D,1,MPD3D_create,MPD3D_script);
	REGISTERX("msmooth","Msmooth by Donald Graft","",VF_MSMOOTH,1,create_msmooth,msmooth_script);

	REGISTERX("soften","Soften","",VF_SOFTEN,1,soften_create,soften_script);
        REGISTERX("cnr2","CNR2 (MarcFD/Tritical)","Chroma Nois Reduction filter",VF_CNR2,1,cnr2_create,cnr2_script);
	//REGISTERX("smoothclean","Smooth Clean",VF_SMOOTHCLEAN,0,smooth_create,smooth_script);
        registerFilter("-------- Convolution Kernel ---------",VF_INVALID,2,NULL,NULL);
        REGISTERX("sharpen","Sharpen","Sharpening kernel",VF_SHARPEN,1,sharpen_create,sharpen_script);
        REGISTERX("mean","Mean","Mean (blur) kernel",VF_MEAN,1,mean_create,mean_script);
        REGISTERX("median","Median","Median kernel 3x3.",VF_MEDIAN,1,median_create,median_script);
        REGISTERX("largemedian","Median (5x5)","Median kernel 5x5.",VF_LARGEMEDIAN,1,largeMedian_create,largeMedian_script);
        REGISTERX("gaussian","Gauss Smooth","Gauss Smooth lernel",VF_GAUSSIAN,1,Gaussian_create,gaussian_script);
        REGISTERX("msharpen","MSharpen by Donald Graft","Masked sharpen",VF_MSHARPEN,1,msharpen_create,msharpen_script);
        REGISTERX("asharp","ASharp by MarcFD","Adaptative sharpener",VF_ASHARP,1,asharp_create,asharp_script);
        REGISTERX("forcedpp","Forced PostProcessing","",VF_FORCEDPP,1,forcedpp_create,forcedpp_script);        

	registerFilter("----------------- Misc --------------",VF_INVALID,2,NULL,NULL);
#ifdef USE_FREETYPE   
	REGISTERX("subtitle","Subtitler","",VF_SUBTILE,1,subtitle_create,subtitle_script);
#endif
        REGISTERX("vobsub","VobSub","",VF_VOBSUB,1,vobsub_create,vobsub_script);
        //*********************
 registerFilter("-------- Deblender ---------",VF_INVALID,2,NULL,NULL);

        REGISTERX("blendremover","Blend Remover","Remove picture that are a blend between the previous \nand the next.",VF_BLENDREMOVAL,1,blendremove_create,blendremove_script);
        REGISTERX("hardivtcremove","Hard pulldown removal","Try to remove ivtc that has been analog captured or resized.",VF_HARDIVTC,1,hardivtc_create,hardivtc_script);
        REGISTERX("whirl","Whirl","Useless whirlwind effect",VF_WHIRL,1,whirl_create,whirl_script);
        REGISTERX("mosaic","Mosaic","Split the picture into tiny thumbnails",VF_MOSAIC,1,     mosaic_create,mosaic_script);

// Does not work
//        REGISTERX("unblend","Unblend by Bach",VF_UNBLEND,1,unblend_create,unblend_script);
        
        REGISTER("","Partial","",VF_PARTIAL,0,partial_create);
//	REGISTER("","Remove Salt",VF_SALT,0,salt_create);
 printf("\n");
}		


//


#endif
