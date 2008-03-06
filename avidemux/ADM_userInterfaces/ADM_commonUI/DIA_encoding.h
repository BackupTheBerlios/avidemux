/*

*/
#ifndef ADM_ENCODING_H
#define ADM_ENCODING_H


#include "ADM_tray.h"
#define MAX_BR_SLOT 200

typedef struct 
{
  uint32_t size;
  uint32_t quant;
}encodingSlice;

class DIA_encoding
{
private:
                Clock	clock;
                uint32_t  _lastTime;            // Start time used to calc. ETA
                uint32_t  _lastFrame;           // Start frame used to calc. ETA
                uint32_t  _nextSampleStartTime; // Next start time to be used for ETA
                uint32_t  _nextSampleStartFrame; // Next start frame for ETA
                uint32_t  _nextUpdate;           // Next time to update the GUI
                float _fps_average;
                uint32_t _average_bitrate;
                uint64_t _totalSize;
                uint64_t _audioSize;
                uint64_t _videoSize;
                uint32_t _bitrate_sum;           // Sum of bitrate array
                encodingSlice _bitrate[MAX_BR_SLOT];
                uint32_t _roundup;
                uint32_t _current;
                uint32_t _total;
                uint32_t _lastnb;
                uint32_t _fps1000;
				uint32_t _originalPriority;
        
                void setBitrate(uint32_t br,uint32_t globalbr);
                void setSize(int size);
                void setAudioSizeIn(int size);
                void setVideoSizeIn(int size);
                void setQuantIn(int size);
                void updateUI(void);
                ADM_tray *tray;
public:
                DIA_encoding( uint32_t fps1000 );
                ~DIA_encoding( );
                
                void reset( void );
                void setPhasis(const char *n);
                void setCodec(const char *n);
                void setAudioCodec(const char *n);
                void setFps(uint32_t fps);
                void setFrame(uint32_t nb,uint32_t size, uint32_t quant,uint32_t total);
                void setContainer(const char *container);
                void setAudioSize(uint32_t size);
                uint8_t isAlive(void);
};

#endif
