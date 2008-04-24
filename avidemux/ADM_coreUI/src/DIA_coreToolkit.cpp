/** *************************************************************************
        \file                  DIA_coreToolkit.cpp
 
		(c) 2008 Mean, fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ADM_default.h"
#include "DIA_coreToolkit.h"
#include "DIA_coreUI_internal.h"
#include "DIA_factoryStubs.h"
#include <stdarg.h>
static CoreToolkitDescriptor *Toolkit=NULL;
#define MAX_ALERT_SIZE 1024
/**
 * 	\fn DIA_toolkitInit
 *  \brief Hook a set of coreUIToolkit into the dll (needed for win32)
 */
uint8_t  DIA_toolkitInit(CoreToolkitDescriptor *d)
{
	uint32_t major,minor,patch;
	Toolkit=d;
	return 1;
}
/**
 * 	\fn GUI_Info_HIG
 *  \brief Display a warning/info/debug message. The primary field is the "title" of the window, secondary format is printf like
 */

void            GUI_Info_HIG(const ADM_LOG_LEVEL level,const char *primary, const char *secondary_format, ...)
{
	char text[MAX_ALERT_SIZE+1] = {0};
	ADM_assert(Toolkit);
	va_list ap;
	
		va_start(ap, secondary_format);
		if (secondary_format)
		{
			 vsnprintf(text,
						MAX_ALERT_SIZE,
						secondary_format, 
						ap);
		}		
		va_end(ap);
		
	Toolkit->infoHig(level,primary,text);
	
}
/**
 * 	\fn GUI_Error_HIG
 *  \brief Display an error message. The primary field is the "title" of the window, secondary format is printf like
 */

void            GUI_Error_HIG(const char *primary, const char *secondary_format, ...)
{
	char text[MAX_ALERT_SIZE+1] = {0};
		ADM_assert(Toolkit);
		va_list ap;
		
			va_start(ap, secondary_format);
			if (secondary_format)
			{
				 vsnprintf(text,
							MAX_ALERT_SIZE,
							secondary_format, 
							ap);
			}		
			va_end(ap);
	Toolkit->errorHig(primary,text);	
}
/**
 * 	\fn GUI_Confirmation_HIG
 *  \brief ask for confirmation. The button_confirm will be the title of the button (accept, do it,...)
 */

int             GUI_Confirmation_HIG(const char *button_confirm, const char *primary, const char *secondary_format, ...)
{
	char text[MAX_ALERT_SIZE+1] = {0};
			ADM_assert(Toolkit);
			va_list ap;
			
				va_start(ap, secondary_format);
				if (secondary_format)
				{
					 vsnprintf(text,
								MAX_ALERT_SIZE,
								secondary_format, 
								ap);
				}		
				va_end(ap);
	return Toolkit->confirmationHig(button_confirm,primary,text);	
}
/**
 * 	\fn GUI_YesNo
 *  \brief Ask for yes/no. Yes will return 1, No will return 0
 */

int             GUI_YesNo(const char *primary, const char *secondary_format, ...)
{
	char text[MAX_ALERT_SIZE+1] = {0};
			ADM_assert(Toolkit);
			va_list ap;
			
				va_start(ap, secondary_format);
				if (secondary_format)
				{
					 vsnprintf(text,
								MAX_ALERT_SIZE,
								secondary_format, 
								ap);
				}		
				va_end(ap);
	return Toolkit->yesno(primary,text);	
}
/**
 * 	\fn GUI_Question
 *  \brief About the same as GUI_YesNo, the button will be ok/cancel
 */
int             GUI_Question(const char *alertstring)
{
	ADM_assert(Toolkit);
	return Toolkit->question(alertstring);	
}
/**
 * 	\fn GUI_Alternate
 *  \brief Ask to choose between choice1 and choice2
 * 
 */
int             GUI_Alternate(char *title,char *choice1,char *choice2)
{
	ADM_assert(Toolkit);
	return Toolkit->alternate(title,choice1,choice2);	
}

/**
 * 	\fn GUI_Verbose
 *  \brief  Set ui in verbose mode (default). Show all popup & questions
 */
void            GUI_Verbose(void)
{
	ADM_assert(Toolkit);
	Toolkit->verbose();	
}
/**
 * 	\fn GUI_Quiet
 *  \brief Set the ui in silent mode. All popups & questions will be answered with their default value
 */
void            GUI_Quiet(void)
{
	ADM_assert(Toolkit);
	Toolkit->quiet();	
}
/**
 * 	\fn GUI_isQuiet
 *  \brief  Is the UI in quiet mode ?
 */
uint8_t			GUI_isQuiet(void)
{
	ADM_assert(Toolkit);
	return Toolkit->isQuiet();	
}


