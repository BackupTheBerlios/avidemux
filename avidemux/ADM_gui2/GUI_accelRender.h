//
// C++ Interface: GUI_accelRender
//
// Description: 
//  Class to hold all accelerated display (xv, sdl....)
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
class AccelRender
{
	public:
		AccelRender::AccelRender( void);
		virtual	uint8_t init(GtkWidget * window, uint32_t w, uint32_t h)=0;
		virtual	uint8_t end(void)=0;
		virtual uint8_t display(uint8_t *ptr, uint32_t w, uint32_t h)=0;
		
};
