/*

*/
#include "ADM_toolkit/TLK_clock.h"
#define MAX_BR_SLOT 200
class DIA_encoding
{
private:
		Clock	clock;
		uint32_t  _lastTime;
		uint32_t  _lastFrame;
		uint32_t _fps_average;
		uint64_t _totalSize;
		uint64_t _audioSize;
		uint64_t _videoSize;
		uint32_t _bitrate[MAX_BR_SLOT];
		uint32_t _roundup;
		uint32_t _current;
	
		void setBitrate(uint32_t br);
		void setSize(int size);
		void setAudioSize(int size);
		void setVideoSize(int size);
public:
		DIA_encoding( uint32_t fps1000 );
		~DIA_encoding( );
		
		void reset( void );
		void setPhasis(const char *n);
		void setCodec(const char *n);
		void setAudioCodec(const char *n);
		void setFps(uint32_t fps);
		void feedFrame(uint32_t br);		
		void feedAudioFrame(uint32_t br);		
		void setQuant(uint32_t quant);
		void setFrame(uint32_t nb,uint32_t total);
                void setContainer(const char *container);
		
 		uint8_t isAlive(void);
};
