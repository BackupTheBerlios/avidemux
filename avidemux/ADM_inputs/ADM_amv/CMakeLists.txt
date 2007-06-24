
SET(ADM_amv_SRCS 
ADM_amv.cpp
ADM_amvAudio.cpp
)
ADD_LIBRARY(ADM_amv STATIC ${ADM_amv_SRCS})
include_directories(..)
