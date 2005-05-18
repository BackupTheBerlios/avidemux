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
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <ADM_assert.h>
#ifndef CYG_MANGLING
 #include "unistd.h"
#endif
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
#include "ADM_filter/video_filters.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_SCRIPT
#include "ADM_toolkit/ADM_debug.h"


//_________________________

extern void HandleAction(Action act);
//_________________________
#include "adm_command.h" 
void ADS_commandList( void );
ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg,uint8_t fake);
//_______________________
#include "ADM_editor/ADM_outputfmt.h"
#include "ADM_gui2/GUI_ui.h"
typedef struct ADM_CONTAINER
{
    ADM_OUT_FORMAT type;
    const char     *name;
}ADM_CONTAINER;
#define MK_CONT(x) {ADM_##x,#x}

const ADM_CONTAINER container[]=
{
  MK_CONT(AVI),
  MK_CONT(OGM),
  MK_CONT(ES),
  MK_CONT(PS),
  MK_CONT(AVI_DUAL),
  MK_CONT(AVI_UNP),
  MK_CONT(FMT_DUMMY)  
};    
#define NB_CONT sizeof(container)/sizeof(ADM_CONTAINER)
//*************************************
int scriptClearSegments(int n, Arg *srgs)
{
        if(n!=0)
                {
                        return 0;
                }
        return video_body->deleteAllSegments();
}
/*
        Param #1, source nb
        Param #2, start frame
        Param #3  Nb Frame
*/
int scriptAddSegment(int n, Arg *args)
{
        if(n!=3) return 0;
        return video_body->addSegment(args[0].arg.integer,
                                        args[1].arg.integer,
                                        args[2].arg.integer);

}

//*************************************
extern uint8_t indexMpeg(char *mpeg,char *file,uint8_t aid);
int scriptIndexMpeg(int n,Arg *args)
{
  char *in,*out;
  uint32_t aid;
  
  aid=args[2].arg.integer;
  
  in=args[0].arg.string; 
  out=args[1].arg.string;
  
  return indexMpeg(in,out,aid);
  
  
  
}
int scriptOutputFormat(int n, Arg *args)
{
    const char *str=args[0].arg.string;
    for(int i=0;i<NB_CONT;i++)
    {
        if(!strcmp(str,container[i].name))
        {
                 UI_SetCurrentFormat(container[i].type);
                 return 1;   
        }    
    }    
    printf("\n Cannot set output format\n");
    return 0;
}    
//_______________________
extern VF_FILTERS 	filterGetTagFromName(char *inname);
int scriptAddVideoFilter(int n,Arg *args)
{
VF_FILTERS filter;
		filter=filterGetTagFromName(args[0].arg.string);
		if(filter==VF_DUMMY) return 0;
		printf("Filter tag :%d\n",filter);
		return (filterAddScript(filter,n,args));
		
}
//___________________________
extern uint8_t A_ListAllBlackFrames( char *file );
int scriptScanBlackFrames(int n, Arg *args)
{

    return A_ListAllBlackFrames( args[0].arg.string );
}
//___________________________
extern int ogmSave(char *name);
int scriptSaveOgm(int n,Arg *args)
{
char *a;
		a=args[0].arg.string;
		return ogmSave(a);
}
//___________________________
extern int A_delete(uint32_t start, uint32_t end);
int scriptRemoveFrame(int n,Arg *args)
{
uint32_t a,b;
	a=args[0].arg.integer;
	b=args[1].arg.integer;
             if(avifileinfo && a >= avifileinfo->nb_frames)
                       a = avifileinfo->nb_frames-1;
             if(avifileinfo && b >= avifileinfo->nb_frames)
                       b = avifileinfo->nb_frames-1;
		
	return A_delete(a,b);
}
//_______________________
extern int filterLoadXml(char *docname,uint8_t silent);
int scriptLoadFilter(int n,Arg *args)
{
	return filterLoadXml(args[0].arg.string,0);
}

//_______________________
extern int A_Save (char *name);
int scriptSave(int n,Arg *args)
{
	return A_Save(args[0].arg.string);
}
//_______________________

