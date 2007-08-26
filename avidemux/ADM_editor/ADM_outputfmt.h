//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// (c) Mean, fixounet@free.fr
//
// Copyright: See COPYING file that comes with this distribution
// 2002-2004
//
#ifndef ADM_OUT_FMT
#define ADM_OUT_FMT
typedef enum 
{
	ADM_AVI=0,
	ADM_AVI_DUAL,
	ADM_AVI_PAK,
	ADM_AVI_UNP,
	ADM_PS,
	ADM_TS,
	ADM_ES,
	ADM_MP4,
	ADM_OGM,
	ADM_PSP,
	ADM_FLV,
	ADM_FORMAT_MAX,
	ADM_FMT_DUMMY=ADM_FORMAT_MAX
}ADM_OUT_FORMAT;

typedef struct 
{
  ADM_OUT_FORMAT format;
  const char *text;
}ADM_FORMAT_DESC;

const ADM_FORMAT_DESC ADM_allOutputFormat[]=
{
  {ADM_AVI,_("AVI")},
  {ADM_AVI_DUAL,_("AVI, dual audio")},
  {ADM_AVI_PAK,_("AVI, pack VOP")},
  {ADM_AVI_UNP,_("AVI, unpack VOP")},  
  {ADM_PS,_("MPEG-PS (A+V)")},
  {ADM_TS,_("MPEG-TS (A+V)")},
  {ADM_ES,_("MPEG video")},
  {ADM_MP4,_("MP4")},
  {ADM_OGM,_("OGM")},
  {ADM_PSP,_("PSP")},
  {ADM_FLV,_("FLV")}
};

#endif
//EOF
