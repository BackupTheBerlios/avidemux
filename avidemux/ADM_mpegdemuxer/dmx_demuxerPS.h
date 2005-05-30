#ifndef DMX_DMX_PS
#define DMX_DMX_PS

 
#include "dmx_demuxer.h"

#define MAX_PES_BUFFER (65*1024) // should be safe enough
#define MAX_PES_STREAM 50       // should be enough too :)

#define PICTURE_START_CODE 0x00L
#define SLICE_MIN_START    0x01L
#define SLICE_MAX_START    0xAFL
#define SEQ_START_CODE     0xB3L
#define EXT_START_CODE     0xB5L
#define SEQ_END_CODE       0xB7L
#define GOP_START_CODE     0xB8L
#define ISO_END_CODE       0xB9L
#define PACK_START_CODE    0xBAL

#define USER_DATA_START_CODE    0xb2
#define SYSTEM_START_CODE       0xbb
#define PADDING_CODE						0xbe
#define PRIVATE_STREAM_1        0xbd
#define PRIVATE_STREAM_2        0xbf
#define SYSTEM_END_CODE         0xb9
/*
        A bit of explanation here.
        The demuxer will take each packet and lookup what it is
        If it is the seeked pes,it will copy its content into _pesBuffer
        If not it will keep statical info and continue

        The pid is like that :
                For simple pes-pid it is the pes_pid (0xC0, 0xE0,...)
                For data stored in private stream 1 (LPCM, AC3,...), the value is 0xFF00 + sub stream id
                        with sub stream id : 0-8 for AC3, A0-A8 for LPCM, 


*/
class dmx_demuxerPS: public dmx_demuxer
 {
          protected : 
                  uint64_t      stampAbs;
                  uint32_t      consumed;
                  fileParser    *parser;
                  uint32_t       myPid;           // pid: high part =0xff if private stream, 00 if not
                  
                  uint8_t       *_pesBuffer;

                  uint32_t      _pesBufferIndex; // current position in pesBuffer

                  uint64_t      _pesBufferStart;
                  uint32_t      _pesBufferLen;
                  uint64_t      _pesPTS;
                  uint64_t      _pesDTS;
                

                  uint64_t      _oldPesStart;    // Contains info for previous packet of same pid
                  uint32_t      _oldPesLen;      // useful when need to go back (after video startcode)
                  uint64_t      _oldPTS;
                  uint64_t      _oldDTS;
                  uint32_t      _probeSize;      // If not nul, we will only seek to this
                  uint32_t      maxPid;

                  uint64_t      seen[256];                                  
                  uint8_t       mask[256];       
                
                  uint8_t       refill(void);
                  uint8_t       getPacketInfo(uint8_t stream,uint8_t *substream,uint32_t *len,uint64_t *pts,uint64_t *dts);

                  uint8_t	getStat(uint64_t *oseen) {oseen=seen;return 1;}
          public:
                           dmx_demuxerPS(uint32_t nb,MPEG_TRACK *tracks) ;
                virtual    ~dmx_demuxerPS();             
                
                     uint8_t      open(char *name);
                 
                
                  uint8_t         forward(uint32_t f);
                  uint8_t         stamp(void); 

                  uint64_t        elapsed(void);
                
                  uint8_t         getPos( uint64_t *abs,uint64_t *rel);
                  uint8_t         setPos( uint64_t abs,uint64_t  rel);
                
                  uint64_t        getSize( void) { return _size;}          
                  uint8_t         setProbeSize(uint32_t probe);                
                
                  uint32_t        read(uint8_t *w,uint32_t len);
                  uint8_t         sync( uint8_t *stream,uint64_t *abs,uint64_t *r,uint64_t *pts, uint64_t *dts);

                  uint8_t         hasAudio(void) { return 0;}


// Inlined
uint8_t         read8i(void)
{
        uint8_t r;
        
        if(_pesBufferIndex+1<=_pesBufferLen)
        {
                consumed++;
                r=_pesBuffer[_pesBufferIndex++];
                return r;
        }

        read(&r,1);
        return r;
}
uint16_t         dmx_demuxerPS::read16i(void)
{
 uint16_t r;
uint8_t p[2];
        
        if(_pesBufferIndex+2<=_pesBufferLen)
        {
                consumed+=2;
                r=((_pesBuffer[_pesBufferIndex])<<8)+(_pesBuffer[_pesBufferIndex+1]);
                _pesBufferIndex+=2;
                return r;
        }

        read(p,2);
        r=(p[0]<<8)+p[1];
        return r;
}
uint32_t         dmx_demuxerPS::read32i(void)                 
{
uint32_t r;
uint8_t  *p;
uint8_t b[4];        
        if(_pesBufferIndex+4<=_pesBufferLen)
        {
                consumed+=4;
                p=&(_pesBuffer[_pesBufferIndex]);
                _pesBufferIndex+=4;
        }
        else
        {
                read(b,4);
                p=b;
        }
        r=(p[0]<<24)+(p[1]<<16)+(p[2]<<8)+(p[3]);
        return r;
}
                
                  
};      
        

#endif
