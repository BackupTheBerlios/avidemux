/***************************************************************************
                          GUI_binding.cpp  -  description
                             -------------------
    begin                : Fri Jan 17 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#  include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "math.h"
#include "default.h"

#include "gui_action.hxx"
extern void HandleAction(Action act);  
// when keys are pressed
// We have to duplicate the ALT ... shortcut
// because of change of focus ?

void UI_on_key_press(GtkWidget *widget, GdkEventKey* event, gpointer user_data)
{
    UNUSED_ARG(widget);
    UNUSED_ARG(user_data);
	gboolean shift = FALSE;
	gboolean ctrl = FALSE;
	gboolean alt = FALSE;
	Action action;

	//printf("key : %d (%c) \n",event->keyval,event->keyval);
	
	if (event->state & GDK_CONTROL_MASK)
	{
		ctrl = TRUE;
	}
	if (event->state & GDK_SHIFT_MASK)
	{
		shift = TRUE;
	}
	if(event->state & GDK_MOD1_MASK)
	{
		alt = TRUE;
	}
	// ALT+x
	//_____________
	if(alt)
	{
	
		switch(event->keyval)
		{
			case GDK_l:
			case GDK_L: action=ACT_LoadWork;break;
			case GDK_s:
			case GDK_S: action=ACT_SaveWork;break;
				
			// repeat Keyboard stuff
			case GDK_B:
			
				if(shift)
					action=ACT_GotoMarkA;
				else
					action=ACT_MarkA;
				break;
			case GDK_E:
			
				if(shift)
					action=ACT_GotoMarkB;
				else
					action=ACT_MarkB;
				break;
			default:
				return;		
		}
		HandleAction(action);
		return;
	
	}
	action=ACT_DUMMY;
	// CTRL+x
	//_____________
	if(TRUE==ctrl)
	{
	  switch (event->keyval)
	  {
	  	case GDK_A:
		case GDK_a:
					action=ACT_AppendAvi;break;
					
	  	case GDK_c:
		case GDK_C:	action=ACT_Copy;break;
		
				
	  	case GDK_bracketleft:
					action = ACT_GotoMarkA;break;
		case GDK_bracketright:
					action = ACT_GotoMarkB;break;
					
		case GDK_F:
		case GDK_f:
					action=ACT_VideoParameter;break;					
					
	  	case GDK_G:
		case GDK_g:
					action=ACT_Goto;break;

	  	
		case GDK_M:
		case GDK_m:
					action=ACT_SaveImg;break;
		
		case GDK_O:
		case GDK_o:
					action=ACT_OpenAvi;break;
		case GDK_P:
		case GDK_p:
					action=ACT_DecoderOption;break;					
		
		case GDK_S:
		case GDK_s:
					action=ACT_SaveAvi;break;
		case GDK_u:
		case GDK_U:
					action=ACT_SetMuxParam;break;
		case GDK_v:
		case GDK_V:		action=ACT_Paste;break;		
		
		case GDK_X:
		case GDK_x:
					action=ACT_Delete;break;					
					
		default:
			break;

	  }
	}

	if(action!=ACT_DUMMY)
	{
		HandleAction(action);
		return;
	}

    switch (event->keyval)
	{
	case GDK_F1:
		action=ACT_VideoModeToggle;break;
	case GDK_F2:
		action=ACT_AudioModeToggle;break;
			
	case GDK_space:
		action=ACT_PlayAvi;
		break;
	case GDK_X:
		action=ACT_Delete;
		break;
	
	case GDK_Up:
		action=ACT_NextKFrame;
		break;
	case GDK_Down:
		action=ACT_PreviousKFrame;
		break;
			
		// Position advance
       	case GDK_Left: case GDK_KP_Left:

		// One frame
		if((shift == FALSE) && (ctrl == FALSE))
		{
			action = ACT_PreviousFrame;
		}
		// 50 frames
		else if(ctrl == TRUE)
		{
			action = ACT_Back50Frames;
		}
		// 25 frames
		else
		{
			action = ACT_Back25Frames;
		}
		break;

		// Position reverse
	case GDK_Right: case GDK_KP_Right:
		if((shift == FALSE) && (ctrl == FALSE))
		{
			action = ACT_NextFrame;
		}
		else if(ctrl == TRUE)
		{
			action = ACT_Forward50Frames;
		}
		else
		{
			action = ACT_Forward25Frames;
		}
		break;
		
		// Delete A-B section
	case GDK_Delete: case GDK_KP_Delete:
		action = ACT_Delete;
		break;
		
		// Go to first frame
	case GDK_Home: case GDK_KP_Home:
		//case GDK_Page_Up:  case GDK_KP_Page_Up:
		action = ACT_Begin;
		break;

		// Go to last frame
	case GDK_End: case GDK_KP_End:
		//case GDK_Page_Down: case GDK_KP_Page_Down:
		action = ACT_End;
		break;

		// Set A marker
	case GDK_bracketleft:
		action = ACT_MarkA;
		break;

		// Set B marker
		case GDK_bracketright:
	action = ACT_MarkB;
		break;

		default:
			printf("Key : %x not handled\n",event->keyval);
		return;
		break;
	}

	HandleAction(action);
}
