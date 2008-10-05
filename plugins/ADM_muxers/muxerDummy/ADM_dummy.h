/**
 *
 *
 *
 *
 */
#ifndef MUXERDUMMY_H
#define MUXERDUMMY_H
class muxerDummy : public ADM_muxer
{
protected:

public:
                muxerDummy() {};
        virtual ~muxerDummy() {};
        virtual bool open(const char *file, ADM_videoStream *s,uint32_t nbAudioTrack n,ADM_audioStream *a);
        virtual bool save(void) ;
        virtual bool close(void) ;

};
#endif

