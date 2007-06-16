
SET(ADM_flv_SRCS 
ADM_flv.cpp
ADM_flvIndex.cpp
ADM_flvAudio.cpp
)
ADD_LIBRARY(ADM_flv STATIC ${ADM_flv_SRCS})
include_directories(..)
