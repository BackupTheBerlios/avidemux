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


typedef struct admCommand
{
	char 		*command;
	uint32_t	nbArgs;
	Arg		*args;
};

static const admCommand myCommands[]=
{
	{"exit",0,NULL}
};
