
SET(ADM_asf_SRCS 
ADM_asf_audio.cpp  ADM_asfChunk.cpp  ADM_asf.cpp  ADM_asfPacket.cpp)
ADD_LIBRARY(ADM_asf STATIC ${ADM_asf_SRCS})
