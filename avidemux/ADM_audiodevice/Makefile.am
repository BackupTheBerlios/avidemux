####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_audiodevice.a

## INCLUDES were found outside kdevelop specific part

libADM_audiodevice_a_METASOURCES = AUTO

libADM_audiodevice_a_SOURCES = ADM_deviceArts.cpp ADM_audiodevice.cpp \
	ADM_deviceoss.cpp ADM_deviceALSA.cpp ADM_deviceALSA.h ADM_deviceAudioCore.cpp \
	ADM_deviceSDL.cpp ADM_deviceWin32.cpp ADM_deviceEsd.cpp


EXTRA_DIST = ADM_audiodevice.cpp  ADM_deviceArts.cpp       \
ADM_deviceEsd.cpp  ADM_deviceWin32.cpp  audio_out.h \
ADM_audiodevice.h    ADM_deviceArts.h         ADM_deviceEsd.h    ADM_deviceWin32.h    oss_out.cpp \
ADM_deviceALSA.cpp   ADM_deviceAudioCore.cpp  ADM_deviceSDL.cpp  ADM_deviceoss.cpp \
ADM_deviceALSA.h     ADM_deviceAudioCore.h    ADM_deviceSDL.h    ADM_deviceoss.h

####### kdevelop will overwrite this part!!! (end)############

INCLUDES = $(all_includes) -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil \
		 $(ARTS_CFLAGS)
noinst_HEADERS = ADM_audiodevice.h ADM_deviceSDL.h ADM_deviceWin32.h
