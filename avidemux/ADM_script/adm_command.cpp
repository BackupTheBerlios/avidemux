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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "adm_scanner.h" 
#include "adm_command.h" 

int ADS_execCommand(char *cmd, int nb, Arg *arg);

//
//	This is where the fun begins
//	We 
//		- search the command through the loop
//		- check the params
//		- invoke the caller function
//	
//_____________________________________________
int ADS_execCommand(char *cmd, int nb, Arg *arg)
{
int found=-1;
	// First go to lowercase
	for(int i=strlen(cmd)-1;i>=0;i--)
	{
		cmd[i]=tolower(cmd[i]);
	}
	// then lookup the command
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
		printf("Command : %s unknown\n",cmd);
		return 0;
	}
	// 2 check # of parameter
	if(nb!=myCommands[found].nbArgs)
	{
		printf("Command %s : expecting %d parameters, got %d\n",cmd,myCommands[found].nbArgs,nb);
		return 0;
	}
	printf("Command %s ok\n",cmd);
	return 1;
}
