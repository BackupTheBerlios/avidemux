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
#include <math.h>
#include <string.h>
#include "ADM_lavcodec.h"

#include "ADM_assert.h"
#include "fourcc.h"
#include "ADM_toolkit/ADM_quota.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_toolkit/toolkit.hxx"

#include "prefs.h"
#include "ADM_vidEncode.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ENCODER
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_dialog/DIA_enter.h"
#include "oplug_mpeg/op_mpeg.h"
#include "ADM_gui2/GUI_ui.h"


// Some static stuff
static COMPRES_PARAMS *videoCodecGetDescriptor (SelectCodecType codec);
void setVideoEncoderSettings (COMPRESSION_MODE mode, uint32_t param,     uint32_t extraConf, uint8_t * extraData);
static void encoderPrint (void);
static const char *encoderGetName (void);


uint32_t encoderGetNbEncoder (void);
const char *encoderGetIndexedName (uint32_t i);

#include "adm_encConfig.h"
#include "ADM_vidEncode.hxx"
#include "adm_encoder.h"


#ifdef USE_XVID_4
#include "ADM_codecs/ADM_xvid4.h"
#include "ADM_codecs/ADM_xvid4param.h"
#include "adm_encXvid4.h"

#endif
#ifdef USE_XX_XVID
#include "ADM_codecs/ADM_xvid.h"
#include "adm_encxvid.h"
#include "xvid.h"

#endif



#include "ADM_codecs/ADM_ffmpeg.h"
#include "adm_encffmpeg.h"


#include "ADM_codecs/ADM_mjpegEncode.h"
#include "adm_encmjpeg.h"
#include "adm_encCopy.h"
#include "adm_encyv12.h"

#define FF_TRELLIS 0		// use treillis for mpeg1 encoding
/*
  	Codec settings here
*/

#include "ADM_encCodecDesc.h"

SelectCodecType current_codec = CodecFF;
///////////////////////////////////////////




uint8_t loadVideoCodecConf (char *name);
uint8_t saveVideoCodecConf (char *name);
uint8_t mk_hex (uint8_t a, uint8_t b);
//*********************************************
CodecFamilty
videoCodecGetFamily (void)
{
  if (current_codec == CodecXVCD || current_codec == CodecXSVCD
      || current_codec == CodecXDVD)
    return CodecFamilyXVCD;
  if (current_codec == CodecVCD || current_codec == CodecSVCD
      || current_codec == CodecDVD)
    return CodecFamilyMpeg;
  return CodecFamilyAVI;

}
//*********************************************
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
//******************************************************************
// Return the COMPRES_PARAMS corresponding to the given codec Id
//****************************************************************
COMPRES_PARAMS *
videoCodecGetDescriptor (SelectCodecType codec)
{
  int nb, i;
  nb = sizeof (AllVideoCodec) / sizeof (COMPRES_PARAMS *);
  for (i = 0; i < nb; i++)
    {
      if (AllVideoCodec[i]->codec == codec)
	return AllVideoCodec[i];
    }
  printf ("Warning ! codec %d not found\n", codec);
  return NULL;
}
uint8_t
videoCodecSetFinalSize (uint32_t size)
{
  COMPRES_PARAMS *mode;
  mode = videoCodecGetDescriptor (current_codec);
  if (!mode)
    return 0;
  mode->finalsize = size;
  if (mode->capabilities & ADM_ENC_CAP_2PASS)
    mode->mode = COMPRESS_2PASS;
  return 1;

}
/*
	We return 2 things :
			The codec conf : i.e. mode process or not
				and the extraData size

*/


