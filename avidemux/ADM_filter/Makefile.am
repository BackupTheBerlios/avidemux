####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_filter.a

## INCLUDES were found outside kdevelop specific part

libADM_filter_a_METASOURCES = AUTO

libADM_filter_a_SOURCES = vidVCD.cpp  filter_declaration.cpp filter.cpp  filter_saveload.cpp 


EXTRA_DIST = filter.cpp             \
filter_saveload.cpp    vidVCD.cpp  video_filters.h \
filter_declaration.cpp  


####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes) -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil \
		   $(XML_CFLAGS)
noinst_HEADERS =  vidVCD.h
