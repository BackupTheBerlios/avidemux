
SET(ADM_encoder_SRCS 
adm_encConfig.cpp  adm_encdivx2p.cpp  adm_encffmpeg1.cpp  adm_encmjpeg.cpp     adm_encoder.cpp     adm_encx264.cpp   adm_encxvid.cpp
adm_encCopy.cpp    adm_encdivx.cpp    adm_encffmpeg.cpp   adm_encmpeg2enc.cpp  adm_encRequant.cpp  adm_encXvid4.cpp  adm_encyv12.cpp)
ADD_LIBRARY(ADM_encoder STATIC ${ADM_encoder_SRCS})
