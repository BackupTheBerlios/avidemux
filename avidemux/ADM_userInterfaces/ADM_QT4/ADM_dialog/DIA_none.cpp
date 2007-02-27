
/***************************************************************************
                          DIA_animated
    copyright            : (C) 2006 by mean
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



#include <config.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "default.h"

# include <math.h>


#include "ADM_image.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_toolkit/filesel.h"


#include "ADM_video/ADM_vidAnimated_param.h"
#include "ADM_video/ADM_vidASharp_param.h"
#include "ADM_audiofilter/audiofilter_normalize_param.h"
#include "ADM_video/ADM_vidCNR2_param.h"
#include "ADM_video/ADM_vidColorYuv_param.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_encoder/adm_encmjpeg_param.h"
#include "ADM_video/ADM_vidPartial_param.h"
#include "ADM_video/ADM_vidTDeint_param.h"
#include "ADM_video/ADM_vidRotate_param.h"
#include "ADM_video/ADM_vidEqualizer.h"
#include "ADM_video/ADM_vidHue.h"
#include "ADM_video/ADM_vobsubinfo.h"
#include "ADM_video/ADM_vidVobSub.h"
#include "ADM_video/ADM_vidChromaShift_param.h"
#include "ADM_video/ADM_vidMPdelogo.h"
#include "ADM_video/ADM_vidEq2.h"
#include "ADM_video/ADM_vidContrast.h"
#include "ADM_video/ADM_vidFont.h"
#include "ADM_video/ADM_vidSRT.h"
#include "ADM_audiofilter/audioencoder_vorbis_param.h"    
    
#include "ADM_audiofilter/audioeng_buildfilters.h"
    
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_audiofilter/audioencoder.h"
#include "ADM_lavcodec.h"
#include "ADM_codecs/ADM_ffmpegConfig.h"

#include "DIA_resizeWiz.h"
    
#include "ADM_assert.h"
int SliderIsShifted=0;

//******************************
int DIA_coloryuv(COLOR_YUV_PARAM *param){return 0;}
 int DIA_getAudioFilter(GAINparam *normalized, RESAMPLING *downsamplingmethod, int *tshifted,
  			 int *shiftvalue, int *drc,int *freqvalue,FILMCONV *filmconv,CHANNEL_CONF *channel) {return 0;}

int DIA_getCropParams(  char *name,CROP_PARAMS *param,AVDMGenericVideoStream *in){return 0;}
#ifdef USE_XX_XVID 
#include "xvid.h"
int  DIA_getXvidCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,xvidEncParam *param){return 0;}
#endif
uint8_t DIA_animated(ANIMATED_PARAM *param){return 0;}
uint8_t DIA_rotate(AVDMGenericVideoStream *astream,ROTATE_PARAM *param){return 0;}
uint8_t DIA_cnr2(CNR2Param *param){return 0;}
uint8_t DIA_getASharp(ASHARP_PARAM *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getEQ2Param(Eq2_Param *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, ADMImage *image){return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getHue(Hue_Param *param, ADMImage *image){return 0;}
uint8_t DIA_getHue(Hue_Param *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getMPdelogo(MPDELOGO_PARAM *param,AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getPartial(PARTIAL_CONFIG *param,AVDMGenericVideoStream *son,AVDMGenericVideoStream *previous){return 0;}
uint8_t DIA_mjpegCodecSetting(COMPRES_PARAMS *param){return 0;}
uint8_t DIA_pipe(char **cmd,char **param){return 0;}
uint8_t DIA_requant(COMPRES_PARAMS *incoming){return 0;}
uint8_t  DIA_setUserMuxParam( int *mode, int *param, int *muxsize){return 0;}
uint8_t  DIA_tdeint(TDEINT_PARAM *param){return 0;}
uint8_t DIA_vobsub(vobSubParam *param){return 0;}
uint8_t DIA_x264(COMPRES_PARAMS *config){return 0;}
uint8_t DIA_XVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf	){return 0;}
uint8_t DIA_xvid4(COMPRES_PARAMS *incoming){return 0;}

//******************************
uint8_t getFFCompressParams (COMPRES_PARAMS * incoming) {return 0;}
uint8_t DIA_quota(char *) {return 0;}

uint8_t  DIA_job_select(char **jobname, char **filename) {return 0;}
uint8_t DIA_audioTrack(AudioSource *source, uint32_t *track,uint32_t nbTrack, audioInfo *infos){return 0;}
const char * GUI_getCustomScript(uint32_t nb) {return 0;}
uint8_t  DIA_v2v(char **vobname, char **ifoname,char **vobsubname) {return 0;}
uint8_t DIA_RecentFiles( char **name ){return 0;}

uint8_t DIA_about( void ){return 0;}
void DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate ){return ;}
uint8_t DIA_gotoTime(uint16_t *hh, uint16_t *mm, uint16_t *ss){return 0;}
//int GUI_handleVFilter (void){return 0;}
uint8_t  initFileSelector(void){return 0;}
uint8_t initGUI( void ){return 1;}

uint8_t DIA_job(uint32_t nb,char **name){return 0;}
uint8_t DIA_lavDecoder(uint32_t *swapUv, uint32_t *showU){return 0;}
uint8_t DIA_resize(uint32_t *width,uint32_t *height,uint32_t *algo,uint32_t originalw, 
                        uint32_t originalh,uint32_t fps) {return 0;}
uint8_t DIA_d3d(double *luma,double *chroma,double *temporal){return 0;}
uint8_t DIA_kerneldeint(uint32_t *order, uint32_t *threshold, uint32_t *sharp, 
                          uint32_t *twoway, uint32_t *map){return 0;}
uint8_t DIA_4entries(char *title,uint32_t *left,uint32_t *right,uint32_t *top,uint32_t *bottom) {return 0;}
uint8_t DIA_videoCodec (SelectCodecType * codec){return 0;}
uint8_t DIA_getChromaShift( AVDMGenericVideoStream *instream,CHROMASHIFT_PARAM    *param ){ return 0;}
uint8_t DIA_contrast(AVDMGenericVideoStream *astream,CONTRAST_PARAM *param)  { return 0;}
uint8_t DIA_audioCodec( AUDIOENCODER *codec ) {return 0;}
uint8_t DIA_resizeWiz(RESWIZ_FORMAT *format, RESWIZ_AR *source, RESWIZ_AR *destination) {return 0;}
uint8_t DIA_dnr(uint32_t *llock,uint32_t *lthresh, uint32_t *clock,
			uint32_t *cthresh, uint32_t *scene) {return 0;}
			
uint8_t DIA_srt(ADMImage *source, SUBCONF *param) {return 0;}			
			
			
//EOF