//%
uint32_t
videoProcessMode (void)
{
  if (current_codec == CodecCopy)
    return 0;
  return 1;
}
uint8_t
EncoderSaveMpeg (const char *name)
{
  uint8_t raw;
  uint8_t ret = 0;
  switch (UI_GetCurrentFormat ())
    {
    case ADM_ES:
      raw = 1;
      break;
    case ADM_PS:
      raw = 0;
      break;
    case ADM_TS:
      raw = 2;
      break;
    default:
      GUI_Error_HIG ("Wrong output format", "Select MPEG as the output.");
      return 0;
    }
  if (current_codec != CodecDVD && raw == 2)
    {
      GUI_Error_HIG ("Wrong video codec",
		     "Select DVD as the video codec for MPEG TS output.");
      return 0;
    }
  switch (current_codec)
    {
    case CodecVCD:
      if (raw)
	ret = oplug_mpeg_vcd (name);
      else
	ret = oplug_mpeg_vcd_ps (name);
      break;
    case CodecSVCD:
      if (raw)
	ret = oplug_mpeg_svcd (name);
      else
	ret = oplug_mpeg_svcd_ps (name);
      break;
    case CodecDVD:
      switch (raw)
	{
	case 2:
	  ret = oplug_mpeg_ts (name);
	  break;
	case 1:
	  ret = oplug_mpeg_dvd (name);
	  break;
	case 0:
	  ret = oplug_mpeg_dvd_ps (name);
	  break;
	default:
	  ADM_assert (0);
	}
      break;
    default:
      ADM_assert (0);
    }
  return ret;
}

/*
	Ultimately that will be dispatched on a per codec
	stuff and will be merged with xml stuff to allow
	save config / load config
*/
int
videoCodecConfigureAVI (char *cmdString, uint32_t optSize, uint8_t * opt)
{
  COMPRES_PARAMS *param;

#define UNSET_COMPRESSION_MODE (COMPRESSION_MODE)0xff
#define NO_COMPRESSION_MODE    (COMPRESSION_MODE)0xfe
  int iparam, equal = 0xfff;
  COMPRESSION_MODE compmode = UNSET_COMPRESSION_MODE;
  char *cs = cmdString;
  char *go = cmdString;



  while (*go)
    {
      // search the =
      for (unsigned int i = 0; i < strlen (cs); i++)
	{
	  if (*(cs + i) == '=')
	    {
	      equal = i;
	      break;
	    }
	}
      if (equal == 0xfff)
	{
	  printf ("\n I did not understand the args for video conf.\n");
	  return 0;
	}
      go = cs + equal + 1;
      *(cs + equal) = 0;
      iparam = atoi (cs + equal + 1);
      printf ("codec conf is %s\n", cs);
      // search the codec
      if (!strcasecmp (cs, "aq"))
	{
	  compmode = COMPRESS_AQ;
	  aprintf ("aq Mode\n");
	}
      if (!strcasecmp (cs, "cq"))
	{
	  compmode = COMPRESS_CQ;
	  aprintf ("cq Mode\n");
	}
      if (!strcasecmp (cs, "cbr"))
	{
	  compmode = COMPRESS_CBR;
	  //iparam*=1000;
	  aprintf ("cbr Mode\n");
	}
      if (!strcasecmp (cs, "2pass"))
	{
	  compmode = COMPRESS_2PASS;
	  aprintf ("2pass\n");
	}
    if (!strcasecmp (cs, "2passbitrate"))
	{
	  compmode = COMPRESS_2PASS_BITRATE;
	  aprintf ("2pass bitrate\n");
	}
      if (!strcasecmp (cs, "follow"))
	{
	  compmode = COMPRESS_SAME;
	  aprintf ("Follow mode\n");
	}

      // search for other options
      if (!strcasecmp (cs, "mbr"))
	{
	  compmode = NO_COMPRESSION_MODE;
	  iparam = (iparam * 1000) >> 3;

	  switch (current_codec)
	    {
	    case CodecSVCD:
	      SVCDExtra.maxBitrate = (int) iparam;
	      break;
	    case CodecDVD:
	      DVDExtra.maxBitrate = (int) iparam;
	      break;
	    default:
	      break;
	    }
	}
      if (!strcasecmp (cs, "matrix"))
	{
	  compmode = NO_COMPRESSION_MODE;
	  switch (current_codec)
	    {
	    case CodecSVCD:
	      SVCDExtra.user_matrix = (int) iparam;
	      break;
	    case CodecDVD:
	      DVDExtra.user_matrix = (int) iparam;
	      break;
	    default:
	      break;
	    }
	}
      if (!strcmp (cs, "ws"))
	{
	  switch (current_codec)
	    {
	    case CodecDVD:
	      DVDExtra.widescreen = 1;
	    case CodecSVCD:
	      SVCDExtra.widescreen = 1;
	      break;
	    default:
	      break;
	    }
	}
      if (compmode == UNSET_COMPRESSION_MODE)
	{
	  printf ("\n ***** Unknown mode for video codec (%s)\n", cmdString);
	  return 0;
	}

      if (compmode != NO_COMPRESSION_MODE)
	{
	  aprintf ("param:%d\n", iparam);
	  setVideoEncoderSettings (compmode, iparam, optSize, opt);
	}

      // find next option
      for (; *go != '\0'; go++)
	{
	  if (*go == ',')
	    {
	      cs = go + 1;
	      break;
	    }
	}
    }

  return 1;
}

