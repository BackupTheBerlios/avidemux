/*
	Type used by scanner

*/

typedef enum 
{
	APM_NUM,
	APM_HEXNUM,
	APM_STRING,
	APM_QUOTED,
	APM_FLOAT,
	APM_LAST

}APM_TYPE;
int PushParam(APM_TYPE, char *value);
int Call(char *string);
typedef union
{
	int 	integer;
	float	real;
	char	*string;

}ArgI;

typedef struct Arg
{
	APM_TYPE type;
	ArgI	 arg;

};
#define MAXPARAM 20
//END

