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

#include "config.h"
#include "ADM_default.h"



#include "ADM_videoFilter.h"
#include "ADM_videoFilter_internal.h"
/*
void registerFilterEx(const char *name,const char *filtername,VF_CATEGORY category,
		AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, CONFcouple *),
		AVDMGenericVideoStream *(*create_from_script) (AVDMGenericVideoStream *in, int n,Arg *args),
		const char *descText)
*/

#define FILTERDEC_create(x) extern   AVDMGenericVideoStream *x(AVDMGenericVideoStream *in, CONFcouple *param)
#define FILTERDEC_createFromScript(x) extern   AVDMGenericVideoStream *x(AVDMGenericVideoStream *in, int n,Arg *args)

#define REGISTERX(z,a,b,c,d,e,f,g) {\
			FILTERDEC_create(f);\
			FILTERDEC_createFromScript(g);\
			registerFilterEx((const char*)a,(const char *)b,z,f,g,(const char *)c);\
}

void registerVideoFilters( void )
{
printf("\n Registering Filters\n");
printf(  "*********************\n");

	
	REGISTERX(VF_TRANSFORM, "crop",QT_TR_NOOP("Crop"),QT_TR_NOOP("Remove lines from top/bottom/left/right."),VF_CROP,1,crop_create,crop_script);
	REGISTERX(VF_TRANSFORM, "mpresize",QT_TR_NOOP("MPlayer resize"),QT_TR_NOOP("Change image size. Faster than Avisynth's Resize."),VF_MPLAYERRESIZE,1,mpresize_create,mpresize_script);
	REGISTERX(VF_TRANSFORM, "resize",QT_TR_NOOP("Resize"),QT_TR_NOOP("Picture resizer ported from Avisynth."), VF_RESIZE,1,res_create,resize_script);
  	REGISTERX(VF_TRANSFORM, "addblack",QT_TR_NOOP("Add black borders"),QT_TR_NOOP("Add pure black border(s). Enlarges the picture."),VF_ADDBORDER,1,addBorder_create,addBorder_script);
	REGISTERX(VF_TRANSFORM, "blacken",QT_TR_NOOP("Blacken borders"),QT_TR_NOOP("Fill borders with pure black. Doesn't alter size."),VF_BLACKEN,1,bsmear_create,bsmear_script);
	REGISTERX(VF_TRANSFORM, "rotate",QT_TR_NOOP("Rotate"),QT_TR_NOOP("Rotate the picture by 90, 180 or 270 degrees."),VF_ROTATE,1,rotate_create,rotate_script);
    REGISTERX(VF_TRANSFORM, "resamplefps",QT_TR_NOOP("Resample fps"),QT_TR_NOOP("Change framerate while keeping duration."),VF_RESAMPLE_FPS,1,resamplefps_create,resamplefps_script);                
    REGISTERX(VF_TRANSFORM, "reverse",QT_TR_NOOP("Reverse"),QT_TR_NOOP("Play video backward."),VF_REVERSE,1,reverse_create,reverse_script);
    REGISTERX(VF_TRANSFORM, "fade",QT_TR_NOOP("Fade"),QT_TR_NOOP("Fade in/out."),VF_FADE,1,fade_create,fade_script);     

    REGISTERX(VF_INTERLACING, "YADIF",QT_TR_NOOP("yadif"),QT_TR_NOOP("Yet Another DeInterlacer. Ported from MPlayer."),VF_YADIF,1,yadif_create,yadif_script);
    REGISTERX(VF_INTERLACING, "mcdeinterlace",QT_TR_NOOP("mcDeinterlace"),QT_TR_NOOP("Motion compensation deinterlacer. Ported from MPlayer."),VF_MCDEINT,1,mcdeint_create,mcdeint_script);
	REGISTERX(VF_INTERLACING, "deinterlace",QT_TR_NOOP("Deinterlace"),QT_TR_NOOP("Mask interlacing artifacts. Port of Smart deinterlace."),VF_DEINTERLACE,1,deinterlace_create,deinterlace_script);
	REGISTERX(VF_INTERLACING, "kerneldeint",QT_TR_NOOP("KernelDeint"),QT_TR_NOOP("Kernel deinterlacer by Donald Graft."),VF_KRNDEINT,1,kerneldeint_create,kerneldeint_script);
    REGISTERX(VF_INTERLACING, "tdeint",QT_TR_NOOP("TDeint"),QT_TR_NOOP("Motion adaptative deinterlacer by Tritical."),VF_TDEINT,1,tdeint_create,tdeint_script);
    REGISTERX(VF_INTERLACING, "lavcppdeint",QT_TR_NOOP("libavcodec deinterlacer"),QT_TR_NOOP("All FFmpeg deinterlace filters (bicubic, median, ...)."),VF_LAVPP_DEINT,1,lavppdeint_create,lavppdeint_script);
	REGISTERX(VF_INTERLACING, "telecide",QT_TR_NOOP("Decomb Telecide"),QT_TR_NOOP("Great deinterlacing package including IVTC."),VF_DECOMB,1,decomb_create,decomb_script);
	REGISTERX(VF_INTERLACING, "decimate",QT_TR_NOOP("Decomb Decimate"),QT_TR_NOOP("Useful to remove dupes left by Telecide."),VF_DECIMATE,1,decimate_create,decimate_script);
	REGISTERX(VF_INTERLACING, "pulldown",QT_TR_NOOP("Pulldown"),QT_TR_NOOP("Convert 24 fps to 30 fps by repeating fields."),VF_PULLDOWN,1,pulldown_create,pulldown_script);	
	REGISTERX(VF_INTERLACING, "dgbob","DGBob",QT_TR_NOOP("Bobber - convert each field into a full sized frame."),VF_DGBOB,1,dgbob_create,dgbob_script);
    REGISTERX(VF_INTERLACING, "palfieldshift",QT_TR_NOOP("PAL field shift"),QT_TR_NOOP("Shift fields by one. Useful for some PAL movies."),VF_PALSHIFT,1,addPALShift_create,addPALShift_script);	
	REGISTERX(VF_INTERLACING, "palsmart",QT_TR_NOOP("PAL smart"),QT_TR_NOOP("Smartly revert non constant PAL field shift."),VF_TELECIDE,1,telecide_create,telecide_script);

	//	REGISTERX("lavdeint","Lavcodec Deinterlacer",VF_LAVDEINT,1,lavdeint_create,lavdeint_script);
        
        
	
	REGISTERX(VF_INTERLACING, "drop",QT_TR_NOOP("Drop"),QT_TR_NOOP("Drop damaged fields (e.g. from VHS capture)."),VF_DROPOUT,1,dropout_create,dropout_script);
	REGISTERX(VF_INTERLACING, "swapfields",QT_TR_NOOP("Swap fields"),QT_TR_NOOP("Swap top and bottom fields."),VF_SWAPFIELDS,1,swapfield_create,swapfield_script);
	REGISTERX(VF_INTERLACING, "smartswapfield",QT_TR_NOOP("Smart swap fields"),QT_TR_NOOP("Smartly swap fields. Needed when field order changes."),VF_SMARTSWAPFIELDS,1,swapsmart_create,swapsmart_script);

	REGISTERX(VF_INTERLACING, "keepevenfield",QT_TR_NOOP("Keep even fields"),QT_TR_NOOP("Keep top fields. Gives a half height picture."),VF_KEEPEVEN,1,keepeven_create,keepeven_script);
	REGISTERX(VF_INTERLACING, "keepoddfield",QT_TR_NOOP("Keep odd fields"),QT_TR_NOOP("Keep bottom fileds. Gives a half height picture."),VF_KEEPODD,1,keepodd_create,keepodd_script);
	REGISTERX(VF_INTERLACING, "separatefields",QT_TR_NOOP("Separate fields"),QT_TR_NOOP("Each field becomes full picture, half sized."),VF_SEPARATEFIELDS,1, separatefield_create,separatefield_script);
	REGISTERX(VF_INTERLACING, "mergefield",QT_TR_NOOP("Merge fields"),QT_TR_NOOP("Merge two pictures as if they were two fields."),VF_MERGEFIELDS,1,mergefield_create,mergefield_script);

    REGISTERX(VF_INTERLACING, "hzstackfield",QT_TR_NOOP("Hz Stack fields"),QT_TR_NOOP("Put botj fields side by side."),VF_HZSTACKFIELD,1,hzstackfield_create,hzstackfield_script);
        
	REGISTERX(VF_INTERLACING, "stackfield",QT_TR_NOOP("Stack fields"),QT_TR_NOOP("Put two fields on top of one another."),VF_STACKFIELD,1,stackfield_create,stackfield_script);
    REGISTERX(VF_INTERLACING, "unstackfield",QT_TR_NOOP("Unstack fields"),QT_TR_NOOP("Interleave top and bottom part of the picture."),VF_UNSTACK_FIELD,1,unstackfield_create,unstackfield_script);

    
    
    REGISTERX(VF_COLORS, "eq2",QT_TR_NOOP("MPlayer eq2"),QT_TR_NOOP("Adjust contrast, brightness, saturation and gamma."),VF_EQ2,1,Eq2_create,Eq2_script);
    REGISTERX(VF_COLORS, "hue",QT_TR_NOOP("MPlayer hue"),QT_TR_NOOP("Adjust hue and saturation."),VF_HUE,1,hue_create,hue_script);
    REGISTERX(VF_COLORS, "contrast",QT_TR_NOOP("Contrast"),QT_TR_NOOP("Adjust contrast, brightness and colors."),VF_CONTRAST,1,contrast_create,contrast_script);
    REGISTERX(VF_COLORS, "equalizer",QT_TR_NOOP("Luma equalizer"),QT_TR_NOOP("Luma correction filter with histogram."),VF_EQUALIZER,1,equalizer_create,equalizer_script);

    REGISTERX(VF_COLORS, "swapuv",QT_TR_NOOP("Swap U and V"),QT_TR_NOOP("Invert chroma U and chroma V."),VF_SWAPUV,1,swapuv_create,swapuv_script);
    REGISTERX(VF_COLORS, "chromashift",QT_TR_NOOP("Chroma shift"),QT_TR_NOOP("Shift chroma U/V to fix badly synced luma/chroma."),VF_CHROMASHIFT,1,create_chromashift,chromashift_script);

    REGISTERX(VF_COLORS, "lumaonly",QT_TR_NOOP("Luma only"),QT_TR_NOOP("Convert picture to greyscale (black and white)."),VF_LUMA,1,luma_create,luma_script);
    REGISTERX(VF_COLORS, "chromauonly",QT_TR_NOOP("Chroma U only"),QT_TR_NOOP("Convert picture to grey, keeping only chroma U."),VF_CHROMAU,1,chromaU_create,chromaU_script);
    REGISTERX(VF_COLORS, "chromavonly",QT_TR_NOOP("Chroma V only"),QT_TR_NOOP("Convert picture to grey, keeping only chroma V."),VF_CHROMAV,1,chromaV_create,chromaV_script);
   
    REGISTERX(VF_COLORS, "delta",QT_TR_NOOP("Luma delta"),QT_TR_NOOP("Difference between current and previous picture."),VF_DELTA,1,delta_create,delta_script);
    REGISTERX(VF_COLORS, "coloryuv",QT_TR_NOOP("Avisynth ColorYUV"),QT_TR_NOOP("Alter colors (auto white balance etc...). Ported from Avisynth."),VF_COLOR_YUV,1,coloryuv_create,coloryuv_script);

    
    REGISTERX(VF_NOISE, "mpdenoise3d",QT_TR_NOOP("MPlayer denoise3d"),QT_TR_NOOP("Reduce noise, smooth image, increase compressibility."),VF_MPLLQD3D,1,MPD3Dlow_create,MPD3Dlow_script);
	REGISTERX(VF_NOISE, "mphqdenoise3d",QT_TR_NOOP("MPlayer hqdn3d"),QT_TR_NOOP("High quality version of denoise3d. Slower but more precise."),VF_MPLHQD3D,1,MPD3D_create,MPD3D_script);
	REGISTERX(VF_NOISE, "fluxsmooth",QT_TR_NOOP("FluxSmooth"),QT_TR_NOOP("Spatio-temporal cleaner by Ross Thomas."),VF_FLUXSMOOTH,1,fluxsmooth_create,fluxsmooth_script);
#ifdef ADM_CPU_X86
    	REGISTERX(VF_NOISE, "temporalcleaner",QT_TR_NOOP("Temporal Cleaner"),QT_TR_NOOP("Vlad59's Avisynth port of Jim Casaburi's denoiser."),VF_VLADSMOOTH,1,vladsmooth_create,vladsmooth_script);
#endif
	REGISTERX(VF_NOISE, "denoise",QT_TR_NOOP("Denoise"),QT_TR_NOOP("Port of Transcode DNR."),VF_DENOISE,1,denoise_create,denoise_script);
	REGISTERX(VF_NOISE, "stabilize",QT_TR_NOOP("Stabilize"),QT_TR_NOOP("Light denoiser."),VF_STABILIZE,1,stabilize_create,stabilize_script);
    REGISTERX(VF_NOISE, "cnr2",QT_TR_NOOP("Cnr2"),QT_TR_NOOP("Chroma noise reduction filter by MarcFD/Tritical."),VF_CNR2,1,cnr2_create,cnr2_script);	

	REGISTERX(VF_SHARPNESS, "msmooth",QT_TR_NOOP("MSmooth by Donald Graft"),QT_TR_NOOP("Smooth the image, don't blur edges. Useful on anime."),VF_MSMOOTH,1,create_msmooth,msmooth_script);
	REGISTERX(VF_SHARPNESS, "soften",QT_TR_NOOP("Soften"),QT_TR_NOOP("A variant of stabilize."),VF_SOFTEN,1,soften_create,soften_script);

	//REGISTERX("smoothclean","Smooth Clean",VF_SMOOTHCLEAN,0,smooth_create,smooth_script);
    REGISTERX(VF_SHARPNESS, "sharpen",QT_TR_NOOP("Sharpen"),QT_TR_NOOP("Enhance difference between near pixels."),VF_SHARPEN,1,sharpen_create,sharpen_script);
    REGISTERX(VF_SHARPNESS, "msharpen",QT_TR_NOOP("MSharpen"),QT_TR_NOOP("Sharpen edges without amplifying noise. By Donald Graft."),VF_MSHARPEN,1,msharpen_create,msharpen_script);
    REGISTERX(VF_SHARPNESS, "asharp",QT_TR_NOOP("asharp"),QT_TR_NOOP("Adaptative sharpener by MarcFD."),VF_ASHARP,1,asharp_create,asharp_script);

    REGISTERX(VF_SHARPNESS, "gaussian",QT_TR_NOOP("Gauss smooth"),QT_TR_NOOP("Gaussian smooth. Blur the picture."),VF_GAUSSIAN,1,Gaussian_create,gaussian_script);
    REGISTERX(VF_SHARPNESS, "mean",QT_TR_NOOP("Mean"),QT_TR_NOOP("Mean (blur) kernel."),VF_MEAN,1,mean_create,mean_script);
    REGISTERX(VF_SHARPNESS, "median",QT_TR_NOOP("Median"),QT_TR_NOOP("Median kernel 3x3. Remove high frequency noise."),VF_MEDIAN,1,median_create,median_script);
    REGISTERX(VF_SHARPNESS, "largemedian",QT_TR_NOOP("Median (5x5)"),QT_TR_NOOP("Median kernel 5x5. Good for reducing chroma noise."),VF_LARGEMEDIAN,1,largeMedian_create,largeMedian_script);
    REGISTERX(VF_SHARPNESS, "forcedpp",QT_TR_NOOP("Forced postprocessing"),QT_TR_NOOP("Apply blind postprocessing."),VF_FORCEDPP,1,forcedpp_create,forcedpp_script);        

#ifdef USE_FREETYPE   
	REGISTERX(VF_SUBTITLE, "subtitle",QT_TR_NOOP("Subtitler"),QT_TR_NOOP("Add srt/sub subtitles to the picture."),VF_SUBTILE,1,subtitle_create,subtitle_script);
	REGISTERX(VF_SUBTITLE, "ass",QT_TR_NOOP("ASS"),QT_TR_NOOP("Add ASS/SSA subtitles to the picture."),VF_ASS,1,ass_create,ass_script);
#endif

        REGISTERX(VF_SUBTITLE,"vobsub",QT_TR_NOOP("VobSub"),QT_TR_NOOP("Embed VobSub subtitles into picture."),VF_VOBSUB,1,vobsub_create,vobsub_script);
#if 1 //def ADM_DEBUG
        REGISTERX(VF_SUBTITLE,"dvbsub",QT_TR_NOOP("DVB sub"),QT_TR_NOOP("Embed DVB-T subtitle."),VF_DVBSUB,1,dvbsub_create,dvbsub_script);
#else
        FILTERDEC(dvbsub_create);
        FILTERDECX(dvbsub_script) ;
#endif

        //*********************
 
    REGISTERX(VF_MISC, "blendremover",QT_TR_NOOP("Blend remover"),QT_TR_NOOP("Remove blend between previous and next picture."),VF_BLENDREMOVAL,1,blendremove_create,blendremove_script);
    REGISTERX(VF_MISC, "hardivtcremove",QT_TR_NOOP("Hard pulldown removal"),QT_TR_NOOP("Remove IVTC that has been analog captured or resized."),VF_HARDIVTC,1,hardivtc_create,hardivtc_script);
    REGISTERX(VF_MISC, "whirl",QT_TR_NOOP("Whirl"),QT_TR_NOOP("Useless whirlwind effect."),VF_WHIRL,1,whirl_create,whirl_script);
    REGISTERX(VF_MISC, "mosaic",QT_TR_NOOP("Mosaic"),QT_TR_NOOP("Split the picture into tiny thumbnails."),VF_MOSAIC,1,     mosaic_create,mosaic_script);
    REGISTERX(VF_MISC, "mpdelogo",QT_TR_NOOP("MPlayer delogo"),QT_TR_NOOP("Blend a logo by interpolating its surrounding box."),VF_MPDELOGO,1,mpdelogo_create,mpdelogo_script);
    REGISTERX(VF_MISC, "logo",QT_TR_NOOP("Logo"),QT_TR_NOOP("Add a png as logo."),VF_LOGO,1,logo_create,logo_script);
    REGISTERX(VF_MISC, "animatedmenu",QT_TR_NOOP("Animated Menu"), QT_TR_NOOP("Create a video made of 6 mini windows, very useful to do DVD menus."),
            VF_ANIMATED,1,animated_create,animated_script);
	REGISTERX(VF_MISC, "computeaverage",QT_TR_NOOP("Compute Average"),QT_TR_NOOP("Compute average luma value for each pixel throughout all frames, and save to file"),VF_COMPUTEAVERAGE,1,computeaverage_create,computeaverage_script);
	REGISTERX(VF_MISC, "threshold",QT_TR_NOOP("Threshold"),QT_TR_NOOP("Force too-bright/too-dim pixels to 0 and the rest to 255 or vice-versa"),VF_THRESHOLD,1,threshold_create,threshold_script);
	//REGISTERX("particlelist",QT_TR_NOOP("Particle List"),QT_TR_NOOP("Identify 'particles' (groups of pixels)"),VF_PARTICLELIST,1,particle_create,particle_script);
	//REGISTERX("eraser",QT_TR_NOOP("Eraser"),QT_TR_NOOP("\"Erase\" arbitrary areas of each frame"),VF_ERASER,1,eraser_create,eraser_script);
	//REGISTERX("swissarmyknife",QT_TR_NOOP("Swiss Army Knife"),QT_TR_NOOP("Apply operation like P'=P*A, P-A, P+A, etc. using a convolution, rolling average, image from file, or constant"),VF_SWISSARMYKNIFE,1,swissarmyknife_create,swissarmyknife_script);


// Does not work
//        REGISTERX("unblend","Unblend by Bach",VF_UNBLEND,1,unblend_create,unblend_script);
        
	REGISTERX(VF_MISC,"partial",QT_TR_NOOP("Partial"),"",VF_PARTIAL,0,partial_create,partial_script);
//	REGISTER("","Remove Salt",VF_SALT,0,salt_create);
 printf("\n");
}		


// EOF
