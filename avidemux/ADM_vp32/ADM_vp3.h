/***************************************************************************
                          ADM_vp3.h  -  description
                             -------------------
	Reuse of Lutz Sammer VP32 decoder port to linux                            
                             
    begin                : Sat Sep 28 2002
    copyright            : (C) 2002 by mean
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

#define POSTPROCESS
 extern "C" 
 {
	      #include "ADM_vp32/include/vfw_PB_Interface.h"
			#include "ADM_vp32/include/pbdll.h"	 
	}
#undef POSTPROCESS
