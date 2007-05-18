
#ifndef CODECA_LIST
#define CODECA_LIST
typedef struct {
        const char *name;
        const char *menuName;
        AUDIOENCODER codec;
}CODECLIST;

 const CODECLIST myCodecList[]=
{
                {"copy","Copy", AUDIOENC_COPY},
#ifdef HAVE_LIBMP3LAME
                {"lame","MP3 (Lame)", AUDIOENC_MP3},
#endif
#ifdef USE_FAAC
                {"aac","AAC (FAAC)", AUDIOENC_FAAC},
#endif
#ifdef USE_VORBIS
                {"vorbis","Vorbis", AUDIOENC_VORBIS},
#endif
#ifdef USE_AFTEN
                {"aften","AC3 (Aften)", AUDIOENC_AFTEN},
#endif
                {"twolame","MP2 (Twolame)", AUDIOENC_2LAME},
                {"mp2", "MP2 (lavc)", AUDIOENC_MP2},
                {"ac3", "AC3 (lavc)",AUDIOENC_AC3},
                {"none", "Wav PCM",AUDIOENC_NONE},
                {"lpcm", "Wav LPCM",AUDIOENC_LPCM}
};
#endif
