/**
 *      \file ADM_muxer.h
 *      \brief External interface for muxers
 *      (c) Mean 2008
 *
 */
#ifndef ADM_muxer_H
#define ADM_muxer_H
#include "ADM_default.h"
#include "ADM_audioStream.h"
/**
    \class ADM_videoStream

*/
class ADM_videoStream
{
protected:
            uint32_t width,height,averageFps1000;
            uint32_t fourCC;
            bool     isCFR;
public:
                      ADM_videoStream() {};
            virtual ~ADM_videoStream() {};
            uint32_t getWidth(void) {return width;}
            uint32_t getHeight(void) {return height;}
            uint32_t getFCC(void) {return fourCC;}
            bool     getIsCfr(void) {return isCFR;}
            uint32_t getAvgFps1000(void) {return averageFps1000;}

virtual     bool     getPacket(uint32_t *len, uint8_t *data, uint32_t maxLen,uint64_t *pts,uint64_t *dts)=0;
virtual     bool     getExtraData(uint32_t *extraLen, uint8_t **extraData) {*extraLen=0;*extraData=NULL;return true;};
};
/**
 *      \class ADM_muxer
 *
 */
class ADM_muxer
{
public:
                          ADM_muxer() {};
        virtual           ~ADM_muxer() {};
        virtual bool      open(const char *filename,   ADM_videoStream *videoStream,
                                uint32_t nbAudioTrack, ADM_audioStream **audioStreams)=0;

        virtual  bool     save(void)=0;
        virtual  bool     close(void)=0; 
        virtual  bool     configure(void) {return true;};


};
#endif

