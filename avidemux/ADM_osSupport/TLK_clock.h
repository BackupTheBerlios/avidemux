//
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
