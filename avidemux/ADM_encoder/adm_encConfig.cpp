/***************************************************************************
                          adm_encConfig.cpp  -  description
                             -------------------
    begin                : Thu Dec 26 2002
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
#include <config.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>


#include "gui_action.hxx"
#ifdef USE_FFMPEG
#include "ADM_lavcodec.h"
#endif

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encConfig.h"
#include "prefs.h"
#ifdef USE_DIVX
#include "ADM_codecs/ADM_divxEncode.h"
#include "ADM_encoder/adm_encdivx.h"
#endif

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ENCODER
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_dialog/DIA_enter.h"

extern void UI_setVProcessToggleStatus( uint8_t status );
extern void GUI_setVideoCodec(Action action );
static void setVideoEncoderSettings(COMPRESSION_MODE mode, uint32_t  param,
								uint32_t extraConf, uint8_t *extraData);
extern int getCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fs);

// Put that in a proper include...
extern void oplug_mpeg_vcd(char *name);
extern void oplug_mpeg_svcd(char *name);
extern void oplug_mpeg_dvd(char *name);
extern void oplug_mpeg_svcdConf( void );
extern void oplug_mpeg_dvdConf( void );

extern void UI_PrintCurrentVCodec(const char *str);
extern void oplug_mpegff_conf(void);

uint32_t encoderGetNbEncoder(void);
const char* encoderGetIndexedName(uint32_t i);
extern void UI_setVideoCodec(int i);

#ifdef USE_XVID_4 
	#include "ADM_codecs/ADM_xvid4.h"
	#include "ADM_codecs/ADM_xvid4param.h"
	#include "ADM_encoder/adm_encXvid4.h"
extern uint8_t DIA_xvid4(COMPRESSION_MODE * mode, uint32_t * qz,  uint32_t * br,uint32_t *fsize,
		xvid4EncParam *param);	

 
#endif
#ifdef USE_XX_XVID
	#include "ADM_codecs/ADM_xvid.h"
	#include "ADM_encoder/adm_encxvid.h"
	#include "xvid.h"

  extern int DIA_getXvidCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,xvidEncParam *param);
#endif


#ifdef USE_FFMPEG
  #include "ADM_codecs/ADM_ffmpeg.h"
	#include "ADM_encoder/adm_encffmpeg.h"

extern int getFFCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,FFcodecSetting *conf);
#endif

#ifdef USE_FFMPEG
    #include "ADM_codecs/ADM_mjpegEncode.h"
	#include "ADM_encoder/adm_encmjpeg.h"
#endif

extern  uint8_t DIA_DVDffParam(COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,
						   FFcodecSetting *conf);

/*
  	Codec settings here

*/
#ifdef USE_FFMPEG
  FFMPEGConfig ffmpegConfig={
  	{COMPRESS_CQ,4,1500,700},
	{
	 ME_EPZS,//	ME
	 0, // 		GMC	
	 1,	// 4MV
	 0,//		_QPEL;	 
	 0,//		_TREILLIS_QUANT
	 2,//		qmin;
	 31,//		qmax;
	 3,//		max_qdiff;
	 0,//		max_b_frames;
	 0, //		mpeg_quant;
	 1, //
	 -2, // 		luma_elim_threshold;
	 1,//
	 -5, 		// chroma_elim_threshold;
	 0.05,		//lumi_masking;
	 1,		// is lumi
	 0.01,		//dark_masking; 
	 1,		// is dark
 	 0.5,		// qcompress amount of qscale change between easy & hard scenes (0.0-1.0
    	 0.5,		// qblur;    amount of qscale smoothing over time (0.0-1.0) 
	0,		// min bitrate in kB/S
	0,		// max bitrate
	0, 		// default matrix
	0, 		// no gop size
	NULL,
	NULL,
	0,		// interlaced
	0,		// WLA: bottom-field-first
	0,		// wide screen
	2,		// mb eval = distortion
	8000,		// vratetol 8Meg
	0,		// is temporal
	0.0,		// temporal masking
	0,		// is spatial
	0.0,		// spatial masking
	0,		// NAQ
	0		// DUMMY 
 	} 
  };

  // mpeg1
  
 FFMPEGConfig ffmpegMpeg1Config={
  	{COMPRESS_CBR,4,1500000,700},
	{
	 ME_EPZS,//	ME
	 0, // 		GMC
	 0,//		_4MV;
	 0,//		_QPEL;
	 0,//		_TREILLIS_QUANT
	 2,//		qmin;
	 31,//		qmax;
	 3,//		max_qdiff;
	 0,//		max_b_frames;
	 1, //		mpeg_quant;
	 1,//
	 -2, // 		luma_elim_threshold;
	 1,//
	 -5, // 		chroma_elim_threshold;
	 0.05,//		lumi_masking;
	 1,
	 0.01,//		dark_masking;
	 1,
 	 0.5,// 	qcompress;  /* amount of qscale change between easy & hard scenes (0.0-1.0)*/
    	 0.5,// 	qblur;      /* amount of qscale smoothing over time (0.0-1.0) */
	(600*1000>>3),		// min bitrate in kB/S
	(2200*1000)>>3,		// max bitrate
	0,					// user_matrix 0->default, 1 tmpg, 2 animé , 3 kvcd
	12,					// Safe gop size limit
	NULL,				// inter & intra matrix, will be set depending on user_matrix
	NULL,
	0,		// interlaced
	0,		// WLA: bottom-field-first
	0,		// wide screen
	2,		// mb eval = distortion
	8000,		// vratetol
	0,
	0.5,		// temporal masking
	0,
	0.5,		// spatial masking
	0,		// NAQ
	0,		// Use xvid ratecontrol
	40,		// buffersize VCD like
	0		// DUMMY
	
	
 	}
  };
  FFMPEGConfig ffmpegMpeg2Config={
  	{COMPRESS_CBR,4,1500000,700},
	{
	 ME_EPZS,//	ME
	 0, // 		GMC
	 0,//		_4MV;
	 0,//		_QPEL;
	 0,//		_TREILLIS_QUANT
	 2,//		qmin;
	 31,//		qmax;
	 3,//		max_qdiff;
	 0,//		max_b_frames;
	 1, //		mpeg_quant;
	 1,//
	 -2, // 		luma_elim_threshold;
	 1,//
	 -5, // 		chroma_elim_threshold;
	 0.05,//		lumi_masking;
	 1,
	 0.01,//		dark_masking;
	 1,
 	 0.5,// 	qcompress;  /* amount of qscale change between easy & hard scenes (0.0-1.0)*/
    	 0.5,// 	qblur;      /* amount of qscale smoothing over time (0.0-1.0) */
	0,		// min bitrate in kB/S
	(8000*1000)>>3,		// max bitrate
	0,					// user_matrix 0->default, 1 tmpg, 2 animé , 3 kvcd
	12,					// Safe gop size limit
	NULL,				// inter & intra matrix, will be set depending on user_matrix
	NULL,
	0,		// interlaced
	0,		// WLA: bottom-field-first
	0,		// wide screen
	2,		// mb eval = distortion
	8000,		// vratetol
	0,
	0.5,		// temporal masking
	0,
	0.5,		// spatial masking
	0,		// NAQ
	0,		// Use xvid ratecontrol
	240,		// buffersize 240 KB for Mpeg2 /
	0		// DUMMY
	
	
 	}
  };
  // Mpeg2enc

  #include "mpeg2enc/ADM_mpeg2enc.h"
  
 MPEG2ENCConfig mpeg2encSVCDConfig={
  	{COMPRESS_CBR,4,1500000,700},
	{
	 	(2500*1000)>>3,		// Max BR
		12,		// Gop size
		0,		//int	wideScreen;
		0,		//int	matrix;
		0,		//int	interlacingType;
		0		// bff
	},
	0		// unconfigured
  };
  
  MPEG2ENCConfig mpeg2encDVDConfig={
  	{COMPRESS_CBR,4,1500000,700},
	{
	 	(9000*1000)>>3,		// Max BR
		12,		// Gop size
		0,		//int	wideScreen;
		0,		//int	matrix;
		0,		//int	interlacingType;
		0		// bff
	},
	0		// unconfigured
  };


