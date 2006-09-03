
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
                {"lame","Lame", AUDIOENC_MP3},
#endif
#ifdef USE_FAAC
                {"aac","FAAC", AUDIOENC_FAAC},
#endif
#ifdef USE_VORBIS
                {"vorbis","Vorbis", AUDIOENC_VORBIS},
#endif
#ifdef USE_AFTEN
                {"aften","AFTEN", AUDIOENC_AFTEN},
#endif
                {"mp2", "FFm MP2", AUDIOENC_MP2},
                {"ac3", "FFm AC3",AUDIOENC_AC3},
                {"twolame","Twolame", AUDIOENC_2LAME},
                {"none", "Wav PCM",AUDIOENC_NONE},
                {"lpcm", "Wav LPCM",AUDIOENC_LPCM}
};
#endif
