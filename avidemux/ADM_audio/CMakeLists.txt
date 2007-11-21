SET(ADM_audio_SRCS ADM_a52info.cpp  ADM_dcainfo.cpp  audioac3ex.cpp  audiomp3ex.cpp   audio_packetizer.cpp  audiotimeline.cpp
	ADM_aacinfo.cpp  ADM_mp3info.cpp  audiogen.cpp    audio_mpegidentify.cpp  audiosource_ext.cpp   audiowavex.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(ADM_audio ${ADM_audio_SRCS})