#endif
/*

*/
#ifdef USE_XX_XVID
 static XVIDconfig  xvidConfig={{COMPRESS_CQ,4,1500000,700},
                    {
	 5, // High ME algo
	 0, // interlaced;
	 1, // inter4v;
	 XVID_H263QUANT,// quantizer;
	 2,// imin;
	 31, // imax;
	 2 ,// pmin;
	 31, // pmax;
	 
	 0,// lumi;

 	 XVID_ME_PMVFAST, // me;
	 1,// halfpel  ;

	 
	 10, // kfboost;
	 1, // min_key_interval;
	 300, // max_key_interval;
	 
	 	25, // curve_compression_high;
		10, // curve_compression_low;

		240, // 	bitrate_payback_delay;
	 
	

	 0,// startcred_start;
	 0,// startcred_end;
	

	 0,// endcred_start;
	 0,// endcred_end;
	 
	 0,// cred_rate;
	 

	1, // use_alt_curve;
	2, //VBR_ALT_CURVE_LINEAR, //int alt_curve_type;
	90, // alt_curve_low_dist;
	500, // alt_curve_high_dist;
	50, // alt_curve_min_rel_qual;
	1, // alt_curve_use_auto;
	1, // alt_curve_auto_str;
	1, // alt_curve_use_auto_bonus_bias;
	50, // alt_curve_bonus_bias;
	1//VBR_PAYBACK_BIAS // bitrate_payback_method 	 ;
//	"/tmp/Xvid2passLog.txt"; // path

										}
 										};;

