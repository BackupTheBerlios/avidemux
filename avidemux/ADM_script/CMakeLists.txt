SET(ADM_script_SRCS
	ADM_AvidemuxAudio.cpp  ADM_AvidemuxVideo.cpp    ADM_JSAvidemux.cpp       ADM_JSDirectorySearch.cpp  ADM_JSGlobal.cpp
	ADM_Avidemux.cpp       ADM_JSAvidemuxAudio.cpp  ADM_JSAvidemuxVideo.cpp  ADM_JSFunctions.cpp        DirectorySearch.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_script ${ADM_script_SRCS})
ADD_CFLAGS_ALL_TARGETS(ADM_script "-DJS_THREADSAFE -DXP_UNIX")