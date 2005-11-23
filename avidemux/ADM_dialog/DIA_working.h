/***************************************************************************
                          DIA_working.cpp  -  description
                             -------------------
    begin                : Thu Apr 21 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr

	This class deals with the working window

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __DIA_WK__
#define __DIA_WK__
#include "ADM_toolkit/TLK_clock.h"
	class DIA_working
	{
		private :
				uint32_t 	lastper;
				Clock	_clock;
				uint32_t	_nextUpdate;
				uint32_t 	elapsed;
				void 		postCtor( void );
		public:
				void 		*_priv;
						DIA_working( void );
						DIA_working( const char *title );
						~DIA_working();
				// If returns 1 -> Means aborted
				uint8_t  	update(uint32_t percent);
				uint8_t 	update(uint32_t current,uint32_t total);
				uint8_t  	isAlive (void );
				void 		closeDialog(void);
	};


#endif
