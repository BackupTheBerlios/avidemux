//
// C++ Implementation: adm_command
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "adm_scanner.h" 
#include "avi_vars.h"
#include "gui_action.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encConfig.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_SCRIPT
#include "ADM_toolkit/ADM_debug.h"


//_________________________

int scriptLoad(Arg *args);
int scriptExit(Arg *args);
int scriptAppend(Arg *args);
int scriptAudioProcess(Arg *args);
int scriptVideoProcess(Arg *args);

int scriptAudioDownsample(Arg *args);
int scriptAudioResample(Arg *args);
int scriptAudioNormalize(Arg *args);

int scriptLoadAudio(Arg *args);
int scriptSaveAudio(Arg *args);
int scriptGoto(Arg *args);

int scriptSetFps(Arg *args);

int scriptSleep(Arg *args);
int scriptSetMarkerA(Arg *args);
int scriptSetMarkerB(Arg *args);
int scriptScanVbr(Arg *args);
int scriptAudioDelay(Arg *args);
int scriptFilm2Pal(Arg *args);
int scriptAudioCodec(Arg *args);
int scriptLoadCodec(Arg *args);
int scriptSavejpeg(Arg *args);
int scriptRaw(Arg *args);
int scriptSave(Arg *args);
int scriptSaveDVD(Arg *args);
int scriptVideoCodec(Arg *args);
int scriptVideoCodec2(Arg *args);
int scriptSave(Arg *args);
int scriptLoadFilter(Arg *args);

extern void HandleAction(Action act);
//_________________________
#include "adm_command.h" 
void ADS_commandList( void );
ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg,uint8_t fake);
//_______________________

extern int filterLoadXml(char *docname,uint8_t silent);
int scriptLoadFilter(Arg *args)
{
	return filterLoadXml(args[0].arg.string,0);
}

//_______________________
extern int A_Save (char *name);
int scriptSave(Arg *args)
{
	return A_Save(args[0].arg.string);
}
//_______________________

int scriptVideoCodec(Arg *args)
{
char *codec,*conf;
	codec=args[0].arg.string;
	conf=args[1].arg.string;
	if(!videoCodecSelectByName(codec)) 
	{
		return 0;
	}
	// now do the conf
	// format CBR=bitrate in kbits
	//	  CQ=Q
	//	  2 Pass=size
	// We have to replace 
	if(!videoCodecConfigure(conf,0,NULL))
		return 0;	
	return 1;
}
extern uint8_t loadVideoCodecConf( char *name);
int scriptVideoCodec2(Arg *args)
{

	
	// Load codec specific file
	if(! loadVideoCodecConf( args[2].arg.string)) return 0;
	return scriptVideoCodec(args);

}
//_______________________
extern int A_saveDVDPS(char *name);
int scriptSaveDVD(Arg *args)
{
	return A_saveDVDPS(args[0].arg.string);
}
//_______________________
extern int ADM_saveRaw (char *name);
int scriptRaw(Arg *args)
{
	return ADM_saveRaw(args[0].arg.string);
}
//_______________________
extern int A_saveJpg (char *name);
int scriptSavejpeg(Arg *args)
{
	return A_saveJpg(args[0].arg.string);
}
//_______________________
int scriptAudioCodec(Arg *args)
{
char *name=args[0].arg.string;
	LowerCase(name);
	// First search the codec by its name
	if(!audioCodecSetByName(name)) return 0;
	// set set bitrate
	audioFilter_SetBitrate( args[1].arg.integer);
	return 1;
}
//_______________________
extern uint8_t loadVideoCodecConf( char *name);
int scriptLoadCodec(Arg *args)
{
	return loadVideoCodecConf(args[0].arg.string);
}
//_______________________
int scriptFilm2Pal(Arg *args)
{
	return audioFilterFilm2Pal(args[0].arg.integer);
}
//_________Sleep in ms________________
int scriptAudioDelay(Arg *args)
{
		return audioFilterDelay(args[0].arg.integer);				
}
//_________Sleep in ms________________
int scriptScanVbr(Arg *args)
{
	HandleAction(ACT_AudioMap);
	return 1;

}
//_________Sleep in ms________________
int scriptSetMarkerA(Arg *args)
{
int	f=args[0].arg.integer;
	

	if (!avifileinfo)
	{
		
		return 0;
	} 
	if(f==-1) f=avifileinfo->nb_frames-1;
	if(f<0 || f>avifileinfo->nb_frames-1) return 0;
	frameStart=f;  
	return 1;

}
//_________Sleep in ms________________
int scriptSetMarkerB(Arg *args)
{
int	f=args[0].arg.integer;
	

	if (!avifileinfo)
	{
		
		return 0;
	} 
	if(f==-1) f=avifileinfo->nb_frames-1;
	if(f<0 || f>avifileinfo->nb_frames-1) return 0;
	frameEnd=f;  
	return 1;

}

