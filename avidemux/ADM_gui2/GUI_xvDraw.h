class XvAccelRender: public AccelRender
{
	public:
				XvAccelRender( void ) ;
		virtual	uint8_t init( GtkWidget * window, uint32_t w, uint32_t h);
		virtual	uint8_t end(void);				
		virtual uint8_t display(uint8_t *ptr, uint32_t w, uint32_t h);
};



