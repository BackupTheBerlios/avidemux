####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_lavformat.a


libADM_lavformat_a_METASOURCES = AUTO

libADM_lavformat_a_SOURCES =  mpeg.c utils.c avio.c ADM_lavformat.cpp \
aviobuf.c cutils.c img2.c file.c mpegtsenc.c ADM_muxts.cpp movenc.c \
avienc.c wav.c mov.c matroska.c


EXTRA_DIST = ADM_lavformat.cpp  ADM_muxts.cpp  avformat.h  avio.h  mpegts.h  rtp.h   rtspcodes.h \
ADM_lavformat.h    ADM_tscrc.h    avi.h       mov.h   riff.h    rtsp.h


####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes)  -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I../ADM_lavcodec -I.. -DHAVE_AV_CONFIG_H -I../ADM_lavutil
noinst_HEADERS =  
