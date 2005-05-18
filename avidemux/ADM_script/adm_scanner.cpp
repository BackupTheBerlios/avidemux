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
#include <ctype.h>
#include <ADM_assert.h>
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
char *script_getVar(char *in, enum yytokentype *r);
};
extern FILE *yyin;
extern int yylineno;
extern int yydebug;

static void CleanParam(void);
static uint32_t pushed;
static Arg args[MAXPARAM];
void dumpStack( char *cmd, int nb, Arg *arg);
extern ASC_ERROR ADS_execCommand(char *cmd, int nb, Arg *arg,uint8_t fake);
extern void GUI_Quiet( void );
extern void GUI_Verbose( void );
static char scriptError[1024];
static uint8_t thisIsADrill;


typedef struct scriptVar
{
        char *name;
        char *string;
        enum yytokentype type;
}scriptVar;


static scriptVar myVars[ADM_MAX_VAR];
uint32_t nbVar=0;
//_____________________________________
uint8_t parseScript(char *scriptname);
//_____________________________________
uint8_t parseScript(char *scriptname)
{
int i;
	//yydebug=1;
	// Do it one time to check syntax/grammar
	//_______________________________________
	thisIsADrill=1;
	printf("Checking syntax/grammar...\n");
	yyin=fopen(scriptname,"rt");
	if(!yyin) 
	{
		GUI_Alert("Cannot open that file");
		return 0;	
	}
	pushed=0;
	i=yyparse();
	fclose(yyin);
	if(i)
	{
		printf("Error %s at line %d\n",scriptError,yylineno);
		return 0;
	}
	// Now do it for real
	//_______________________________________
	printf("Syntax/grammar ok, executing\n");
	thisIsADrill=0;
	yyin=fopen(scriptname,"rt");
	if(!yyin) 
	{
		GUI_Alert("Cannot open that file");
		return 0;	
	}
	pushed=0;
	GUI_Quiet();
	i=yyparse();
	GUI_Verbose();
	fclose(yyin);
	if(!i)
		{
			printf("Parsing successfull\n");
	//		exit(0); ;
                        return 1;
		}
	
	return 0;
	//exit(-1) ;
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
char *command=ADM_strdup(value);
ASC_ERROR status;
int	  ret=1;
	
	
	status=ADS_execCommand(value,pushed,args,thisIsADrill);
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
	ADM_dealloc(command);
	return ret;
}
//______________________________
void CleanParam(void)
{
	
	for(uint32_t i=0;i<pushed;i++)
	{
		if(args[i].type==APM_STRING)
		{
			ADM_dealloc(args[i].arg.string);
			args[i].arg.string=NULL;
		}	
	}
	pushed=0;

}
/*
        Push a var into the stack var

*/
uint8_t scriptAddVar(char *var,char *value)
{
        if(!var || !strlen(var)) 
        {
                printf("Script : Var name invalid\n");
                return 0;
        }
        if(!value || !strlen(value)) 
        {
                printf("Script : value invalid\n");
                return 0;
        }
        myVars[nbVar].name=ADM_strdup(var);
        myVars[nbVar].string=ADM_strdup(value);
        // check it is a number
        uint8_t digit=1;
        for(int i=0;i<strlen(value);i++)
        {
                 if(!isdigit(value[i]))
                        {digit=0;break;}
        }
        if(digit)
             myVars[nbVar].type=(enum yytokentype)NUMBER;
        else
             myVars[nbVar].type=(enum yytokentype)STRING;
        nbVar++;
        return 1;

}
/*
       Retrieve a var in the stack

*/
char *script_getVar(char *in, enum yytokentype *r)
{

        printf("Get var called with in=[%s]\n",in);
        for(uint32_t i=0;i<nbVar;i++)
        {
                if(myVars[i].name)
                {
                        if(!strcmp(myVars[i].name,in+1)) // skip the  $ 
                        {
                                *r=(enum yytokentype)myVars[i].type;
                                return ADM_strdup(myVars[i].string);
                        }

                }
        }
        printf("Warning: var [%s] is unknown !\n");
        return NULL;

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
		case APM_BOOL:
			LowerCase(value);
			if(!strcmp(value,"true") || !strcmp(value,"on"))
			{
				myarg->type=APM_NUM;
				myarg->arg.integer=1;				
			}
			else if(!strcmp(value,"false") || !strcmp(value,"off"))
			{
				myarg->type=APM_NUM;
				myarg->arg.integer=0;
			}
			else return 0;
			break;
		case APM_NUM: 
			myarg->type=APM_NUM;
			myarg->arg.integer=atoi(value);			
			break;
		case APM_STRING: 
			alt=ADM_strdup(value);			
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
			alt=ADM_strdup(value+1);			
			alt[strlen(alt)-1]=0;			
			myarg->type=APM_STRING;
			myarg->arg.string=alt;			
			break;
		default:
			return 0;
	}	
	pushed++;
	ADM_assert(pushed<MAXPARAM);
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