#endif
#ifdef USE_XVID_4
 static XVID4config xvid4Config={
 		{COMPRESS_CQ,4,1500000,700},
                    {
		6, //int guiLevel;		
	
		1, //int min_key_interval;
		250, // Max key interval
		2, //int bframes;
	
		0, //int mpegQuantizer;	
		0, //int interlaced;
		1, //int inter4mv;
		0, //int trellis;	
		0, //int cartoon;
		0, //int greyscale;		
		0, // qpel
		0, // GMC
		1, // hqac
		0, // Chroma optim
		{2,2,2},//qmin
		{31,31,31},//qmax
	
	// This if for 2 pass 	
		0, //int keyframe_boost;
		0, //int curve_compression_high;
		0, //int curve_compression_low;
		5,//int overflow_control_strength;
		5,//int max_overflow_improvement;
		5,//int max_overflow_degradation;
		0, //int kfreduction;
		0, //int kfthreshold;
	
		24,//int container_frame_overhead;
		150,//int bquant_ratio;
		100,//int bquant_offset;    
		1, //vhqmode		    
		1, // chroma me
		0, // turbo
		0, // Packed bitstream - Not Xvid Default
		1, // closed_gop
		0  // bframe_threshold
		    }
};;

#endif
#ifdef USE_DIVX
 static DIVXConfig  divxConfig={{COMPRESS_CQ,4,1500,700}};
 																
#endif

#ifdef USE_FFMPEG
static MJPEGConfig mjpegConfig={90,0};

extern uint8_t DIA_mjpegCodecSetting( int *qual, int *swap );
#endif



///////////////////////////////////////////
#ifdef USE_DIVX
SelectCodecType current_codec=CodecDivx;
#else
SelectCodecType current_codec=CodecFF;
#endif
extern void UI_PrintCurrentVCodec(const char *str);
static void encoderPrint(void);
static const char *encoderGetName(void);
extern uint32_t videoProcessMode;

