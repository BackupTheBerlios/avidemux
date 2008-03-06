/** *************************************************************************
    \fn ADM_clock.h
    \brief Handle time class
                      
    copyright            : (C) 2008 by mean
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __CLK__
#define __CLK__
class Clock
{
	private: uint32_t _startTime;

	public:
			Clock(void );
			~Clock( );
			uint32_t getElapsedMS( void );
			uint8_t reset( void );


};
#endif
