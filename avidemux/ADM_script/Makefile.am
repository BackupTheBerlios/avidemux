####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_script.a


libADM_script_a_METASOURCES = AUTO

libADM_script_a_SOURCES =   \
		 ADM_JSAvidemux.cpp ADM_JSAvidemux.h ADM_JSGlobal.cpp \
		ADM_JSGlobal.h ADM_Avidemux.cpp ADM_AvidemuxAudio.cpp ADM_JSAvidemuxAudio.cpp \
	ADM_AvidemuxVideo.cpp ADM_JSAvidemuxVideo.cpp ADM_JSFunctions.cpp \
	DirectorySearch.cpp ADM_JSDirectorySearch.cpp


EXTRA_DIST =  ADM_Avidemux.cpp       ADM_JSAvidemux.h           ADM_JSFunctions.cpp    DirectorySearch.h \
ADM_Avidemux.h         ADM_JSAvidemuxAudio.cpp    ADM_JSGlobal.cpp       StdFile.cpp \
ADM_AvidemuxAudio.cpp  ADM_JSAvidemuxAudio.h      ADM_JSGlobal.h         StdFile.h \
ADM_AvidemuxAudio.h    ADM_JSAvidemuxVideo.cpp    ADM_SharedLibrary.cpp  adm_scanner.h \
ADM_AvidemuxVideo.cpp  ADM_JSAvidemuxVideo.h      ADM_SharedLibrary.h \
ADM_AvidemuxVideo.h    ADM_JSDirectorySearch.cpp  ADM_container.h \
ADM_JSAvidemux.cpp     ADM_JSDirectorySearch.h    DirectorySearch.cpp


####### kdevelop will overwrite this part!!! (end)############
noinst_HEADERS = ADM_JSGlobal.h ADM_Avidemux.h \
		ADM_AvidemuxAudio.h ADM_JSAvidemuxAudio.h ADM_AvidemuxVideo.h ADM_JSAvidemuxVideo.h




INCLUDES = -I$(top_srcdir)/avidemux $(SMONKEY_CFLAGS) -DJS_THREADSAFE

