INCLUDE(admCheckFontConfig)
checkFontConfig()

SET(ADM_LIB ADM_libass)

SET(${ADM_LIB}_SRCS 
ass_bitmap.c  ass.c  ass_cache.c  ass_fontconfig.c  ass_library.c   ass_render.c  ass_utils.c  mputils.c
ass_font.c)

ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_DEFINITIONS(${FREETYPE2_CFLAGS} "-I${LIBICONV_INCLUDE_DIR}")

IF (FONTCONFIG_FOUND)
	ADD_DEFINITIONS(${FONTCONFIG_CFLAGS} "-DHAVE_FONTCONFIG=1")
ENDIF (FONTCONFIG_FOUND)

IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC)
ENDIF (UNIX)