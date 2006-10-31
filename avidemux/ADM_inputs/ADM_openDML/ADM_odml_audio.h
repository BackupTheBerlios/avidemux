#ifndef ADM_ODML_AUDIO_H
#define ADM_ODML_AUDIO_H

class odmlIndex;

class AVDMAviAudioStream : public AVDMGenericAudioStream
{
  protected:
    		
    uint32_t 		_nb_chunks;
    uint64_t		_abs_position;
    uint32_t		_rel_position;
    uint32_t 		_current_index;		
    odmlIndex 		*_index;
    uint32_t		_extraLen;
    uint8_t			*_extraData;
    FILE			*_fd;
		
  public:
    virtual ~AVDMAviAudioStream();
    AVDMAviAudioStream(		odmlIndex *idx,
                                uint32_t nbchunk,
                                const char  *name,
                                WAVHeader * wav, 
                                uint32_t preload,
                                uint32_t extraLen,
                                uint8_t  *extraData);
    virtual uint32_t 		read(uint32_t len,uint8_t *buffer);
    virtual uint8_t  		goTo(uint32_t newoffset);
    virtual	uint8_t			extraData(uint32_t *l,uint8_t **d)
    {
      *l=_extraLen;
      *d=_extraData;
      return 1;
    }
    virtual	uint8_t				getPacket(uint8_t *dest, uint32_t *len, 
        uint32_t *samples);

};



#endif
