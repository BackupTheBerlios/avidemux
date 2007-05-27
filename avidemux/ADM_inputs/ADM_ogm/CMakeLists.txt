
SET(ADM_ogm_SRCS 
ADM_ogmAudio.cpp  ADM_ogm.cpp  ADM_ogmpages.cpp  ADM_ogmreorder.cpp)
ADD_LIBRARY(ADM_ogm STATIC ${ADM_ogm_SRCS})
include_directories(..)
