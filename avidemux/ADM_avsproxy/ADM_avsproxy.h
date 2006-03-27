/*

(c) Mean 2006
*/

#ifndef AVS_PROXY_H

#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"


class avsHeader         :public vidHeader
{
    protected:
        int         mySocket;
        uint8_t     sendData(uint32_t cmd,uint32_t frame, uint32_t payload_size,uint8_t *payload);
        uint8_t     receiveData(uint32_t *cmd, uint32_t *frame,uint32_t *payload_size,uint8_t *payload);
        uint8_t     askFor(uint32_t cmd,uint32_t frame, uint32_t payloadsize,uint8_t *payload);
    public:


        virtual   void 				Dump(void) {};
        virtual   uint32_t 			getNbStream(void) { return 1;};
        virtual   uint8_t 			needDecompress(void) { return 0;};

        avsHeader( void );
        ~avsHeader(  );
// AVI io
        virtual 	uint8_t			open(char *name);
        virtual 	uint8_t			close(void) ;
  //__________________________
  //				 Info
  //__________________________

  //__________________________
  //				 Audio
  //__________________________

        virtual 	WAVHeader *getAudioInfo(void ) { return NULL ;} ;
        virtual 	uint8_t			getAudioStream(AVDMGenericAudioStream **audio)
        {  *audio=NULL;return 0;};

// Frames
  //__________________________
  //				 video
  //__________________________

        virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags) {return 1;}
        virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags) 
                            {*flags=AVI_KEY_FRAME;return AVI_KEY_FRAME;}
        virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                uint32_t *flags)
        {
            if(flags) *flags=AVI_KEY_FRAME;
            return getFrameNoAlloc(framenum,ptr,framelen);
            
        };
        virtual 	uint8_t  	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)	;

};
#endif
//EOF
