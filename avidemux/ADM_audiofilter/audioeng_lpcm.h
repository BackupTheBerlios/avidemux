
#ifndef __ADM_LPCM_H
#define __ADM_LPCM_H

//_____________________________________________
class AVDMProcessAudio_Lpcm : public AVDMBufferedAudioStream
{
protected:
                  uint8_t       _bufferin[PROCESS_BUFFER_SIZE];
          virtual uint32_t      grab(uint8_t *obuffer);

                
public:
                                                
                                ~AVDMProcessAudio_Lpcm();                
                                AVDMProcessAudio_Lpcm(AVDMGenericAudioStream *instream  );              
                
};
#endif
