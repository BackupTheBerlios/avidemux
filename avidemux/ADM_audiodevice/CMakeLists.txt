
SET(ADM_audiodevice_SRCS 
ADM_audiodevice.cpp  ADM_deviceArts.cpp       ADM_deviceEsd.cpp  ADM_deviceSDL.cpp    oss_out.cpp
ADM_deviceALSA.cpp   ADM_deviceAudioCore.cpp  ADM_deviceoss.cpp  ADM_deviceWin32.cpp)
ADD_LIBRARY(ADM_audiodevice STATIC ${ADM_audiodevice_SRCS})
