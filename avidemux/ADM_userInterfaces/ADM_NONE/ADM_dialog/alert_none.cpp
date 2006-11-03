# include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "default.h"
#include "prefs.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_assert.h" 



void            GUI_Alert(const char *alertstring)
{
  printf("Alert : %s\n", alertstring);
}
void            GUI_Info(const char *alertstring)
{
  printf("Info : %s\n", alertstring);
}

void            GUI_Info_HIG(const ADM_LOG_LEVEL level,const char *primary, const char *secondary_format, ...)
{
  
}
void            GUI_Error_HIG(const char *primary, const char *secondary_format, ...)
{
  printf("Error : %s\n", primary);
}
int             GUI_Confirmation_HIG(const char *button_confirm, const char *primary, const char *secondary_format, ...)
{
  printf("confirmation : %s\n", button_confirm);
  return 0; 
}
int             GUI_YesNo(const char *primary, const char *secondary_format, ...)
{
  printf("yesno : %s\n", primary);
  return 0; 
}

int             GUI_Question(const char *alertstring)
{
  
  printf("GUI_Question : %s\n", alertstring);
  return 0; 
}

int      GUI_Alternate(char *title,char *choice1,char *choice2)
{
  printf("GUI_Alternate : %s\n", title);
  return 0; 
}

uint8_t  GUI_getDoubleValue(double *valye, float min, float max, const char *title)
{
    *valye=min;
  return 0; 
}
uint8_t  GUI_getIntegerValue(int *valye, int min, int max, const char *title)
{
  *valye=min;
  return 0; 
}

uint8_t		isQuiet(void)
{
    return 1;
}
void            GUI_Verbose(void)
{
}
void            GUI_Quiet(void)
{
}
