################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_utilities)
#
SET(${ADM_LIB}_SRCS 
ADM_bitstream.cpp  ADM_image.cpp         ADM_imageUtils.cpp  ADM_print.cpp    aviframe.cpp    aviio.cpp   fourcc.cpp  subchunk.cpp
ADM_fileio.cpp     ADM_imageResizer.cpp  ADM_pp.cpp          avidemutils.cpp  aviindexer.cpp  broken.cpp  prefs.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)

include_directories(${LIBXML2_INCLUDE_DIR})

################################
