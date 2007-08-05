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
	REGISTERX("crop","Crop","Remove lines from top/bottom/left/right.",VF_CROP,1,crop_create,crop_script);
	REGISTERX("mpresize","MPlayer resize","Change image size. Faster than Avisynth's Resize.",VF_MPLAYERRESIZE,1,mpresize_create,mpresize_script);
	REGISTERX("resize","Resize","Picture resizer ported from Avisynth.", VF_RESIZE,1,res_create,resize_script);
  	REGISTERX("addblack","Add black borders","Add pure black border(s). Enlarges the picture.",VF_ADDBORDER,1,addBorder_create,addBorder_script);
	REGISTERX("blacken","Blacken borders","Fill borders with pure black. Doesn't alter size.",VF_BLACKEN,1,bsmear_create,bsmear_script);
  	REGISTERX("vflip","Vertical flip","Vertically flip the picture.",VF_FLIP,1,flipv_create,flipv_script);
	REGISTERX("rotate","Rotate","Rotate the picture by 90, 180 or 270 degrees.",VF_ROTATE,1,rotate_create,rotate_script);
        //REGISTERX("chfps","Change FPS",VF_CHFPS,1,chfps_create,chfps_script);
        REGISTERX("resamplefps","Resample fps","Change framerate while keeping duration.",VF_RESAMPLE_FPS,1,resamplefps_create,resamplefps_script);                
        REGISTERX("reverse","Reverse","Play video backward.",VF_REVERSE,1,reverse_create,reverse_script);
        REGISTERX("fade","Fade","Fade in/out.",VF_FADE,1,fade_create,fade_script);     

	registerFilter("----------- Interlacing -------------",VF_INVALID,2,NULL,NULL);
	// Buggy : Removed REGISTER("IVTC",VF_IVTC,1,ivtc_create);
//	REGISTERX("mpivtc","Mplayer ivtc",VF_MPDETC,1,mpdetc_create,mpdetc_script);	
        REGISTERX("YADIF","yadif","Yet Another DeInterlacer. Ported from MPlayer.",VF_YADIF,1,yadif_create,yadif_script);
        REGISTERX("mcdeinterlace","mcDeinterlace","Motion compensation deinterlacer. Ported from MPlayer.",VF_MCDEINT,1,mcdeint_create,mcdeint_script);
	REGISTERX("deinterlace","Deinterlace","Mask interlacing artifacts. Port of Smart deinterlace.",VF_DEINTERLACE,1,deinterlace_create,deinterlace_script);
	REGISTERX("kerneldeint","KernelDeint","Kernel deinterlacer by Donald Graft.",VF_KRNDEINT,1,kerneldeint_create,kerneldeint_script);
        REGISTERX("tdeint","TDeint","Motion adaptative deinterlacer by Tritical.",VF_TDEINT,1,tdeint_create,tdeint_script);
        REGISTERX("lavcppdeint","libavcodec deinterlacer","All FFmpeg deinterlace filters (bicubic, median, ...).",VF_LAVPP_DEINT,1,lavppdeint_create,lavppdeint_script);
	REGISTERX("telecide","Decomb Telecide","Great deinterlacing package including IVTC.",VF_DECOMB,1,decomb_create,decomb_script);
	REGISTERX("decimate","Decomb Decimate","Useful to remove dupes left by Telecide.",VF_DECIMATE,1,decimate_create,decimate_script);
	REGISTERX("pulldown","Pulldown","Convert 24 fps to 30 fps by repeating fields.",VF_PULLDOWN,1,pulldown_create,pulldown_script);
	
	REGISTERX("dgbob","DGBob","Bobber - convert each field into a full sized frame.",VF_DGBOB,1,dgbob_create,dgbob_script);

        REGISTERX("palfieldshift","PAL field shift","Shift fields by one. Useful for some PAL movies.",VF_PALSHIFT,1,addPALShift_create,addPALShift_script);	
	REGISTERX("palsmart","PAL smart","Smartly revert non constant PAL field shift.",VF_TELECIDE,1,telecide_create,telecide_script);
