include(dm_plugin)
SET(ADM_ps_SRCS 
	ADM_ps.cpp
	ADM_psPlugin.cpp
	ADM_psIndex.cpp
	ADM_psReadIndex.cpp
	ADM_psAudio.cpp
	ADM_psAudioProbe.cpp
	ADM_psAudioProbe.h

    dmxPSPacket.cpp
    dmx_io.cpp
    ADM_indexFile.cpp
)

ADD_LIBRARY(ADM_dm_ps SHARED ${ADM_ps_SRCS})



INIT_DEMUXER(ADM_dm_ps)
INSTALL_DEMUXER(ADM_dm_ps)