int
videoCodecConfigure (char *cmdString, uint32_t optionSize, uint8_t * option)
{
  CodecFamilty family;

  if (!cmdString)
    return 0;

  family = videoCodecGetFamily ();
  switch (family)
    {
    case CodecFamilyAVI:
    case CodecFamilyXVCD:
    case CodecFamilyMpeg:
      return videoCodecConfigureAVI (cmdString, optionSize, option);
      break;
/*			case CodecFamilyMpeg :
				videoCodecConfigureMpeg(cmdString);
				break;
*/
    default:
      printf ("This codec family does not accept paramaters\n");
      return 0;
    }
  return 0;

}

//___________________________________________________
// Used to know the # of menu entries
//___________________________________________________
uint32_t
encoderGetNbEncoder (void)
{
  return (sizeof (AllVideoCodec) / sizeof (COMPRES_PARAMS *)) - 1;	// There is a dummy extra one at the end
}
/* *** Returne the name of the encoder #i, as displayer by a menu/combo box */
const char *
encoderGetIndexedName (uint32_t i)
{
  int nb = encoderGetNbEncoder ();
  COMPRES_PARAMS *param;
  ADM_assert (i <= nb);
  return AllVideoCodec[i]->menuName;
}
/************************************/
void
videoCodecChanged (int newcodec)
{
  int nb = encoderGetNbEncoder ();
  ADM_assert (newcodec <= nb);
  current_codec = AllVideoCodec[newcodec]->codec;
}
/************************************/
void
encoderPrint (void)
{
  int nb = encoderGetNbEncoder ();
  for (uint32_t i = 0; i < nb; i++)
    {
      if (current_codec == AllVideoCodec[i]->codec)
	{
	  UI_setVideoCodec (i);
	  return;
	}

    }
  ADM_assert (0);
}
//___________________________________________________
void
saveEncoderConfig (void)
{
  int nb = encoderGetNbEncoder ();
  for (uint32_t i = 0; i < nb; i++)
    {
      if (current_codec == AllVideoCodec[i]->codec)
	{
	  prefs->set (CODECS_PREFERREDCODEC, AllVideoCodec[i]->tagName);
	}

    }

}
const char *
encoderGetName (void)
{
  int nb = encoderGetNbEncoder ();
  for (uint32_t i = 0; i < nb; i++)
    {
      if (current_codec == AllVideoCodec[i]->codec)
	return AllVideoCodec[i]->tagName;

    }
  return "???";

}

