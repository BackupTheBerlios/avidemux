################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libass)

if(USE_FREETYPE)
	SET(${ADM_LIB}_SRCS 
	ass_bitmap.c  ass.c  ass_cache.c  ass_fontconfig.c  ass_library.c   ass_render.c  ass_utils.c  mputils.c
	ass_font.c
	)
	ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
	ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
	
	include_directories(${FREETYPE2_INCLUDE_DIRS})
endif(USE_FREETYPE)

################################