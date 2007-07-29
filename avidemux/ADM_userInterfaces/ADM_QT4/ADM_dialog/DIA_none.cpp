
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


#include "ADM_videoFilter/ADM_vidAnimated_param.h"
#include "ADM_video/ADM_vidASharp_param.h"
#include "ADM_audiofilter/audiofilter_normalize_param.h"
#include "ADM_video/ADM_vidCNR2_param.h"
#include "ADM_video/ADM_vidColorYuv_param.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_encoder/adm_encmjpeg_param.h"
#include "ADM_video/ADM_vidPartial_param.h"
#include "ADM_video/ADM_vidEqualizer.h"
#include "ADM_video/ADM_vidHue.h"
#include "ADM_video/ADM_vobsubinfo.h"
#include "ADM_video/ADM_vidVobSub.h"
#include "ADM_videoFilter/ADM_vidChromaShift_param.h"
#include "ADM_video/ADM_vidMPdelogo.h"
#include "ADM_video/ADM_vidEq2.h"
#include "ADM_video/ADM_vidContrast.h"
//#include "ADM_video/ADM_vidFont.h"
class ADMfont;
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


#ifdef USE_XX_XVID 
#include "xvid.h"
int  DIA_getXvidCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,xvidEncParam *param){return 0;}
#endif
int     DIA_coloryuv(COLOR_YUV_PARAM *param){return 0;}
uint8_t DIA_cnr2(CNR2Param *param){return 0;}
uint8_t DIA_getEQ2Param(Eq2_Param *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, ADMImage *image){return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, AVDMGenericVideoStream *in){return 0;}
uint8_t DIA_getMPdelogo(MPDELOGO_PARAM *param,AVDMGenericVideoStream *in){return 0;}
//uint8_t DIA_getPartial(PARTIAL_CONFIG *param,AVDMGenericVideoStream *son,AVDMGenericVideoStream *previous){return 0;}
//uint8_t DIA_vobsub(vobSubParam *param){return 0;}
//uint8_t DIA_x264(COMPRES_PARAMS *config){return 0;}
//uint8_t DIA_job(uint32_t nb,char **name){return 0;}
uint8_t DIA_videoCodec (SelectCodecType * codec){return 0;}
uint8_t DIA_audioCodec( AUDIOENCODER *codec ) {return 0;}
//uint8_t DIA_srt(ADMImage *source, SUBCONF *param) {return 0;}			
uint8_t DIA_quota(char *) {return 0;}
uint8_t  DIA_job_select(char **jobname, char **filename) {return 0;}
//const char * GUI_getCustomScript(uint32_t nb) {return 0;}
uint8_t DIA_RecentFiles( char **name ){return 0;}
uint8_t DIA_about( void ){return 0;}
void    DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate ){return ;}
uint8_t initFileSelector(void){return 0;}
uint8_t initGUI( void ){return 1;}
void destroyGUI(void) {}
int DIA_colorSel(uint8_t *r, uint8_t *g, uint8_t *b) {return 0;}
uint8_t DIA_glyphEdit(void){ return 0;};

int32_t UI_readJog(void)
{
  return 0; 
}
#include "ADM_ocr/ADM_ocr.h"
uint8_t ADM_ocr_engine(   ADM_OCR_SOURCE & source,const char *labelSrt,admGlyph *head)
{
  return 0; 
}

//EOF