void
loadEncoderConfig (void)
{
  char *name;
#if 0				//
  if (!prefs->get (CODECS_PREFERREDCODEC, &name))
    {
      printf ("could not get prefered codec!\n");
      return;
    }
  videoCodecSelectByName (name);
#endif
  /* change some hardcoded defaults ... */
#if 0				//def USE_XX_XVID
  prefs->get (CODECS_XVID_ENCTYPE, (uint *) & (xvidConfig.generic.mode));
  prefs->get (CODECS_XVID_QUANTIZER, &(xvidConfig.generic.qz));
  prefs->get (CODECS_XVID_BITRATE, &(xvidConfig.generic.bitrate));
  prefs->get (CODECS_XVID_FINALSIZE, &(xvidConfig.generic.finalsize));
#endif
}
int
videoCodecSelectByName (const char *name)
{
  int nb = encoderGetNbEncoder ();
  for (uint32_t i = 0; i < nb; i++)
    {
      if (!strcasecmp (name, AllVideoCodec[i]->tagName))
	{
	  printf ("\n Codec %s found\n", name);
	  videoCodecSetcodec (AllVideoCodec[i]->codec);
	  return 1;
	}

    }
  printf ("\n Mmmm Select codec by name failed...(%s).\n", name);
  printf (" Available codec : %d\n",
	  sizeof (AllVideoCodec) / sizeof (COMPRES_PARAMS));
  for (uint32_t i = 0; i < nb; i++)
    {
      printf ("%s:%s\n", AllVideoCodec[i]->tagName,
	      AllVideoCodec[i]->descriptor);
    }
  return 0;
}
const char *
videoCodecGetMode (void)
{
  uint8_t *data;
  uint32_t nbData = 0;
  COMPRES_PARAMS *mode;
  static char string[90];

  mode = videoCodecGetDescriptor (current_codec);
  ADM_assert (mode);
  switch (mode->mode)
    {
    case COMPRESS_AQ:
      sprintf (string, "AQ=%d", mode->qz);
      break;
    case COMPRESS_CQ:
      sprintf (string, "CQ=%d", mode->qz);
      break;
    case COMPRESS_CBR:
      sprintf (string, "CBR=%d", mode->bitrate);
      break;
    case COMPRESS_2PASS:
      sprintf (string, "2PASS=%d", mode->finalsize);
      break;
    case COMPRESS_2PASS_BITRATE:
      sprintf (string, "2PASSBITRATE=%d", mode->avg_bitrate);
      break;
    case COMPRESS_SAME:
      sprintf (string, "FOLLOW=0");
      break;
    default:
      ADM_assert (0);
    }
  return string;

}
const char *
videoCodecGetName (void)
{
  int nb = encoderGetNbEncoder ();
  for (uint32_t i = 0; i < nb; i++)
    {
      if (current_codec == AllVideoCodec[i]->codec)
	{
	  return AllVideoCodec[i]->tagName;
	}

    }
  printf ("\n Mmmm get video  codec  name failed..\n");
  return NULL;
}
///
///  Prompt for a codec and allow configuration
///
///______________________________


uint8_t DIA_videoCodec (SelectCodecType * codec);
void
videoCodecSelect (void)
{
  DIA_videoCodec (&current_codec);
  encoderPrint ();
  // HERE UI_PrintCurrentVCodec( (current_codec))

}
void
videoCodecSetcodec (SelectCodecType codec)
{
  current_codec = codec;
  encoderPrint ();

}
void
videoCodecConfigureUI (void)
{
  printf ("\n configuring codec :%d\n", current_codec);
  COMPRES_PARAMS *param;
  param = videoCodecGetDescriptor (current_codec);
  ADM_assert (param);
  if (param->configure)
    {
      param->configure (param);

    }

#if 0				//DISABLE

#ifdef USE_XX_XVID
case CodecXvid:
  DIA_getXvidCompressParams (&xvidConfig.generic.mode,
			     &xvidConfig.generic.qz,
			     &xvidConfig.generic.bitrate,
			     &xvidConfig.generic.finalsize,
			     &xvidConfig.specific);
  break;
#endif

#endif // DISABLE
}
/*___________________________________________________________
	Set mode param and extra data for the currently selected codec
	The extradata is a free binary chunk (->memcpy of codec.specific part)
___________________________________________________________*/

