#ifndef ADM_AUDIO_MIXER_H
#define ADM_AUDIO_MIXER_H
class AVDMProcessAudio_Mixer : public AVDMBufferedAudioStream
{
    protected:
#define MIXER_BUFFER_SIZE 1024*10*2
        CHANNEL_CONF    _output;
        CHANNEL_CONF    _input;
        uint8_t        mixBuffer[MIXER_BUFFER_SIZE];
    public:
						
        ~AVDMProcessAudio_Mixer();
        AVDMProcessAudio_Mixer(AVDMGenericAudioStream *instream,CHANNEL_CONF out);
        virtual uint32_t grab(uint8_t *obuffer);
};
#endif
