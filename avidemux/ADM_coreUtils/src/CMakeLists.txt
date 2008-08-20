SET(ADM_coreUtils_SRCS 
ADM_bitstream.cpp 
ADM_fileio.cpp 
ADM_gettext.cpp  
ADM_imageLoader.cpp  
avidemutils.cpp  
prefs.cpp)

ADD_LIBRARY(ADM_coreUtils SHARED ${ADM_coreUtils_SRCS})
ADD_SOURCE_CFLAGS(prefs.cpp -I${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS})
INSTALL(TARGETS ADM_coreUtils RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