//_________Sleep in ms________________
int scriptSetFps(Arg *args)
{
	float fps;
	aviInfo info;

	if (avifileinfo)
	{
		video_body->getVideoInfo(&info);				
		info.fps1000 = (uint32_t)floor(args[0].arg.real*1000.f);
		video_body->updateVideoInfo (&info);
		video_body->getVideoInfo (avifileinfo);
		return 1;
	} else 
	{
		return 0;
	}

}

//_________Sleep in s________________
int scriptSleep(Arg *args)
{
	usleep(args[0].arg.integer*1000);
	return 1;

}
//________________________________________________
extern int A_audioSave(char *name);
int scriptSaveAudio(Arg *args)
{
	return A_audioSave(args[0].arg.string);
}
//________________________________________________
extern int GUI_loadMP3(char *name);
extern int A_loadAC3(char *name);
extern int A_loadWave(char *name);
int scriptLoadAudio(Arg *args)
{	
	char *type=args[0].arg.string;
	
	LowerCase(type);
	if(!strcmp(type,"mp3") )
	{
		return GUI_loadMP3(args[1].arg.string);
		
	}
	if(!strcmp(type,"wav") )
	{
		return A_loadWave(args[1].arg.string);
		
	}
	if(!strcmp(type,"ac3") )
	{
		return A_loadAC3(args[1].arg.string);
		
	}	
	return 0;
}
//________________________________________________
int scriptAudioProcess(Arg *args)
{	
	UI_setAProcessToggleStatus( args[0].arg.integer );
	return 1;
}
//________________________________________________
extern int GUI_GoToFrame(uint32_t frame);
int scriptGoto(Arg *args)
{	
	if( args[0].arg.integer<0) return 0;
	return GUI_GoToFrame( args[0].arg.integer );
	
}
//________________________________________________
int scriptVideoProcess(Arg *args)
{	
	UI_setVProcessToggleStatus( args[0].arg.integer );
	return 1;
}
//________________________________________________
int scriptAudioNormalize(Arg *args)
{	
	audioFilterNormalize(1);
	return 1;
}
//________________________________________________
int scriptAudioDownsample(Arg *args)
{	
	audioFilterDownsample( args[0].arg.integer );
	return 1;
}
	
	//________________________________________________
int scriptAudioResample(Arg *args)
{	
	audioFilterResample( args[0].arg.integer );
	return 1;
}
	
//________________________________________________
extern uint8_t addFile(char *name);
extern int A_openAvi2 (char *name, uint8_t mode);
int scriptAppend(Arg *args)
{
	return A_openAvi2 (args[0].arg.string,0);
	
}
//________________________________________________

int scriptLoad(Arg *args)
{	
	return A_openAvi2 (args[0].arg.string,0);
}
//________________________________________________
int scriptExit(Arg *args)
{
	exit(0);
	return 1;
}
//________________________________________________
//
//	This is where the fun begins
//	We 
//		- search the command through the loop
//		- check the params
//		- invoke the caller function
//	
//_____________________________________________
ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg,uint8_t fake)
{
int found=-1;
int candidate=-1;
	assert(nb<MAXPRM);
	// First go to lowercase
	LowerCase(cmd);
	// 1- lookup the command
	//_______________________
	for(int i=sizeof(myCommands)/sizeof(admCommand)-1;i>=0;i--)
	{
		if(!strcmp(cmd,myCommands[i].command))
		{
			candidate=i;
			if(nb==myCommands[i].nbArgs)
			{
				found=i;
				break;
			}
			
		}
	}
	if(found==-1)
	{		
		if(candidate==-1)
			return ASC_UNKNOWN_FUNC;
		else
			return ASC_BAD_NUM_PARAM;
	}	
	// 3- check parameters type
	//_________________________
	for(int i=0;i<nb;i++)
	{
		if(arg[i].type!=myCommands[found].args[i])
		{
			
			return ASC_BAD_PARAM;
		}	
	}
	// 4- Exec
	//___________________________	
	if(fake)
		return ASC_OK;
	if( myCommands[found].func(arg))
		return ASC_OK;
	else
		return ASC_EXEC_FAILED;
}
void ADS_commandList( void )
{
	printf("List of functions available in script mode\n");
	for(int i=sizeof(myCommands)/sizeof(admCommand)-1;i>=0;i--)
	{
		printf("\n%s ( ",myCommands[i].command);
		for(int j=0;j<myCommands[i].nbArgs;j++)
		{
			switch(myCommands[i].args[j])
			{
				case APM_NUM: printf( " number ");break;
				case APM_STRING: printf( " string/file ");break;
				case APM_FLOAT: printf( " float ");break;
				default: printf(" ??? ");break;
			}		
		}
		printf(")");
	}
	printf("\n");
}
