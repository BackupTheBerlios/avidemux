
SET(ADM_oplug_mpeg_SRCS 
op_mpegpass.cpp
)
ADD_LIBRARY(oplug_mpeg STATIC ${ADM_oplug_mpeg_SRCS})
include_directories(../../ADM_inputs)
