
SET(ADM_codecs_SRCS 
ADM_codecs.cpp  ADM_ffmpeg.cpp  ADM_mjpegEncode.cpp  ADM_png.cpp    ADM_theora_dec.cpp  ADM_x264.cpp   ADM_xvid.cpp     ADM_xvideco.cpp
ADM_ffmp43.cpp  ADM_mjpeg.cpp   ADM_mpeg.cpp         ADM_rgb16.cpp  ADM_uyvy.cpp        ADM_xvid4.cpp  ADM_xvidcvs.cpp)
ADD_LIBRARY(ADM_codecs STATIC ${ADM_codecs_SRCS})
