//
// C++ Interface: GUI_sdlDraw
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
class sdlAccelRender: public AccelRender
{
    protected:
                int     useYV12;
                uint8_t *decoded;
	public:
				sdlAccelRender( void ) ;
		virtual	uint8_t init( GtkWidget * window, uint32_t w, uint32_t h);
		virtual	uint8_t end(void);				
		virtual uint8_t display(uint8_t *ptr, uint32_t w, uint32_t h);
};

