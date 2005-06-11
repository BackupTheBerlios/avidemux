%{
#include <stdio.h>
#define YYSTYPE char * 
#include "adm_yacgen.h"
char *script_getVar(char *in, enum yytokentype *r);
static enum yytokentype result;
%}
%%
true				yylval=yytext;return BOOL;
false				yylval=yytext;return BOOL;
[Oo][Nn]			yylval=yytext;return BOOL;
[oO][fF][fF]			yylval=yytext;return BOOL;
\"[0-9a-zA-Z_\ /\.\-:]+\" 	yylval=yytext;return QUOTEDNAME;
\$[0-9a-zA-Z_]+			yylval=script_getVar(yytext,&result);return result;
[0-9]+				yylval=yytext;return NUMBER;
\-[0-9]+			yylval=yytext;return NUMBER;
\-*([0-9])+"."([0-9])*		yylval=yytext;return FLOAT;
0x[0-9a-fA-F]+  		yylval=yytext;return HEXNUMBER;
[a-zA-Z0-9_/\.=\-:]+	  		yylval=ADM_strdup(yytext);return STRING;
\(				return OPEN;
\)				return CLOSE;
\/\/.*				return COMMENT;
\#.*				return OTHERCOMMENT;
,				return COMMA;
;				return END;
%%
