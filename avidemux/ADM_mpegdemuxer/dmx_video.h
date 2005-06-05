
#ifndef DMX_HEADER
#define DMX_HEADER

#include "ADM_mpegdemuxer/dmx_io.h"
#include "ADM_mpegdemuxer/dmx_demuxer.h"
#include "ADM_mpegdemuxer/dmx_audio.h"

typedef struct dmxIndex
{
        uint32_t type;
        uint32_t size;        
        uint32_t relative;        // position toward the current packet (small)
        uint64_t absolute;        // absolute position of the packetin the file (big)
        
}dmxIndex;


 class dmxHeader : public vidHeader
{
protected:
        uint8_t                 asyncJump(uint32_t frame)       ;
        uint8_t                 getFrameMpeg(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                                                                                uint32_t *flags);
        uint32_t                _nbFrames;
        uint32_t                _nbGop;

        dmxIndex                *_index;

        uint32_t                _lastFrame;
        uint8_t                 renumber(void);
        dmxAudioStream          *_audioStream;
        uint8_t                 _fieldEncoded;
public:

                                dmxHeader(void);
                                ~dmxHeader();
        virtual  void           Dump (void );


        virtual   uint32_t                      getNbStream(void);
        virtual   uint8_t                       needDecompress(void);

// AVI io
        virtual         uint8_t                 open(char *name);
        virtual         uint8_t                 close(void);
  //__________________________
  //                             Info
  //__________________________

  //__________________________
  //                             Audio
  //__________________________

        virtual         WAVHeader       *getAudioInfo(void )  ;
        virtual         uint8_t          getAudioStream(AVDMGenericAudioStream **audio);

// Frames
  //__________________________
  //                             video
  //__________________________
                virtual         uint8_t  getExtraHeaderData(uint32_t *len, uint8_t **data);
                virtual         uint8_t  setFlag(uint32_t frame,uint32_t flags);
                virtual         uint8_t  getFrameSize(uint32_t frame,uint32_t *size);
                virtual         uint32_t getFlags(uint32_t frame,uint32_t *flags);
                virtual         uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                                                                                uint32_t *flags);
                virtual         uint8_t         getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)      ;




                virtual         uint8_t            getRaw(uint32_t framenum,uint8_t *ptr,uint32_t* framelen);
                virtual         uint8_t            getRawStart(uint8_t *ptr,uint32_t *len);

private:

                        uint8_t            renumber(uint32_t nb);
                        dmx_demuxer        *demuxer;                    
                       

                        uint8_t            *_extraData;
                        uint32_t           _extraDataLen;

public:
                        uint8_t                                 reorder( void ) {return 1;}
                        uint8_t                                 isReordered( void ) { return 1;}

};
#endif
