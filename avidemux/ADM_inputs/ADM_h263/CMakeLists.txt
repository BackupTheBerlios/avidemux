
SET(ADM_h263_SRCS 
ADM_h263.cpp  ADM_mp4.cpp  bitsRead.cpp)
ADD_LIBRARY(ADM_h263 STATIC ${ADM_h263_SRCS})
include_directories(..)