void
setVideoEncoderSettings (COMPRESSION_MODE mode, uint32_t param,
			 uint32_t extraConf, uint8_t * extraData)
{
  COMPRES_PARAMS *generic = NULL;
  void *specific = NULL;
  uint32_t specSize = 0;
  COMPRES_PARAMS *zparam;

  zparam = videoCodecGetDescriptor (current_codec);
  ADM_assert (zparam);
  zparam->mode = mode;

  switch (mode)
    {
    case COMPRESS_SAME:
      aprintf ("Same as input\n");
      break;
    case COMPRESS_CBR:
      aprintf ("CBR : %lu kbps\n", param);
      zparam->bitrate = param;
      break;
    case COMPRESS_AQ:
      aprintf ("AQ : %lu q\n", param);
      zparam->qz = param;
      break;
    case COMPRESS_CQ:
      aprintf ("CQ : %lu q\n", param);
      zparam->qz = param;
      break;
    case COMPRESS_2PASS:
      aprintf ("2pass : %lu q\n", param);
      zparam->finalsize = param;
      break;
    case COMPRESS_2PASS_BITRATE:
      aprintf ("2passbitrate : %lu q\n", param);
      zparam->avg_bitrate = param;
      break;
    default:
      ADM_assert (0);

    }

  if (extraConf && extraData && zparam->extraSettingsLen)
    {
      if (zparam->extraSettingsLen != extraConf)
	{
	  printf
	    ("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
	  printf
	    ("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
	  printf
	    ("\n*** ERROR : Extra data for codec config has a different size!!! *****\n");
	}
      else
	{
	  printf ("\n using %u bytes of codec specific data...\n", extraConf);
	  memcpy (zparam->extraSettings, extraData, extraConf);
	}
    }
}
Encoder *
getVideoEncoder (uint32_t w, uint32_t h, uint32_t globalHeaderFlag)
{

//int s;
  Encoder *e = NULL;

  switch (current_codec)
    {
    case CodecCopy:
      e = new EncoderCopy (NULL);
      break;
    default:
    case CodecYV12:
      e = new EncoderYV12 ();
      break;
    case CodecFF:
      e = new EncoderFFMPEG (FF_MPEG4, &ffmpegMpeg4);
      break;
    case CodecMjpeg:
      e = new EncoderMjpeg (&MjpegCodec);
      break;


    case CodecFFhuff:
      e = new EncoderFFMPEGFFHuff (&ffmpegFFHUFF);
      break;
    case CodecHuff:
      e = new EncoderFFMPEGHuff (&ffmpegHUFF);
      break;
    case CodecFFV1:
      e = new EncoderFFMPEGFFV1 (&ffmpegFFV1);
      break;
    case CodecSnow:
      e = new EncodeFFMPEGSNow (&ffmpegSnow);
      break;
//              case   CodecH263P:
//                                      e=new   EncoderFFMPEG(FF_H263P,&ffmpeg4Extra);
//                                      break;
    case CodecH263:
      if (!((w == 128) && (h == 96)) && !((w == 176) && (h == 144)))
	{
	  GUI_Error_HIG ("Only QCIF and subQCIF are allowed for H.263", NULL);
	  return 0;
	}
      e = new EncoderFFMPEG (FF_H263, &ffmpegH263Codec);
      break;
#ifdef USE_XVID_4
    case CodecXvid4:
      e = new EncoderXvid4 (&Xvid4Codec);
      break;
#endif

#ifdef USE_X264
    case CodecX264:
#warning GROSS HACK
      {
	x264Extra.globalHeader = globalHeaderFlag;
	e = new EncoderX264 (&x264Codec);
	x264Extra.globalHeader = 0;
	break;
      }
#endif
#if 0
#ifdef USE_XX_XVID
    case CodecXvid:
      e = new EncoderXvid (&xvidExtra);
      break;
#endif

#endif // DISABLE

    }
  return e;
}
/**
	Select the codec and its configuration from a file
	(given as sole argument)

*/
uint8_t
loadVideoCodecConf (char *name)
{
  FILE *fd = NULL;
  char str[4000];
  char str_extra[4000];
  char str_tmp[4000];
  uint32_t nb;
  uint32_t extraSize = 0;
  uint8_t *extra = NULL;

  fd = fopen (name, "rt");
  if (!fd)
    {
      printf ("Trouble reading codec conf\n");
      return 0;

    }
  fscanf (fd, "%s\n", str);
// and video codec
  fscanf (fd, "Video codec : %s\n", str);
  videoCodecSelectByName (str);
//***

  fgets (str, 200, fd);		// here we got the conf
  fscanf (fd, "Video extraConf size : %lu\n", &extraSize);
  if (extraSize)
    {
      uint8_t *ptr;
      fgets (str_tmp, 3999, fd);
      ptr = (uint8_t *) str_tmp;

      for (uint32_t k = 0; k < extraSize; k++)
	{
	  str_extra[k] = mk_hex (*ptr, *(ptr + 1));
	  ptr += 3;
	}
      extra = (uint8_t *) str_extra;
    }

  videoCodecSetConf (str, extraSize, extra);
  fclose (fd);
  return 1;
}
/***********************/
uint8_t
loadVideoCodecConfString (char *cmd)
{
#define MAX_STRING 4000
  char str[MAX_STRING * 3];
  char str_extra[MAX_STRING * 3];
  char str_tmp[MAX_STRING * 3];
  uint32_t nb;
  uint32_t extraSize = 0;
  uint8_t *extra = NULL;

  sscanf (cmd, "%d ", &extraSize);
  ADM_assert (extraSize < MAX_STRING);
  if (extraSize)
    {
      while (*cmd != ' ')
	cmd++;
      cmd++;			// skip the first ' '
      for (uint32_t k = 0; k < extraSize; k++)
	{
	  str_extra[k] = mk_hex (*cmd, *(cmd + 1));
	  cmd += 3;
	}
      extra = (uint8_t *) str_extra;
      videoCodecSetConf (str, extraSize, extra);
    }
  return 1;
}

uint8_t
mk_hex (uint8_t a, uint8_t b)
{
  int a1, b1;
  a1 = a;
  b1 = b;
  if (a >= 'a')
    {
      a1 = a1 + 10;
      a1 = a1 - 'a';
    }
  else
    {
      a1 = a1 - '0';
    }

  if (b >= 'a')
    {
      b1 = b1 + 10;
      b1 = b1 - 'a';
    }
  else
    {
      b1 = b1 - '0';
    }

  return (a1 << 4) + b1;

}
// Old stuff
uint8_t
videoCodecGetConf (uint32_t * optSize, uint8_t ** data)
{
  static char conf[4000];
  static char tmp[2000];
  uint32_t confSize = 0;
  conf[0] = 0;

  *optSize = 0;
  *data = NULL;

  COMPRES_PARAMS *param;
  param = videoCodecGetDescriptor (current_codec);
  ADM_assert (param);
  if (param->extraSettingsLen)
    {
      *data = (uint8_t *) param->extraSettings;
      *optSize = param->extraSettingsLen;
    }

  confSize = *optSize;
  printf ("Conf size : %d\n", confSize);
  return 1;

}

void
videoCodecSetConf (char *name, uint32_t extraLen, uint8_t * extraData)
{
  const char *tmp;
  int mode = 0;
  uint32_t videoProcess;
  COMPRES_PARAMS *param;
  //printf("-Video filter by name : %s\n",name);

  param = videoCodecGetDescriptor (current_codec);
  if (!param)
    {
      GUI_Error_HIG ("Fatal error", NULL);
      printf ("Current codec:%d\n", current_codec);
      ADM_assert (0);
    }
  if (extraLen)
    {
      if (extraLen != param->extraSettingsLen)
	{
	  printf ("Codec:%s\n", param->descriptor);
	  printf ("Expected :%d got:%d\n", param->extraSettingsLen, extraLen);
	  ADM_assert (0);
	}
      memcpy (param->extraSettings, extraData, param->extraSettingsLen);
    }

}



// EOF
