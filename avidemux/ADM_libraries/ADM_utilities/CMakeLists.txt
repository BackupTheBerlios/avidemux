SET(ADM_LIB ADM_utilities)

SET(${ADM_LIB}_SRCS 
	ADM_bitstream.cpp  ADM_image.cpp         ADM_imageUtils.cpp  ADM_print.cpp    aviframe.cpp    aviio.cpp   fourcc.cpp  subchunk.cpp
	ADM_fileio.cpp     ADM_imageResizer.cpp  ADM_pp.cpp          avidemutils.cpp  aviindexer.cpp  broken.cpp  prefs.cpp
	ADM_imageLoader.cpp  ADM_gettext.cpp)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})