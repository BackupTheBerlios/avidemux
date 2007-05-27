
SET(ADM_script_SRCS 
ADM_AvidemuxAudio.cpp  ADM_AvidemuxVideo.cpp    ADM_JSAvidemux.cpp       ADM_JSDirectorySearch.cpp  ADM_JSGlobal.cpp
ADM_Avidemux.cpp       ADM_JSAvidemuxAudio.cpp  ADM_JSAvidemuxVideo.cpp  ADM_JSFunctions.cpp        DirectorySearch.cpp
)
ADD_LIBRARY(ADM_script STATIC ${ADM_script_SRCS})
add_definitions(-DJS_THREADSAFE -DXP_UNIX)
INCLUDE_DIRECTORies(../ADM_libraries/ADM_smjs)