uint8_t loadVideoCodecConf( char *name);
uint8_t saveVideoCodecConf( char *name);
static uint8_t mk_hex(uint8_t a,uint8_t b);
CodecFamilty videoCodecGetFamily( void )
{
	if(current_codec==CodecXVCD || current_codec==CodecXSVCD)
			return CodecFamilyXVCD;
	if(current_codec==CodecVCD || current_codec==CodecSVCD|| current_codec==CodecDVD)
			 return CodecFamilyMpeg;
	return CodecFamilyAVI;

}
	#define Read(x) { \
		tmp=name; \
		if((tmp=strstr(name,#x))) \
			{ \
				tmp+=strlen(#x); \
				aprintf("-- %s\n",tmp); \
				sscanf(tmp,"=%d ",&x); \
			} \
			 else \
			{ printf("*** %s not found !***\n",#x);} \
		}
#define Add(x) {sprintf(tmp,"%s=%d ",#x,x);strcat(conf,tmp);}

/*
	Ultimately that will be dispatched on a per codec
	stuff and will be merged with xml stuff to allow
	save config / load config
*/
void videoCodecSetConf(  char *name,uint32_t extraLen, uint8_t *extraData)
{
 const char *tmp;
 int  mode = 0;
 uint32_t videoProcess;
	//printf("-Video filter by name : %s\n",name);

	Read(videoProcess);
	// it is a toggle !
	if(videoProcess!=videoProcessMode)
	{ // need to toggle it !
		
		videoProcessMode=videoProcess^1;
		UI_setVProcessToggleStatus( videoProcess );
	}
	else
	printf("Already in good mode (%d)\n",videoProcessMode);


	switch( current_codec)
	{
#define MAKECONF(x)  if(extraLen) {ADM_assert(extraLen==sizeof(x));memcpy(&(x),extraData,extraLen);}
#ifdef USE_XVID_4
      	 	case CodecXvid4 :

				MAKECONF(xvid4Config);
		      		break;
				
#endif
#ifdef USE_XX_XVID
      	 	case CodecXvid :

				MAKECONF(xvidConfig);
		      		break;
#endif				
#ifdef  USE_FFMPEG
					case CodecMjpeg :

						break;
#endif
#ifdef USE_DIVX

				 	case CodecDivx :
				  			MAKECONF(divxConfig);
		    			break;
#endif

					case CodecVCD:
						break;
					case CodecSVCD:
						if(extraLen && (extraLen == sizeof(mpeg2encSVCDConfig)) ){
							memcpy(&mpeg2encSVCDConfig,extraData,extraLen);
							mpeg2encSVCDConfig.configured = 1;
						}
						break;
					case CodecDVD:
						if(extraLen && (extraLen == sizeof(mpeg2encDVDConfig)) ){
							memcpy(&mpeg2encDVDConfig,extraData,extraLen);
							mpeg2encDVDConfig.configured = 1;
						}
						break;


#ifdef USE_FFMPEG
					case  CodecFF:
					case  CodecH263:
					case  CodecH263P:
					case  CodecHuff:
					case  CodecFFV1:
					case  CodecSnow:
											MAKECONF(ffmpegConfig);
											break;
					case CodecXSVCD:
									MAKECONF(ffmpegMpeg2Config);
									break;
					case CodecXVCD:
									MAKECONF(ffmpegMpeg1Config);
									break;
#endif
					default:
								ADM_assert(0);

	}

}
/*
	We return 2 things :
			The codec conf : i.e. mode process or not
				and the extraData size

*/

const char  *videoCodecGetConf( uint32_t *optSize, uint8_t **data)
{
	static char conf[4000];
	static char tmp[2000];
	uint32_t confSize=0;
	conf[0]=0;

	*optSize=0;
	*data=NULL;


	switch( current_codec)
	{
#ifdef USE_XX_XVID
      	 			case CodecXvid :
						*data=(uint8_t *)&xvidConfig;
						*optSize=sizeof(xvidConfig);
		      		break;
#endif
#ifdef USE_XVID_4
      	 			case CodecXvid4 :
						*data=(uint8_t *)&xvid4Config;
						*optSize=sizeof(xvid4Config);
		      		break;
#endif
#ifdef  USE_FFMPEG
					case CodecMjpeg :

						break;
#endif
#ifdef USE_DIVX

				 	case CodecDivx :

							*data=(uint8_t *)&divxConfig;
							*optSize=sizeof(divxConfig);
		    			break;
#endif
#ifdef USE_FFMPEG
					case  CodecFF:
					case  CodecH263:
					case  CodecH263P:
					case  CodecHuff:
					case  CodecFFV1:
					case CodecSnow:


											*data=(uint8_t *)&ffmpegConfig;
											*optSize=sizeof(ffmpegConfig);
											break;
					case CodecXSVCD:
									*data=(uint8_t *)&ffmpegMpeg2Config;
									*optSize=sizeof(ffmpegMpeg2Config);
									break;
					case CodecXVCD:

									*data=(uint8_t *)&ffmpegMpeg1Config;
									*optSize=sizeof(ffmpegMpeg1Config);
									break;
#endif
#ifdef USE_MJPEG
					case CodecVCD:
									break;
					case CodecSVCD:									
									*data=(uint8_t *)&mpeg2encSVCDConfig;
									*optSize=sizeof(mpeg2encSVCDConfig);
									break;
					case CodecDVD:
									*data=(uint8_t *)&mpeg2encDVDConfig;
									*optSize=sizeof(mpeg2encDVDConfig);
									break;
#endif
					default:
								ADM_assert(0);

	}

	uint8_t videoProcess=videoProcessMode;
	Add(videoProcess);
	confSize=*optSize;
	Add(confSize);

	aprintf("Conf :%s (%d) (%d)\n",conf,videoProcessMode,confSize);
	return conf;

}

//%
void EncoderSaveMpeg(char *name)
{
#ifdef USE_MJPEG
	switch(current_codec)
	{
		case CodecVCD:
				oplug_mpeg_vcd(name);
				break;
		case CodecSVCD:
				oplug_mpeg_svcd(name);
				break;
		case CodecDVD:
				oplug_mpeg_dvd(name);
				break;
		default:ADM_assert(0);
	}
#else
	GUI_Alert("** NO MPEG ENCODING SUPPORT**");
#endif

}

/*
	Ultimately that will be dispatched on a per codec
	stuff and will be merged with xml stuff to allow
	save config / load config
*/
int videoCodecConfigureAVI(  char *cmdString,uint32_t optSize, uint8_t *opt)
{
#define UNSET_COMPRESSION_MODE (COMPRESSION_MODE)0xff
#define NO_COMPRESSION_MODE    (COMPRESSION_MODE)0xfe
		int  iparam,equal=0xfff;
		COMPRESSION_MODE compmode=UNSET_COMPRESSION_MODE;
		char *cs = cmdString;
		char *go = cmdString;

		while( *go ){
			// search the =
			for(unsigned int i=0;i<strlen(cs);i++)
			{
				if(*(cs+i)=='=')
				{
						equal=i;
						break;
				}
			}
			if(equal==0xfff)
			{
				printf("\n i did not understood the args for video conf\n");
				return 0;
			}
			go = cs+equal+1;
			*(cs+equal)=0;
			iparam=atoi(cs+equal+1);
			printf("codec conf is %s\n",cs);
			// search the codec
			if(!strcasecmp(cs,"cq"))
				{
						compmode=COMPRESS_CQ;
						aprintf("cq Mode\n");
				}
			if(!strcasecmp(cs,"cbr"))
				{
						compmode=COMPRESS_CBR;
						iparam*=1000;
						aprintf("cbr Mode\n");
				}
			if(!strcasecmp(cs,"2pass"))
				{
						compmode=COMPRESS_2PASS;
						aprintf("2pass\n");
				}
			// search for other options
			if(!strcasecmp(cs,"mbr")){
			   compmode = NO_COMPRESSION_MODE;
			   iparam = (iparam*1000)>>3;
			   switch( current_codec ){
			      case CodecSVCD:
			         mpeg2encSVCDConfig.specific.maxBitrate = (int)iparam;
			         break;
			      case CodecDVD:
			         mpeg2encDVDConfig.specific.maxBitrate = (int)iparam;
			         break;
			      default:
			         break;
			   }
			}
			if(!strcasecmp(cs,"matrix")){
			   compmode = NO_COMPRESSION_MODE;
			   switch( current_codec ){
			      case CodecSVCD:
			         mpeg2encSVCDConfig.specific.user_matrix = (int)iparam;
			         break;
			      case CodecDVD:
			         mpeg2encDVDConfig.specific.user_matrix = (int)iparam;
			         break;
			      default:
			         break;
			   }
			}

			if(compmode==UNSET_COMPRESSION_MODE)
			{
				printf("\n ***** Unknown mode for video codec (%s)\n",cmdString);
				return 0;
			}

			if( compmode != NO_COMPRESSION_MODE ){
				aprintf("param:%d\n",iparam);
				setVideoEncoderSettings(compmode, iparam,optSize,opt);
			}

			// find next option
			for(;*go != '\0';go++){
                                if( *go == ',' ){
					cs = go + 1;
					break;
                                }
                        }
		}

		return 1;
}

int videoCodecConfigure(  char *cmdString,uint32_t optionSize,uint8_t  *option)
{
		CodecFamilty family;

		if(!cmdString) return 0;

		family=videoCodecGetFamily();
		switch(family)
		{
			case CodecFamilyAVI:
			case CodecFamilyXVCD:
			case CodecFamilyMpeg:
				return videoCodecConfigureAVI(cmdString,optionSize,option);
				break;
/*			case CodecFamilyMpeg :
				videoCodecConfigureMpeg(cmdString);			
				break;
*/				
			default:
				printf("This codec family does not accept paramaters\n");
				return 0;
		}
		return 0;

}

//___________________________________________________
typedef struct codecEnumByName
{
	SelectCodecType type;
	const char	*displayName;
	const char	*name;
}codecEnumByName;

static const codecEnumByName mycodec[]=
{
#ifdef USE_DIVX
	{CodecDivx	,"Divx","Divx"},
#endif
#ifdef USE_XX_XVID
	{CodecXvid	,"Xvid","Xvid"},
#endif

#ifdef USE_XVID_4
	{CodecXvid4	,"Xvid4","Xvid4"},
#endif


	{CodecMjpeg	,"MJpeg","Mjpeg"},
#ifdef USE_MJPEG
	{CodecVCD	,"VCD","VCD"},
	{CodecSVCD	,"SVCD","SVCD"},
	{CodecDVD	,"DVD","DVD"},
#endif
	{CodecXVCD	,"XVCD","XVCD"},
	{CodecXSVCD	,"DVD (lavc)","XSVCD"},
	{CodecFF	,"Lav Mpeg4","FFmpeg4"},
	{CodecH263	,"H263","H263"},
	{CodecH263P	,"H263+","H263+"},
	{CodecHuff	,"Huffyuv","Huffyuv"},
	{CodecFFV1	,"FFV1","FFV1"},
	{CodecSnow	,"Snow","Snow"}
};
uint32_t encoderGetNbEncoder(void)
{
	return sizeof(mycodec)/sizeof(codecEnumByName);
}
const char* encoderGetIndexedName(uint32_t i)
{
	ADM_assert(i<sizeof(mycodec)/sizeof(codecEnumByName));
	return mycodec[i].displayName;
}
void videoCodecChanged(int newcodec)
{
	ADM_assert(newcodec<sizeof(mycodec)/sizeof(codecEnumByName));
	current_codec=mycodec[newcodec].type;
}
void encoderPrint(void)
{
	for(uint32_t i=0;i<sizeof(mycodec)/sizeof(codecEnumByName);i++)
	{
		if(current_codec==mycodec[i].type)
		{
			UI_setVideoCodec(i);
			return;
		}
	
	}
	ADM_assert(0);
}
//___________________________________________________
void saveEncoderConfig( void )
{
	for(uint32_t i=0;i<sizeof(mycodec)/sizeof(codecEnumByName);i++)
	{
		if(current_codec==mycodec[i].type)
		{
			prefs->set(CODECS_PREFERREDCODEC,mycodec[i].name);
		}

	}
	
}
const char *encoderGetName(void)
{
	for(uint32_t i=0;i< sizeof(mycodec)/sizeof(codecEnumByName);i++)
	{
		if(current_codec==mycodec[i].type)
			return mycodec[i].name;
	
	}
	return "???";

}

void loadEncoderConfig ( void )
{
 char *name;
		if(!prefs->get(CODECS_PREFERREDCODEC, &name))
		{
			printf("could not get prefered codec!\n");
			return;
		}
		videoCodecSelectByName(name);
}
int videoCodecSelectByName(const char *name)
{
		for(uint32_t i=0;i<sizeof(mycodec)/sizeof(codecEnumByName);i++)
		{
			if(!strcasecmp(name,mycodec[i].name))
			{
				printf("\n Codec %s found\n",name);
				videoCodecSetcodec(mycodec[i].type);
				return 1;
			}

		}
		printf("\n Mmmm Select codec by name failed...(%s).\n",name);
		printf(" Available codec : %d\n",sizeof(mycodec)/sizeof(codecEnumByName));
		for(uint32_t i=0;i<sizeof(mycodec)/sizeof(codecEnumByName);i++)
		{
			printf("%s\n",mycodec[i].name);
		}
		return 0;
}

const char *videoCodecGetName( void )
{
	for(uint32_t i=0;i<sizeof(mycodec)/sizeof(codecEnumByName);i++)
	{
		if(current_codec==mycodec[i].type)
		{
			return mycodec[i].name;
		}

	}
	printf("\n Mmmm get video  codec  name failed..\n");
	return NULL;
}

void initEncoders( void)
{
//		strcpy( TwoPassLogFile,"/tmp/Xvid2passLog");
		printf("**********OBSOLETE*********\n");
}
///
///  Prompt for a codec and allow configuration
///
///______________________________


uint8_t DIA_videoCodec( SelectCodecType *codec );
void videoCodecSelect( void )
{
	DIA_videoCodec( &current_codec );
	encoderPrint();
	// HERE UI_PrintCurrentVCodec( (current_codec))

}
void videoCodecSetcodec(SelectCodecType codec)
{
	current_codec=codec;
	encoderPrint();

}
void videoCodecConfigureUI( void )
{
	printf("\n configuring codec :%d\n",current_codec);
	switch(current_codec)
			{


		case CodecMjpeg :
						DIA_mjpegCodecSetting(&(mjpegConfig.qual),&(mjpegConfig.swapped));
						break;
#ifdef USE_MJPEG
		case CodecVCD :
						break;
		case CodecSVCD :
						oplug_mpeg_svcdConf( );
						break;
		case CodecDVD :
						oplug_mpeg_dvdConf( );
						break;

#endif
		case CodecXSVCD :
			 			DIA_DVDffParam(&ffmpegMpeg2Config.generic.mode,
								&ffmpegMpeg2Config.generic.qz,
								&ffmpegMpeg2Config.generic.bitrate,
								&ffmpegMpeg2Config.generic.finalsize,
								&ffmpegMpeg2Config.specific);
						break;
		case CodecXVCD :
						oplug_mpegff_conf();
						break;
		


#ifdef USE_XX_XVID
      	 	case CodecXvid :
			 		DIA_getXvidCompressParams(&xvidConfig.generic.mode,
							&xvidConfig.generic.qz,
		      					&xvidConfig.generic.bitrate,
		        				&xvidConfig.generic.finalsize,
		                      			&xvidConfig.specific);		
		      			break;
#endif	
#ifdef USE_XVID_4
      	 	case CodecXvid4 :
			 		 DIA_xvid4(&xvid4Config.generic.mode,
							&xvid4Config.generic.qz,
		      					&xvid4Config.generic.bitrate,
		        				&xvid4Config.generic.finalsize,
		                      			&xvid4Config.specific);	
		      			break;
#endif									
#ifdef USE_DIVX
					
		case CodecDivx :
			  		getCompressParams(&divxConfig.generic.mode,
							&divxConfig.generic.qz,
							&divxConfig.generic.bitrate,
							&divxConfig.generic.finalsize);
		    			break;
#endif
#ifdef USE_FFMPEG	
		case CodecHuff:
				break;
		case CodecFFV1:
				break;
		case CodecSnow:
				{
					//ffmpegConfig.generic.mode=COMPRESS_CQ;
					int val=ffmpegConfig.generic.qz;
					
					  if(DIA_GetIntegerValue(&val, 2, 31, "Snow Quantizer","Snow Quantizer"))
					  {
						ffmpegConfig.generic.qz=val;;
					  }
				}
					break;

		case  CodecH263P:
					printf("\n H263P\n");					
		case  CodecH263:  
					printf("\n H263\n");					
		case  CodecFF:
/*
					getCompressParams(&ffmpegConfig.generic.mode,
							&ffmpegConfig.generic.qz,
							&ffmpegConfig.generic.bitrate,
							&ffmpegConfig.generic.finalsize);	
*/
					getFFCompressParams(&ffmpegConfig.generic.mode,
							&ffmpegConfig.generic.qz,
							&ffmpegConfig.generic.bitrate,
							&ffmpegConfig.generic.finalsize,
							&ffmpegConfig.specific);
							
					break;
#endif
					default:
								ADM_assert(0);
					}								  			
	
	
}
/*___________________________________________________________
	Set mode param and extra data for the currently selected codec
	The extradata is a free binary chunk (->memcpy of codec.specific part)
___________________________________________________________*/

void setVideoEncoderSettings(COMPRESSION_MODE mode, uint32_t  param, uint32_t extraConf, uint8_t *extraData)
{
	COMPRES_PARAMS *generic = NULL;
	void *specific=NULL;
	uint32_t specSize=0;

	switch( current_codec)
	{
				case CodecSVCD :
				  		generic=&mpeg2encSVCDConfig.generic;
						specific=&mpeg2encSVCDConfig.specific;
						specSize=sizeof(mpeg2encSVCDConfig.specific);
		      		break;
				case CodecDVD :
				  		generic=&mpeg2encDVDConfig.generic;
						specific=&mpeg2encDVDConfig.specific;
						specSize=sizeof(mpeg2encDVDConfig.specific);
		      		break;
				case CodecVCD:
						printf("No conf for VCD!\n");
				break;
#ifdef USE_XX_XVID
      	 			case CodecXvid :
				  		generic=&xvidConfig.generic;
						specific=&xvidConfig.specific;
						specSize=sizeof(xvidConfig.specific);
		      		break;
#endif
#ifdef USE_XVID_4
      	 			case CodecXvid4 :
				  		generic=&xvid4Config.generic;
						specific=&xvid4Config.specific;
						specSize=sizeof(xvid4Config.specific);
		      		break;
#endif
#ifdef  USE_FFMPEG
					case CodecMjpeg :

						break;
#endif
#ifdef USE_DIVX

				 	case CodecDivx :
				  			generic=&divxConfig.generic;
							specific=&divxConfig.specific;
							specSize=sizeof(divxConfig.specific);
		    			break;
#endif
#ifdef USE_FFMPEG
					case  CodecFF:
					case  CodecH263:
					case  CodecH263P:
					case  CodecHuff:
					case  CodecFFV1:
					case  CodecSnow:

									generic=&ffmpegConfig.generic ;
									specific=&ffmpegConfig.specific;
									specSize=sizeof(ffmpegConfig.specific);
									break;
					case CodecXSVCD:		generic=&ffmpegMpeg2Config.generic;
									specific=&ffmpegMpeg2Config.specific;
									specSize=sizeof(ffmpegMpeg2Config.specific);
									break;
					case CodecXVCD:
									generic=&ffmpegMpeg1Config.generic;
									specific=&ffmpegMpeg1Config.specific;
									specSize=sizeof(ffmpegMpeg1Config.specific);
									break;
#endif
					default:
								ADM_assert(0);

	}
	  generic->mode=mode;
	  switch(mode)
	  {
				case COMPRESS_CBR:
								aprintf("CBR : %lu kbps\n",param);
								generic->bitrate=param;
								break;
				case COMPRESS_CQ:
								aprintf("CQ : %lu q\n",param);
								generic->qz=param;
								break;
				case COMPRESS_2PASS:
								aprintf("2pass : %lu q\n",param);
								generic->finalsize=param;
								break;
				default:
								ADM_assert(0);												
															
		}

	if(extraConf && extraData && specific)
	{
		if(specSize!=extraConf)
		{
			printf("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
			printf("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
			printf("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
		}
		else
		{
			printf("\n using %u bytes of codec specific data...\n",extraConf);
			memcpy(specific,extraData,extraConf);
		}
	}
}
Encoder *getVideoEncoder( uint32_t w,uint32_t h )

{

//int s;
Encoder *e=NULL;

		switch(current_codec)
			{
#ifdef USE_FFMPEG
#ifndef USE_DIVX
					default:				  
#endif
      	 	case CodecFF :   								  
				  		e=new   EncoderFFMPEG(FF_MPEG4,&ffmpegConfig);
				    	break;			
#endif		

#ifdef USE_FFMPEG
		case   CodecHuff:
					e=new   EncoderFFMPEGHuff(&ffmpegConfig);
				    	break;
		case   CodecFFV1:
					e=new   EncoderFFMPEGFFV1(&ffmpegConfig);
				    	break;
		case   CodecSnow:
					e=new EncodeFFMPEGSNow(&ffmpegConfig);
					break;
		case   CodecH263P:
					e=new   EncoderFFMPEG(FF_H263P,&ffmpegConfig);
				    	break;		
      	 	case CodecH263 : 
         			if( !((w==128) && (h=96)) &&
            		! ((w==176) && (h=144))) 
              	{
								  	GUI_Alert("Only Qcif and SubQcif allowed \n for H263 codec");
								    return 0;
								 } 								  
				  		e=new   EncoderFFMPEG(FF_H263,&ffmpegConfig);
				    	break;			
#endif										
				
#ifdef USE_XX_XVID
      	 				case CodecXvid :   								  
				  		e=new   EncoderXvid( &xvidConfig); 
				    	break;
#endif	
#ifdef USE_XVID_4
      	 				case CodecXvid4 :						  
				  		e=new   EncoderXvid4( &xvid4Config); 
				    	break;
#endif								
#ifdef  USE_FFMPEG
					case CodecMjpeg :
							e=new   EncoderMjpeg(&mjpegConfig); break;
#endif
#ifdef USE_DIVX
					default:				  
				 	case CodecDivx :   
				  		e=new   EncoderDivx(&divxConfig); break;
#endif				    
				  			
			
				}
         if(!e) 
#ifdef USE_DIVX         	
          e=new   EncoderDivx(&divxConfig);
#else
					e=new   EncoderFFMPEG(FF_MPEG4,&ffmpegConfig);
#endif          
		return e;
	
}
/**
	Select the codec and its configuration from a file
	(given as sole argument)

*/
uint8_t loadVideoCodecConf( char *name)
{
FILE *fd=NULL;
char    str[4000];
char    str_extra[4000];
char    str_tmp[4000];
uint32_t    nb;
uint32_t extraSize=0;
uint8_t *extra=NULL;

	fd=fopen(name,"rt");
	if(!fd)
	{
		printf("Trouble reading codec conf\n");
		return 0;
	
	}
	fscanf(fd,"%s\n",str);
// and video codec
	fscanf(fd,"Video codec : %s\n",str );
	videoCodecSelectByName(str);
//***

	fgets(str,200,fd); // here we got the conf
	fscanf(fd,"Video extraConf size : %lu\n",&extraSize);
	if(extraSize)
	{
		uint8_t *ptr;
		fgets(str_tmp,3999,fd);
		ptr=(uint8_t *)str_tmp;

		for(uint32_t k=0;k<extraSize;k++)
		{
			str_extra[k]=mk_hex(*ptr,*(ptr+1));
			ptr+=3;
		}
		extra=(uint8_t *)str_extra;
	}

	videoCodecSetConf(str,extraSize,extra);
	fclose(fd);
	return 1;
}

/**
	Save the video codec and its configuration from a file
	(given as sole argument)

*/
uint8_t saveVideoCodecConf( char *name)
{
FILE *fd=NULL;
	fd=fopen(name,"wt");
	if(!fd)
	{
		printf("Trouble writing codec conf\n");
		return 0;
	
	}
	fprintf(fd,"ADVC\n");
// and video codec
	fprintf(fd,"Video codec : %s\n",videoCodecGetName() );
// video config
	uint32_t 	extraSize;
	uint8_t 	*extra;
	fprintf(fd,"Video conf : %s\n",videoCodecGetConf(&extraSize,&extra)  );

	// Raw hexDump the opt string
	// which contains
	fprintf(fd,"Video extraConf size : %lu\n",extraSize);
	if(extraSize)
	{
		for(uint32_t l=extraSize;l>0;l--)
		{
			fprintf(fd,"%02x ",*extra++);
		}
		fprintf(fd,"\n");
	}
	fclose(fd);
	return 1;
}	
uint8_t mk_hex(uint8_t a,uint8_t b)
{
int a1,b1;
	a1=a;
	b1=b;
	if(a>='a')
	{
		a1=a1+10;
		a1=a1-'a';
	}
	else
	{
		a1=a1-'0';
	}
	
	if(b>='a')
	{
		b1=b1+10;
		b1=b1-'a';
	}
	else
	{
		b1=b1-'0';
	}
	
	return (a1<<4)+b1;

}


// EOF
