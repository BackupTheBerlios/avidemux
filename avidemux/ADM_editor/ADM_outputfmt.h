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
        ADM_AVI_UNP,
        ADM_AVI_PAK,
        ADM_AVI_DUAL,
	ADM_OGM,
	ADM_ES,
	ADM_PS,
        ADM_TS,
        ADM_MP4,
        ADM_PSP,
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
  {     ADM_AVI,"Avi"},
  {     ADM_AVI_UNP,"Avi,unp. vop"},
  {     ADM_AVI_PAK,"Avi,Pack vop"},
  {     ADM_AVI_DUAL,"Avi,dual audio"},
  {	ADM_OGM,"OGM"},
  {	ADM_ES,"Mpeg Video"},
  {	ADM_PS,"Mpeg PS (A+V"},
  {     ADM_TS,"Mpeg TS (A+V"},
  {     ADM_MP4,"MP4"},
  {     ADM_PSP,"PSP"},
  
};

#endif
//EOF
