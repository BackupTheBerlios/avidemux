include_directories(../../)

SET(ADM_dialogFactoryNone_SRCS 
DIA_dialogFactory.cpp  FAC_toggle.cpp
)
ADD_LIBRARY(ADM_dialogFactoryNone STATIC ${ADM_dialogFactoryNone_SRCS})
