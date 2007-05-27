include_directories(../../ADM_commonUI)

SET(ADM_guiNone_SRCS 
file_none.cpp  gui_none.cpp  preview_none.cpp
)
ADD_LIBRARY(ADM_guiNone STATIC ${ADM_guiNone_SRCS})
