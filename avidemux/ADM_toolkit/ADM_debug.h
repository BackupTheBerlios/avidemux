/***************************************************************************
                        Debug #define, ugly actually
                             -------------------
    begin                : Fri Apr 12 2002
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
 #if !defined(MODULE_NAME)
         #error define MODULE_NAME when using aprintf !
#endif

#if !defined(ADM_DEBUG)
         #define aprintf(prt, args...) ;
#else
        #if (MODULE_NAME !=0x8000)
	 	void indirect_printf(int entity, const char *fmt, ...);
                #define aprintf(prt, ...) indirect_printf(MODULE_NAME,prt, ## __VA_ARGS__)
        #else
                #define aprintf(prt, ...) ;
        #endif
#endif

