####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_filter.a

## INCLUDES were found outside kdevelop specific part

libADM_filter_a_METASOURCES = AUTO

libADM_filter_a_SOURCES = vidVCD.cpp gui_filter.cpp filter_declaration.cpp filter.cpp gui_filterlist.cpp gui_filtersub.cpp filter_saveload.cpp \
gui_filtermanager.cpp gui_filtermanager_dialog.cpp


EXTRA_DIST = filter.cpp             \
filter_saveload.cpp  gui_filterlist.cpp  vidVCD.cpp  video_filters.h \
filter_declaration.cpp  gui_filter.cpp       gui_filtersub.cpp   vidVCD.h


####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes) -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil \
		 $(GTK_CFLAGS)  $(XML_CFLAGS) -I../ADM_userInterfaces/ADM_GTK
noinst_HEADERS =  vidVCD.h
