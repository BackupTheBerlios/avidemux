//
// C++ Interface: adm_command
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ADM_script/adm_scanner.h"

typedef int (*dummy_func)(int n,Arg *args);
typedef struct admCommand
{
	char 		*command;
	dummy_func	func;
	uint32_t	nbArgs;
	APM_TYPE	args[MAXPARAM];	// 10 params should be enough
};

int scriptLoad(int n,Arg *args);
int scriptExit(int n,Arg *args);
int scriptAppend(int n,Arg *args);
int scriptAudioProcess(int n,Arg *args);
int scriptVideoProcess(int n,Arg *args);

int scriptAudioDownsample(int n,Arg *args);
int scriptAudioResample(int n,Arg *args);
int scriptAudioNormalize(int n,Arg *args);

int scriptLoadAudio(int n,Arg *args);
int scriptSaveAudio(int n,Arg *args);
int scriptGoto(int n,Arg *args);

int scriptSetFps(int n,Arg *args);

int scriptSleep(int n,Arg *args);
int scriptSetMarkerA(int n,Arg *args);
int scriptSetMarkerB(int n,Arg *args);
int scriptScanVbr(int n,Arg *args);
int scriptAudioDelay(int n,Arg *args);
int scriptFilm2Pal(int n,Arg *args);
int scriptAudioCodec(int n,Arg *args);
int scriptLoadCodec(int n,Arg *args);
int scriptSavejpeg(int n,Arg *args);
int scriptRaw(int n,Arg *args);
int scriptSave(int n,Arg *args);
int scriptSaveDVD(int n,Arg *args);
int scriptVideoCodec(int n,Arg *args);
int scriptVideoCodec2(int n,Arg *args);
int scriptSave(int n,Arg *args);
int scriptLoadFilter(int n,Arg *args);
int scriptAddVideoFilter(int n,Arg *args);
int scriptRemoveFrame(int n,Arg *args);
int scriptSaveOgm(int n, Arg *args);
int scriptIndexMpeg(int n, Arg *args);

int scriptOutputFormat(int n, Arg *srgs);

// All comment must be in lower case!
// A number of param > 0x100 means variable, at least yy with value = yy+0x100
// For video, we need at least one : The video filter name
static const admCommand myCommands[]=
{
	
	{"load", 		scriptLoad,1,APM_STRING},
	{"append",		scriptAppend,1,APM_STRING},
	
	{"videoprocess",	scriptVideoProcess,1,APM_NUM},
	{"audioprocess",	scriptAudioProcess,1,APM_NUM},
	
	{"savejpg",		scriptSavejpeg,1,APM_STRING},
	{"saverawvideo",	scriptRaw,1,APM_STRING},
	
	{"savedvd",		scriptSaveDVD,1,APM_STRING},
	{"save",		scriptSave,1,APM_STRING},
	{"saveogm",		scriptSaveOgm,1,APM_STRING},
	
	{"setformat",   scriptOutputFormat,1,APM_STRING},
	{"normalize",		scriptAudioNormalize,1,APM_NUM},
	{"downsample",		scriptAudioDownsample,1,APM_NUM},
	{"resample",		scriptAudioResample,1,APM_NUM},
	{"delay",		scriptAudioDelay,1,APM_NUM},
	{"film2pal",		scriptFilm2Pal,1,APM_NUM},
	
	{"loadaudio",		scriptLoadAudio,2,APM_STRING,APM_STRING},
	
	{"saveaudio",		scriptSaveAudio,1,APM_STRING},
	{"audiocodec",		scriptAudioCodec,2,APM_STRING,APM_NUM},
	{"videocodec",		scriptVideoCodec,2,APM_STRING,APM_STRING},
	{"videocodec",		scriptVideoCodec2,3,APM_STRING,APM_STRING,APM_STRING},
	
	{"loadfilter",		scriptLoadFilter,1,APM_STRING},
	
	{"scanvbr",		scriptScanVbr,0},
	//{"loadcodec",		scriptLoadCodec,1,APM_STRING},
	
	{"goto",		scriptGoto,1,APM_NUM},
	
	{"setfps",		scriptSetFps,1,APM_FLOAT},
	
	{"sleep",		scriptSleep,1,APM_NUM},
	
	{"setmarkera",		scriptSetMarkerA,1,APM_NUM},
	{"setmarkerb",		scriptSetMarkerB,1,APM_NUM},
	
	{"addvideofilter",	scriptAddVideoFilter,0x101,APM_STRING},
	{"removeframes",	scriptRemoveFrame,2,APM_NUM,APM_NUM},
        
        {"indexmpeg",           scriptIndexMpeg,3,APM_STRING,APM_STRING,APM_NUM},
	
	{"exit", 		scriptExit,0}
};