int scriptVideoCodec(int n,Arg *args)
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
int scriptVideoCodec2(int n,Arg *args)
{
	// Load codec specific file
	if(! loadVideoCodecConf( args[2].arg.string)) return 0;
	return scriptVideoCodec(n,args);

}
//_______________________
extern int A_saveDVDPS(char *name);
int scriptSaveDVD(int n,Arg *args)
{
	return A_saveDVDPS(args[0].arg.string);
}
//_______________________
extern int ADM_saveRaw (char *name);
int scriptRaw(int n,Arg *args)
{
	return ADM_saveRaw(args[0].arg.string);
}
//_______________________
extern int A_saveJpg (char *name);
int scriptSavejpeg(int n,Arg *args)
{
	return A_saveJpg(args[0].arg.string);
}
//_______________________
int scriptAudioCodec(int n,Arg *args)
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
int scriptLoadCodec(int n,Arg *args)
{
	return loadVideoCodecConf(args[0].arg.string);
}
//_______________________
int scriptFilm2Pal(int n,Arg *args)
{
	return audioFilterFilm2Pal(args[0].arg.integer);
}
//_________Sleep in ms________________
int scriptAudioDelay(int n,Arg *args)
{
		return audioFilterDelay(args[0].arg.integer);				
}
//_________Sleep in ms________________
int scriptScanVbr(int n,Arg *args)
{
	HandleAction(ACT_AudioMap);
	return 1;

}
//_________Sleep in ms________________
int scriptSetMarkerA(int n,Arg *args)
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
int scriptSetMarkerB(int n,Arg *args)
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
int scriptSetFps(int n,Arg *args)
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
int scriptSleep(int n,Arg *args)
{
	usleep(args[0].arg.integer*1000);
	return 1;

}
//________________________________________________
extern int A_audioSave(char *name);
int scriptSaveAudio(int n,Arg *args)
{
	return A_audioSave(args[0].arg.string);
}
//________________________________________________
extern int GUI_loadMP3(char *name);
extern int A_loadAC3(char *name);
extern int A_loadWave(char *name);
int scriptLoadAudio(int n,Arg *args)
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
int scriptAudioProcess(int n,Arg *args)
{	
	UI_setAProcessToggleStatus( args[0].arg.integer );
	return 1;
}
//________________________________________________
extern int GUI_GoToFrame(uint32_t frame);
int scriptGoto(int n,Arg *args)
{	
 int frameno;
 	if (!avifileinfo) return 0;
	
 	frameno=args[0].arg.integer;
	if( frameno<0)
	{
		aviInfo info;
		video_body->getVideoInfo(&info);
		frameno=-frameno;
		if(frameno>info.nb_frames) return 0;
		
		frameno=info.nb_frames-frameno;
	}
	return GUI_GoToFrame( frameno );
	
}
//________________________________________________
int scriptVideoProcess(int n,Arg *args)
{	
	UI_setVProcessToggleStatus( args[0].arg.integer );
	return 1;
}
//________________________________________________
int scriptAudioNormalize(int n,Arg *args)
{	
	audioFilterNormalize(1);
	return 1;
}
//________________________________________________
int scriptAudioDownsample(int n,Arg *args)
{	
	audioFilterDownsample( args[0].arg.integer );
	return 1;
}
	
	//________________________________________________
int scriptAudioResample(int n,Arg *args)
{	
	audioFilterResample( args[0].arg.integer );
	return 1;
}
	
//________________________________________________
extern uint8_t addFile(char *name);
extern int A_appendAvi (char *name);
int scriptAppend(int n,Arg *args)
{
  return A_appendAvi (args[0].arg.string);
	
}
//________________________________________________
extern int A_openAvi (char *name);
int scriptLoad(int n,Arg *args)
{	
	return A_openAvi (args[0].arg.string);
}
//________________________________________________
int scriptExit(int n,Arg *args)
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
	ADM_assert(nb<MAXPARAM);
	// First go to lowercase
	LowerCase(cmd);
	// 1- lookup the command
	//_______________________
	for(int i=sizeof(myCommands)/sizeof(admCommand)-1;i>=0;i--)
	{
		if(!strcmp(cmd,myCommands[i].command))
		{
			candidate=i;
			if(myCommands[i].nbArgs>=0x100)
			{
				if(nb>=(myCommands[i].nbArgs&0xff))
				{
					found=i;
					break;
				}
			}
			else
			{
				if(nb==myCommands[i].nbArgs)
				{
					found=i;
					break;
				}
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
	uint32_t check;
	check=myCommands[found].nbArgs;
	if(check>0x100)
	{
		check=check & 0xff;
	}
	else
		check=nb;
	for(int i=0;i<check;i++)
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
	if( myCommands[found].func(nb,arg))
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
