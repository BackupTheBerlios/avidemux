
SET(ADM_audiocodec_SRCS ADM_8bits.cpp       ADM_codecac3.cpp   ADM_codecimaadpcm.cpp  ADM_codeculaw.cpp  ADM_codecwma.cpp  ADM_lpcm.cpp
ADM_audiocodec.cpp  ADM_codecfaad.cpp  ADM_codecmp3.cpp       ADM_codecwav.cpp   ADM_dca.cpp       audiocodec_ogg.cpp)
ADD_LIBRARY(ADM_audiocodec STATIC ${ADM_audiocodec_SRCS})
