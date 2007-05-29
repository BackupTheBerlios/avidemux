
SET(ADM_oplug_ogm_SRCS 
op_ogaudio.cpp  op_ogpage.cpp  op_ogsavecopy.cpp  op_ogsave.cpp  op_ogsaveprocess.cpp
)
ADD_LIBRARY(oplug_ogm STATIC ${ADM_oplug_ogm_SRCS})
include_directories(../../ADM_inputs)

