
/***************************************************************************
                          DIA_none
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

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADM_default.h"

#include "ADM_assert.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCNR2_param.h"
#include "ADM_video/ADM_vidColorYuv_param.h"
#include "ADM_video/ADM_vidPartial_param.h"
#include "ADM_video/ADM_vidEqualizer.h"
#include "ADM_video/ADM_vidMPdelogo.h"

#include "ADM_videoFilter/ADM_vidAnimated_param.h"
#include "ADM_videoFilter/ADM_vidASharp_param.h"
#include "ADM_videoFilter/ADM_vidChromaShift_param.h"
#include "ADM_videoFilter/ADM_vidContrast.h"
#include "ADM_videoFilter/ADM_vidEq2.h"
#include "ADM_videoFilter/ADM_vidHue.h"
#include "ADM_videoFilter/ADM_vobsubinfo.h"
#include "ADM_videoFilter/ADM_vidVobSub.h"

#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_audiofilter/audioencoder.h"
#include "ADM_lavcodec.h"
#include "ADM_codecs/ADM_ffmpegConfig.h"

#include "ADM_ocr/ADM_ocr.h"
#include "ADM_ocr/ADM_ocrInternal.h"

extern int global_argc;
extern char **global_argv;
extern void initTranslator(void);
extern int automation(void);

int SliderIsShifted = 0;

int DIA_coloryuv(COLOR_YUV_PARAM *param) {return 0;}
int DIA_getCropParams(  char *name,CROP_PARAMS *param,AVDMGenericVideoStream *in) {return 0;}

#ifdef USE_XX_XVID 
#include "xvid.h"
int  DIA_getXvidCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,xvidEncParam *param) {return 0;}
#endif

uint8_t DIA_animated(ANIMATED_PARAM *param) {return 0;}
uint8_t DIA_cnr2(CNR2Param *param) {return 0;}
uint8_t DIA_getASharp(ASHARP_PARAM *param, AVDMGenericVideoStream *in) {return 0;}
uint8_t DIA_getEQ2Param(Eq2_Param *param, AVDMGenericVideoStream *in) {return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, ADMImage *image) {return 0;}
uint8_t DIA_getEqualizer(EqualizerParam *param, AVDMGenericVideoStream *in) {return 0;}
uint8_t DIA_getHue(Hue_Param *param, AVDMGenericVideoStream *in) {return 0;}
uint8_t DIA_getMPdelogo(MPDELOGO_PARAM *param,AVDMGenericVideoStream *in) {return 0;}
uint8_t DIA_getPartial(PARTIAL_CONFIG *param,AVDMGenericVideoStream *son,AVDMGenericVideoStream *previous) {return 0;}
uint8_t DIA_pipe(char **cmd,char **param) {return 0;}
uint8_t DIA_vobsub(vobSubParam *param) {return 0;}
uint8_t DIA_x264(COMPRES_PARAMS *config) {return 0;}
uint8_t DIA_XVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf) {return 0;}
uint8_t DIA_xvid4(COMPRES_PARAMS *incoming) {return 0;}
uint8_t DIA_quota(char *) {return 0;}
uint8_t  DIA_job_select(char **jobname, char **filename) {return 0;}
const char * GUI_getCustomScript(uint32_t nb) {return 0;}
uint8_t DIA_RecentFiles( char **name ) {return 0;}
uint8_t DIA_about( void ) {return 0;}
void DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate ) {}
void DIA_properties(void) {}
int GUI_handleVFilter (void) {return 0;}
uint8_t initGUI(void) {return 1;}
void destroyGUI(void) {}
uint8_t DIA_job(uint32_t nb,char **name) {return 0;}
uint8_t DIA_resize(uint32_t *width,uint32_t *height,uint32_t *algo,uint32_t originalw, 
                        uint32_t originalh,uint32_t fps) {return 0;}
uint8_t DIA_d3d(double *luma,double *chroma,double *temporal) {return 0;}
uint8_t DIA_kerneldeint(uint32_t *order, uint32_t *threshold, uint32_t *sharp, 
                          uint32_t *twoway, uint32_t *map) {return 0;}
uint8_t DIA_4entries(char *title,uint32_t *left,uint32_t *right,uint32_t *top,uint32_t *bottom) {return 0;}
uint8_t DIA_videoCodec (SelectCodecType * codec) {return 0;}
uint8_t DIA_getChromaShift( AVDMGenericVideoStream *instream,CHROMASHIFT_PARAM *param ) {return 0;}
uint8_t DIA_contrast(AVDMGenericVideoStream *astream,CONTRAST_PARAM *param) {return 0;}
uint8_t DIA_audioCodec( AUDIOENCODER *codec ) {return 0;}
uint8_t DIA_dnr(uint32_t *llock,uint32_t *lthresh, uint32_t *clock,
			uint32_t *cthresh, uint32_t *scene) {return 0;}
int DIA_srtPos(AVDMGenericVideoStream *source,uint32_t *size,uint32_t *position) {return 0;}
int DIA_colorSel(uint8_t *r, uint8_t *g, uint8_t *b) {return 0;}			
uint8_t DIA_glyphEdit(void) {return 0;}
struct THRESHOLD_PARAM;
struct ADMVideoThreshold;
uint8_t DIA_threshold(AVDMGenericVideoStream *in,
                      ADMVideoThreshold * thresholdp,
                      THRESHOLD_PARAM * param) { return 255; }
struct SWISSARMYKNIFE_PARAM;
struct ADMVideoSwissArmyKnife;
struct MenuMapping;
uint8_t DIA_SwissArmyKnife (AVDMGenericVideoStream * in,
                            ADMVideoSwissArmyKnife * sakp,
                            SWISSARMYKNIFE_PARAM * param,
                            const MenuMapping * menu_mapping,
                            uint32_t menu_mapping_count) { return 255; }
struct ERASER_PARAM;
struct ADMVideoEraser;
uint8_t DIA_eraser (AVDMGenericVideoStream * in,
                    ADMVideoEraser * eraserp,
                    ERASER_PARAM * param,
                    const MenuMapping * menu_mapping,
                    uint32_t menu_mapping_count) { return 255; }
struct PARTICLE_PARAM;
struct ADMVideoParticle;
uint8_t DIA_particle (AVDMGenericVideoStream *in,
                      ADMVideoParticle * particlep,
                      PARTICLE_PARAM * param,
                      const MenuMapping * menu_mapping,
                      uint32_t menu_mapping_count) { return 255; }
uint8_t ADM_ocrUpdateNbLines(void *ui,uint32_t cur,uint32_t total) {return 0;}
uint8_t ADM_ocrUpdateNbGlyphs(void *ui,uint32_t nbGlyphs) {return 0;}
uint8_t ADM_ocrUpdateTextAndTime(void *ui,char *decodedString,char *timeCode) {	return 0;}
uint8_t ADM_ocrDrawFull(void *d,uint8_t *data) {return 0;}
uint8_t ADM_ocrUiEnd(void *d) {	return 0;}
void *ADM_ocrUiSetup(void) {return 0;}
uint8_t ADM_ocrSetRedrawSize(void *ui,uint32_t w,uint32_t h) {return 0;}
ReplyType glyphToText(admGlyph *glyph,admGlyph *head,char *decodedString) {return ReplyOk;}

int UI_Init(int nargc, char **nargv)
{
	initTranslator();

	global_argc = nargc;
	global_argv = nargv;

	return 0;
}

int UI_RunApp(void)
{
	if (global_argc >= 2)
		automation();

	printf("*********************************\n");
	printf("*********************************\n");
	printf("End of program..\n");
	printf("*********************************\n");
	printf("*********************************\n");
}

//EOF
