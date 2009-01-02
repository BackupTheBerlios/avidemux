/**
    dmxPSPacket.cpp

*/

#ifndef DMXPSPACKET_H
#define DMXPSPACKET_H

#include "dmx_io.h"
#include "ADM_Video.h"

/**
    \struct psPacketInfo

*/
typedef struct
{
    uint64_t pts;
    uint64_t dts;
    uint64_t startAt;
    uint32_t offset;

}psPacketInfo;

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
    bool        getPacket(uint32_t maxSize, uint8_t *pid, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer,uint64_t *startAt);
    bool        getPacketOfType(uint8_t pid,uint32_t maxSize, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer,uint64_t *startAt);
    uint64_t    getPos(void);
    bool        setPos(uint64_t pos);
};
/**
    \class psPacketLinear
*/
#define ADM_PACKET_LINEAR 10*1024

class psPacketLinear : public psPacket
{
protected:
        uint8_t  myPid;
        uint64_t startAt;
        uint32_t bufferLen;
        uint64_t bufferPts;
        uint64_t bufferDts;
        uint32_t bufferIndex;
        uint8_t  buffer[ADM_PACKET_LINEAR];
        bool     eof;
        bool     refill(void);
        uint64_t oldStartAt;
        uint32_t oldBufferLen;
        uint64_t oldBufferPts;
        uint64_t oldBufferDts;
        uint32_t consumed;

public:
                psPacketLinear(uint8_t pid);
                ~psPacketLinear();
        uint32_t getConsumed(void);
        uint8_t  readi8();
        uint16_t readi16();
        uint32_t readi32();
        bool     sync(uint8_t *pid);
        bool    read(uint32_t len, uint8_t *buffer);
        bool    forward(uint32_t v);
        bool    stillOk(void) {return !eof;};
        bool    getInfo(psPacketInfo *info);
        bool    seek(uint64_t packetStart, uint32_t offset);
};



#endif