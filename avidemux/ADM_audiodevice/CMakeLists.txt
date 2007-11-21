SET(ADM_audiodevice_SRCS 
	ADM_audiodevice.cpp  ADM_deviceArts.cpp       ADM_deviceEsd.cpp  ADM_deviceSDL.cpp    oss_out.cpp
	ADM_deviceALSA.cpp   ADM_deviceAudioCore.cpp  ADM_deviceoss.cpp  ADM_deviceWin32.cpp  ADM_deviceJack.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_audiodevice ${ADM_audiodevice_SRCS})

SDLify(ADM_deviceSDL.cpp)

IF (USE_ARTS)
  ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "${ARTS_CFLAGS}")
ENDIF (USE_ARTS)