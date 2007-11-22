SET(ADM_LIB ADM_libass)

IF (USE_FREETYPE)
	SET(${ADM_LIB}_SRCS 
		ass_bitmap.c  ass.c  ass_cache.c  ass_fontconfig.c  ass_library.c   ass_render.c  ass_utils.c  mputils.c
		ass_font.c)

	ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} ${FREETYPE2_CFLAGS})

	IF (USE_FONTCONFIG)
		ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} ${FONTCONFIG_CFLAGS})
	ENDIF (USE_FONTCONFIG)
ENDIF (USE_FREETYPE)