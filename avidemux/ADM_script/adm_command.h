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

#define MAXPRM 10
typedef int (*dummy_func)(Arg *args);
typedef struct admCommand
{
	char 		*command;
	dummy_func	func;
	uint32_t	nbArgs;
	APM_TYPE	args[MAXPRM];	// 10 params should be enough
};
// All comment must be in lower case!
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
	
	{"exit", 		scriptExit,0}
};
