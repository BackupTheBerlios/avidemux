//____________________________________________________________________
//
// C++ Implementation: adm_scanner
//
// Description: 
//	Interface to lex/yacc scanner to read a simple script
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
//____________________________________________________________________
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
#define YYSTYPE char * 
extern "C" 
{
#include "adm_yacgen.h"
#include "adm_scanner.h" 
void 	yyerror(const char *str);
int 	yywrap(void);
int	yyparse(void);
};
extern FILE *yyin;
extern int yylineno;
extern int yydebug;

static void CleanParam(void);
static uint32_t pushed;
static Arg args[MAXPARAM];
void dumpStack( char *cmd, int nb, Arg *arg);
extern ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg);

static char scriptError[1024];
//_____________________________________
void parseScript(char *scriptname);
//_____________________________________
void parseScript(char *scriptname)
{
int i;
	//yydebug=1;
	yyin=fopen(scriptname,"rt");
	if(!yyin) 
	{
		GUI_Alert("Cannot open that file");
		return ;	
	}
	pushed=0;
	i=yyparse();
	fclose(yyin);
	if(!i)
		{
			printf("Parsing successfull\n");
			return ;
		}
	
	printf("Error %s at line %d\n",scriptError,yylineno);
	return ;
}
//______________________________
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s at line %d \n",str,yylineno);
}
//______________________________ 
int yywrap(void)
{
        return 1;
} 
// _________ function that handles the call ________
//
//	value contains the function
//	pushed contains the # of parameters
//	args[i] is the nth parameter
int Call( char *value)
{
char *command=strdup(value);
ASC_ERROR status;
int	  ret=1;
	
	
	status=ADS_execCommand(value,pushed,args);
	if(status!=ASC_OK)
	{
		ret=0;
		switch(status)
		{
			case(ASC_UNKNOWN_FUNC):
					sprintf(scriptError,"Unknown function");
					break;
			case(ASC_BAD_NUM_PARAM):
					sprintf(scriptError,"Bad number of parameter");
					break;
			case(ASC_BAD_PARAM):
					sprintf(scriptError,"Wrong parameter type");
					break;
			case(ASC_EXEC_FAILED):
					sprintf(scriptError,"Execution failed");
					break;
			default:
					sprintf(scriptError,"Unknown error");
					break;
		}
		printf("** Script error : %s, command :****\n",scriptError);
		dumpStack(value,pushed,args);	
	}
	
	CleanParam();
	free(command);
	return ret;
}
//______________________________
void CleanParam(void)
{
	
	for(uint32_t i=0;i<pushed;i++)
	{
		if(args[i].type==APM_STRING)
		{
			free(args[i].arg.string);
			args[i].arg.string=NULL;
		}	
	}
	pushed=0;

}
//______________________________
int PushParam(APM_TYPE type, char *value)
{
int val;
Arg *myarg;
char *alt;
	myarg=&args[pushed];
	switch(type)
	{
		case APM_NUM: 
			myarg->type=APM_NUM;
			myarg->arg.integer=atoi(value);
			break;
		case APM_STRING: 
			alt=strdup(value);			
			myarg->type=APM_STRING;
			myarg->arg.string=alt;
			break;
		case APM_HEXNUM: 
			myarg->type=APM_NUM;
			sscanf(value,"%x",&(myarg->arg.integer));
			break;
		case APM_FLOAT:
			myarg->type=APM_FLOAT;
			myarg->arg.real=atof(value);			
			break;
		case APM_QUOTED:
		// Get rid of "" 
			alt=strdup(value+1);			
			alt[strlen(alt)-1]=0;			
			myarg->type=APM_STRING;
			myarg->arg.string=alt;			
			break;
		default:
			return 0;
	}	
	pushed++;
	assert(pushed<MAXPARAM);
	return 1;

}
void dumpStack( char *cmd, int nb, Arg *arg)
{
	printf("Command : <%s>, with %d args\n",cmd,nb);
	for(uint32_t i=0;i<nb;i++)
	{	
		switch(arg[i].type)
		{
			case APM_STRING:
					printf("\t%d is a string: <%s>\n",i,arg[i].arg.string);
					break;
			case APM_NUM:
					printf("\t%d is a integer: <%d>\n",i,arg[i].arg.integer);
					break;
			case APM_FLOAT:
					printf("\t%d is a float: <%f>\n",i,arg[i].arg.real);
					break;
		
		
		}
	
	}
}
//______________________________

