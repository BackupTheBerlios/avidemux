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
	APM_BOOL,
	APM_LAST

}APM_TYPE;

typedef enum
{
	ASC_OK,
	ASC_UNKNOWN_FUNC,
	ASC_BAD_NUM_PARAM,
	ASC_BAD_PARAM,
	ASC_EXEC_FAILED
}ASC_ERROR;

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

