#######################################
include (admCheckAudioEncoderLibs)
include (admConfigSummary)
MESSAGE("***  Audio Codec  ***")
ADM_DISPLAY("Aften     " "${USE_AFTEN}")
ADM_DISPLAY("LAME      " "${USE_LAME}")
ADM_DISPLAY("FAAC      " "${USE_FAAC}")
ADM_DISPLAY("Vorbis    " "${USE_VORBIS}")
MESSAGE("***  /Audio Codec  ***")


if(USE_LAME)
ADD_SUBDIRECTORY(lame)
endif(USE_LAME)

