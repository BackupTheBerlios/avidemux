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
#include <string.h>
#include <assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "adm_scanner.h" 

int scriptLoad(Arg *args);
int scriptExit(Arg *args);
int scriptAppend(Arg *args);
int scriptAudioProcess(Arg *args);
int scriptVideoProcess(Arg *args);

int scriptAudioDownsample(Arg *args);
int scriptAudioResample(Arg *args);
int scriptAudioNormalize(Arg *args);
#include "adm_command.h" 
void ADS_commandList( void );
ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg);
//________________________________________________
int scriptAudioProcess(Arg *args)
{	
	UI_setAProcessToggleStatus( args[0].arg.integer );
	return 1;
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
ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg)
{
int found=-1;
	assert(nb<MAXPRM);
	// First go to lowercase
	for(int i=strlen(cmd)-1;i>=0;i--)
	{
		cmd[i]=tolower(cmd[i]);
	}
	// 1- lookup the command
	//_______________________
	for(int i=sizeof(myCommands)/sizeof(admCommand)-1;i>=0;i--)
	{
		if(!strcmp(cmd,myCommands[i].command))
		{
			found=i;
			break;
		}
	}
	if(found==-1)
	{
		
		return ASC_UNKNOWN_FUNC;
	}
	// 2- check # of parameters
	//_______________________
	if(nb!=myCommands[found].nbArgs)
	{
		
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
