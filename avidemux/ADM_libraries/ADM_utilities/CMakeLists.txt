SET(ADM_LIB ADM_utilities)

SET(${ADM_LIB}_SRCS 
	ADM_bitstream.cpp  ADM_fileio.cpp  ADM_gettext.cpp   ADM_imageLoader.cpp  
	avidemutils.cpp  aviindexer.cpp  fourcc.cpp  prefs.cpp)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_SOURCE_CFLAGS(prefs.cpp -I${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS})