//	REGISTERX("lavdeint","Lavcodec Deinterlacer",VF_LAVDEINT,1,lavdeint_create,lavdeint_script);
        
        
	
	REGISTERX("drop","Drop","Drop damaged fields (e.g. from VHS capture).",VF_DROPOUT,1,dropout_create,dropout_script);
	REGISTERX("swapfields","Swap fields","Swap top and bottom fields.",VF_SWAPFIELDS,1,swapfield_create,swapfield_script);
	REGISTERX("smartswapfield","Smart swap fields","Smartly swap fields. Needed when field order changes.",VF_SMARTSWAPFIELDS,1,swapsmart_create,swapsmart_script);

	REGISTERX("keepevenfield","Keep even fields","Keep top fields. Gives a half height picture.",VF_KEEPEVEN,1,keepeven_create,keepeven_script);
	REGISTERX("keepoddfield","Keep odd fields","Keep bottom fileds. Gives a half height picture.",VF_KEEPODD,1,keepodd_create,keepodd_script);
	REGISTERX("separatefields","Separate fields","Each field becomes full picture, half sized.",VF_SEPARATEFIELDS,1, separatefield_create,separatefield_script);
	REGISTERX("mergefield","Merge fields","Merge two pictures as if they were two fields.",VF_MERGEFIELDS,1,mergefield_create,mergefield_script);

	REGISTERX("stackfield","Stack fields","Put two fields on top of one another.",VF_STACKFIELD,1,stackfield_create,stackfield_script);
        REGISTERX("unstackfield","Unstack fields","Interleave top and bottom part of the picture.",VF_UNSTACK_FIELD,1,unstackfield_create,unstackfield_script);
	
	registerFilter("------------ Luma/Chroma ------------",VF_INVALID,2,NULL,NULL);
              
        REGISTERX("eq2","MPlayer eq2","Adjust contrast, brightness, saturation and gamma.",VF_EQ2,1,Eq2_create,Eq2_script);
        REGISTERX("hue","MPlayer hue","Adjust hue and saturation.",VF_HUE,1,hue_create,hue_script);
        REGISTERX("contrast","Contrast","Adjust contrast, brightness and colors.",VF_CONTRAST,1,contrast_create,contrast_script);
        REGISTERX("equalizer","Luma equalizer","Luma correction filter with histogram.",VF_EQUALIZER,1,equalizer_create,equalizer_script);

        REGISTERX("swapuv","Swap U and V","Invert chroma U and chroma V.",VF_SWAPUV,1,swapuv_create,swapuv_script);
        REGISTERX("chromashift","Chroma shift","Shift chroma U/V to fix badly synced luma/chroma.",VF_CHROMASHIFT,1,create_chromashift,chromashift_script);

        REGISTERX("lumaonly","Luma only","Convert picture to greyscale (black and white).",VF_LUMA,1,luma_create,luma_script);
        REGISTERX("chromauonly","Chroma U only","Convert picture to grey, keeping only chroma U.",VF_CHROMAU,1,chromaU_create,chromaU_script);
        REGISTERX("chromavonly","Chroma V only","Convert picture to grey, keeping only chroma V.",VF_CHROMAV,1,chromaV_create,chromaV_script);
       
        REGISTERX("delta","Luma delta","Difference between current and previous picture.",VF_DELTA,1,delta_create,delta_script);
        REGISTERX("coloryuv","Avisynth ColorYUV","Alter colors (auto white balance etc...). Ported from Avisynth.",VF_COLOR_YUV,1,coloryuv_create,coloryuv_script);
                 


       
    
	
	registerFilter("-------------- Smoother -------------",VF_INVALID,2,NULL,NULL);
	//   registerFilter("Temporal smoother",tempsmooth_create);	
	REGISTERX("mpdenoise3d","MPlayer denoise3d","Reduce noise, smooth image, increase compressibility.",VF_MPLLQD3D,1,MPD3Dlow_create,MPD3Dlow_script);
	REGISTERX("mphqdenoise3d","MPlayer hqdn3d","High quality version of denoise3d. Slower but more precise.",VF_MPLHQD3D,1,MPD3D_create,MPD3D_script);
	REGISTERX("fluxsmooth","FluxSmooth","Spatio-temporal cleaner by Ross Thomas.",VF_FLUXSMOOTH,1,fluxsmooth_create,fluxsmooth_script);
#if defined( ARCH_X86)  || defined(ARCH_X86_64)
    	REGISTERX("temporalcleaner","Temporal Cleaner","Vlad59's Avisynth port of Jim Casaburi's denoiser.",VF_VLADSMOOTH,1,vladsmooth_create,vladsmooth_script);
