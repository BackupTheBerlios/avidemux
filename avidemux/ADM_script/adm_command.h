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
	
	{"normalize",		scriptAudioNormalize,1,APM_NUM},
	{"downsample",		scriptAudioDownsample,1,APM_NUM},
	{"resample",		scriptAudioResample,1,APM_NUM},
	
	{"loadaudio",		scriptLoadAudio,2,APM_STRING,APM_STRING},
	{"saveaudio",		scriptSaveAudio,1,APM_STRING},
	
	{"goto",		scriptGoto,1,APM_NUM},
	
	{"sleep",		scriptSleep,1,APM_NUM},
	{"exit", 		scriptExit,0}
};
