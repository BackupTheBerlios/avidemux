/**
    dmxPSPacket.cpp

*/

#ifndef DMXPSPACKET_H
#define DMXPSPACKET_H

#include "dmx_io.h"
#include "ADM_Video.h"
/**
    \class psPacket
*/
class psPacket
{
protected:
    fileParser  *_file;
    uint64_t    _size;
    uint8_t     getPacketInfo(uint8_t stream,uint8_t *substream,uint32_t *olen,uint64_t *opts,uint64_t *odts);
public:
                psPacket(void);
                ~psPacket();
    bool        open(const char *filenames,bool dontappend);
    bool        close(void);
    bool        getPacket(uint32_t maxSize, uint8_t *pid, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer);
    bool        getPacketOfType(uint8_t pid,uint32_t maxSize, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer);
    uint64_t    getPos(void);
    bool        setPos(uint64_t pos);
};


#endif