#endif
	REGISTERX("denoise","Denoise","Port of Transcode DNR.",VF_DENOISE,1,denoise_create,denoise_script);
	REGISTERX("stabilize","Stabilize","Light denoiser.",VF_STABILIZE,1,stabilize_create,stabilize_script);

        REGISTERX("cnr2","Cnr2","Chroma noise reduction filter by MarcFD/Tritical.",VF_CNR2,1,cnr2_create,cnr2_script);	

	REGISTERX("msmooth","MSmooth by Donald Graft","Smooth the image, don't blur edges. Useful on anime.",VF_MSMOOTH,1,create_msmooth,msmooth_script);
	REGISTERX("soften","Soften","A variant of stabilize.",VF_SOFTEN,1,soften_create,soften_script);

	//REGISTERX("smoothclean","Smooth Clean",VF_SMOOTHCLEAN,0,smooth_create,smooth_script);
        registerFilter("-------- Convolution Kernel ---------",VF_INVALID,2,NULL,NULL);
        REGISTERX("sharpen","Sharpen","Enhance difference between near pixels.",VF_SHARPEN,1,sharpen_create,sharpen_script);
        REGISTERX("msharpen","MSharpen","Sharpen edges without amplifying noise. By Donald Graft.",VF_MSHARPEN,1,msharpen_create,msharpen_script);
        REGISTERX("asharp","asharp","Adaptative sharpener by MarcFD.",VF_ASHARP,1,asharp_create,asharp_script);

        REGISTERX("gaussian","Gauss smooth","Gaussian smooth. Blur the picture.",VF_GAUSSIAN,1,Gaussian_create,gaussian_script);
        REGISTERX("mean","Mean","Mean (blur) kernel.",VF_MEAN,1,mean_create,mean_script);
        REGISTERX("median","Median","Median kernel 3x3. Remove high frequency noise.",VF_MEDIAN,1,median_create,median_script);
        REGISTERX("largemedian","Median (5x5)","Median kernel 5x5. Good for reducing chroma noise.",VF_LARGEMEDIAN,1,largeMedian_create,largeMedian_script);
        REGISTERX("forcedpp","Forced postprocessing","Apply blind postprocessing.",VF_FORCEDPP,1,forcedpp_create,forcedpp_script);        

	registerFilter("----------------- Misc --------------",VF_INVALID,2,NULL,NULL);
#ifdef USE_FREETYPE   
	REGISTERX("subtitle","Subtitler","Add srt/sub subtitles to the picture.",VF_SUBTILE,1,subtitle_create,subtitle_script);
	REGISTERX("ass","ASS","Add ASS/SSA subtitles to the picture.",VF_ASS,1,ass_create,ass_script);
#endif
        REGISTERX("vobsub","VobSub","Embed VobSub subtitles into picture.",VF_VOBSUB,1,vobsub_create,vobsub_script);
#if 1 //def ADM_DEBUG
        REGISTERX("dvbsub","DVB sub","Embed DVB-T subtitle.",VF_DVBSUB,1,dvbsub_create,dvbsub_script);
#endif

        //*********************
 registerFilter("-------- Deblender ---------",VF_INVALID,2,NULL,NULL);

        REGISTERX("blendremover","Blend remover","Remove blend between previous and next picture.",VF_BLENDREMOVAL,1,blendremove_create,blendremove_script);
        REGISTERX("hardivtcremove","Hard pulldown removal","Remove IVTC that has been analog captured or resized.",VF_HARDIVTC,1,hardivtc_create,hardivtc_script);
        REGISTERX("whirl","Whirl","Useless whirlwind effect.",VF_WHIRL,1,whirl_create,whirl_script);
        REGISTERX("mosaic","Mosaic","Split the picture into tiny thumbnails.",VF_MOSAIC,1,     mosaic_create,mosaic_script);
        REGISTERX("mpdelogo","MPlayer delogo","Blend a logo by interpolating its surrounding box.",VF_MPDELOGO,1,mpdelogo_create,mpdelogo_script);
        REGISTERX("logo","Logo","Add a png as logo.",VF_LOGO,1,logo_create,logo_script);
        REGISTERX("animatedmenu","Animated Menu",
            "Create a video made of 6 mini windows, very useful to do DVD menus.",
            VF_ANIMATED,1,animated_create,animated_script);

// Does not work
//        REGISTERX("unblend","Unblend by Bach",VF_UNBLEND,1,unblend_create,unblend_script);
        
        REGISTER("","Partial","",VF_PARTIAL,0,partial_create);
//	REGISTER("","Remove Salt",VF_SALT,0,salt_create);
 printf("\n");
}		


//


#endif
