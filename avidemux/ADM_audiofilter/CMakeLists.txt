SET(ADMaudiofilter_SRCS 
	audiodeng_buildfilters.cpp  audioencoder_lavcodec.cpp  audioeng_process.cpp           audiofilter_film2pal.cpp   audiolib_sox.cpp
	audioencoder_aften.cpp      audioencoder_pcm.cpp       audiofilter_bridge.cpp         audiofilter_limiter.cpp    audio_raw.cpp
	audioencoder.cpp            audioencoder_twolame.cpp   audiofilter_buildchain.cpp     audiofilter_mixer.cpp
	audioencoder_faac.cpp       audioencoder_vorbis.cpp    audiofilter_channel_route.cpp  audiofilter_normalize.cpp
	audioencoder_lame.cpp       audioeng_buff.cpp          audiofilter_dolby.cpp          audiofilter_sox.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(ADM_audiofilter ${ADMaudiofilter_SRCS})
