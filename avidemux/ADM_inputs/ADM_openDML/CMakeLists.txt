
SET(ADM_openDML_SRCS 
ADM_odml_audio.cpp  ADM_odml_regular.cpp  ADM_openDMLDepack.cpp    ADM_opendml_reorder.cpp
ADM_odml_odml.cpp   ADM_openDML.cpp       ADM_opendml_reindex.cpp  ADM_riff.cpp)
ADD_LIBRARY(ADM_openDML STATIC ${ADM_openDML_SRCS})
include_directories(..)
include_directories(../../ADM_userInterfaces/ADM_commonUI)

