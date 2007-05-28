################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libass)
#
if(FT_FOUND)
SET(${ADM_LIB}_SRCS 
ass_bitmap.c  ass.c  ass_cache.c  ass_fontconfig.c  ass_library.c  ass_mp.c  ass_render.c  ass_utils.c  mputils.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
endif(FT_FOUND)

################################
ADD_definitions(${FT_CFLAGS} )
