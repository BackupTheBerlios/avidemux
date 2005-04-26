/***************************************************************************
                          video_filters.h  -  description
                             -------------------
    begin                : Wed Mar 27 2002
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
 #ifndef __VIDEO_FILTERS__
 #define  __VIDEO_FILTERS__

 #define MAX_FILTER 80
#include "ADM_script/adm_scanner.h"
 	typedef enum
 	{
		VF_INVALID=0,
 	      	VF_RESIZE=1,
 	      	VF_CROP,
	      	VF_TELECIDE,
                VF_BLACKEN,
                VF_SHARPEN,
                VF_MEAN,
                VF_MEDIAN,
                VF_GAUSSIAN,
                VF_SMOOTHCLEAN,
                VF_DEINTERLACE,
                VF_ADDBORDER,
                VF_PALSHIFT,
                VF_SWAPUV,
                VF_CONTRAST,
                VF_LUMA,
                VF_CHROMAU,
                VF_CHROMAV,
                VF_STABILIZE,
                VF_FLIP,
                VF_DENOISE,
                VF_SUBTILE,
                VF_LARGEMEDIAN,
                VF_FLUXSMOOTH,
                VF_SALT,
		VF_PARTIAL          ,
		VF_VLADSMOOTH,
		VF_ROTATE,
		VF_DROPOUT,
		VF_SWAPFIELDS,
		VF_KEEPODD,
		VF_KEEPEVEN,
		VF_SEPARATEFIELDS,
		VF_MERGEFIELDS,
		VF_CHROMASHIFT,
		VF_SMARTSWAPFIELDS,
		VF_MPLAYERRESIZE,
		VF_IVTC,
		VF_PULLDOWN,
		VF_MPLHQD3D,
		VF_MPLLQD3D,
		VF_LAVDEINT,
		VF_MPDETC,
		VF_KRNDEINT,
		VF_FORCEDPP,
		VF_SOFTEN,
		VF_DECOMB,
		VF_DECIMATE,
		VF_MSMOOTH,
		VF_STACKFIELD,
		VF_WHIRL,
		VF_EQUALIZER,
                VF_VOBSUB,
                VF_CHFPS,
                VF_RESAMPLE_FPS,
                VF_LAVPP_DEINT,
                VF_UNSTACK_FIELD,
                VF_HUE,
                VF_EQ2,
                VF_DGBOB,
                VF_MSHARPEN,
		VF_DUMMY
 	 }VF_FILTERS;
 	
   typedef struct
   {
       		const char 		*name;
		AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, CONFcouple *);
		VF_FILTERS 	tag;
		uint8_t		viewable;
		char		*filtername;
		AVDMGenericVideoStream *(*create_from_script) (AVDMGenericVideoStream *in, int n,Arg *args);	
   }FILTER_ENTRY;
 	
 typedef struct
   {
   		VF_FILTERS				tag;
		AVDMGenericVideoStream 	*filter;
		CONFcouple				*conf;

   }FILTER; 	
   
   typedef struct
   {
   		uint32_t nb;
		const char 	*param[20];
   }FILTER_PARAM;
   
  AVDMGenericVideoStream *getLastVideoFilter( uint32_t frameStart, uint32_t nbFrame);
    AVDMGenericVideoStream *getLastVideoFilter( void );
 AVDMGenericVideoStream *getFirstVideoFilter( uint32_t frameStart, uint32_t nbFrame);
  AVDMGenericVideoStream *getFirstVideoFilter( void);
  AVDMGenericVideoStream *getFirstCurrentVideoFilter( void);
  
  void	filterSetPostProc( void );
  
#ifdef USE_LIBXML2
void filterSaveXml(char *name);
void filterSaveXml(char *name,uint8_t silent);
void filterLoadXml(char *name);
int  filterLoadXml(char *name,uint8_t silent);
#endif
 void filterSave(char *name);
 void filterLoad(char *name);
 int filterLoad(char *name,uint8_t silent);
 void filterSave(char *name,uint8_t silent);
CONFcouple *filterBuildCouple(FILTER_PARAM *param,uint32_t n,Arg *args);
void filterSaveScript(char *name);
void registerFilter(const char *name,VF_FILTERS tag,uint8_t viewable,
			AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, CONFcouple *),char *fname);
void registerFilterEx(const char *name,VF_FILTERS tag,uint8_t viewable
		,AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, CONFcouple *)
		,char *filtername,AVDMGenericVideoStream *(*create_from_script) (AVDMGenericVideoStream *in, int n,Arg *args));
void 		filterListAll( void );
VF_FILTERS 	filterGetTagFromName(char *inname);
uint8_t 	filterAddScript(VF_FILTERS tags,uint32_t n,Arg *args);
//AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,uint8_t *conf, AVDMGenericVideoStream *in) ;

 #endif
