########################################
# FontConfig
########################################
OPTION(FONTCONFIG "" ON)

MESSAGE(STATUS "Checking for FontConfig")
MESSAGE(STATUS "***********************")

IF (FONTCONFIG)
	PKG_CHECK_MODULES(FONTCONFIG fontconfig)
	PRINT_LIBRARY_INFO("FontConfig" FONTCONFIG_FOUND "${FONTCONFIG_CFLAGS}" "${FONTCONFIG_LDFLAGS}")

	IF (FONTCONFIG_FOUND)
		SET(HAVE_FONTCONFIG 1)
		SET(USE_FONTCONFIG 1)
	ENDIF (FONTCONFIG_FOUND)
ELSE (FONTCONFIG)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (FONTCONFIG)

MESSAGE("")

########################################
# FreeType2
########################################
OPTION(FREETYPE2 "" ON)

MESSAGE(STATUS "Checking for FreeType2")
MESSAGE(STATUS "**********************")

IF (FREETYPE2)
	PKG_CHECK_MODULES(FREETYPE2 freetype2)
	PRINT_LIBRARY_INFO("FreeType2" FREETYPE2_FOUND "${FREETYPE2_CFLAGS}" "${FREETYPE2_LDFLAGS}")

	IF (FREETYPE2_FOUND)
		SET(USE_FREETYPE 1)
	ENDIF (FREETYPE2_FOUND)
ELSE (FREETYPE2)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (FREETYPE2)

MESSAGE("")

########################################
# libiconv
########################################
IF (USE_FREETYPE)
	MESSAGE(STATUS "Checking for libiconv")
	MESSAGE(STATUS "*********************")

	FIND_HEADER_AND_LIB(LIBICONV iconv.h iconv)
	PRINT_LIBRARY_INFO("libiconv" LIBICONV_FOUND "${LIBICONV_INCLUDE_DIR}" "${LIBICONV_LIBRARY_DIR}")

	IF (NOT LIBICONV_INCLUDE_DIR)
		MESSAGE(STATUS "iconv.h not found, disabling FreeType2")
		SET(USE_FREETYPE)
	ELSE (NOT LIBICONV_INCLUDE_DIR)
		IF (LIBICONV_LIBRARY_DIR)
			MESSAGE(STATUS "libiconv is probably needed")

			CHECK_CFLAGS_REQUIRED(iconv_check.cpp "-DICONV_NEED_CONST" "${LIBICONV_INCLUDE_DIR}" "${LIBICONV_LIBRARY_DIR}" LIBICONV_CFLAGS_REQUIRED)
		ELSE (LIBICONV_LIBRARY_DIR)
			SET(LIBICONV_LIBRARY_DIR "")
			MESSAGE(STATUS "libiconv is probably not needed")

			ADM_COMPILE(iconv_check.cpp "-DICONV_NEED_CONST" "${LIBICONV_INCLUDE_DIR}" "" LIBICONV_CFLAGS_REQUIRED LIBICONV_COMPILE_OUTPUT)
		ENDIF (LIBICONV_LIBRARY_DIR)

		IF (LIBICONV_CFLAGS_REQUIRED)
			SET(LIBICONV_DEFINITIONS "-DICONV_NEED_CONST")
			SET(ICONV_NEED_CONST 1)
		ENDIF(LIBICONV_CFLAGS_REQUIRED)
	ENDIF (NOT LIBICONV_INCLUDE_DIR)

	MESSAGE("")
ENDIF (USE_FREETYPE)

########################################
# gettext
########################################
INCLUDE(admCheckGettext)

checkGettext()

SET(ADM_LOCALE "${CMAKE_INSTALL_PREFIX}/share/locale")

########################################
# SDL
########################################
OPTION(SDL "" ON)

MESSAGE(STATUS "Checking for SDL")
MESSAGE(STATUS "****************")

IF (SDL)
	FIND_PACKAGE(SDL)
	PRINT_LIBRARY_INFO("SDL" SDL_FOUND "${SDL_INCLUDE_DIR}" "${SDL_LIBRARY}")
	
	MARK_AS_ADVANCED(SDLMAIN_LIBRARY)
	MARK_AS_ADVANCED(SDL_INCLUDE_DIR)
	MARK_AS_ADVANCED(SDL_LIBRARY)

	IF (SDL_FOUND)
		SET(USE_SDL 1)
	ENDIF (SDL_FOUND)
ELSE (SDL)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (SDL)

MESSAGE("")

########################################
# XVideo
########################################
IF (UNIX AND NOT APPLE)
	OPTION(XVIDEO "" ON)

	IF (XVIDEO)
		MESSAGE(STATUS "Checking for XVideo")
		MESSAGE(STATUS "*******************")

		FIND_HEADER_AND_LIB(XVIDEO X11/extensions/Xvlib.h Xv XvShmPutImage)
		PRINT_LIBRARY_INFO("XVideo" XVIDEO_FOUND "${XVIDEO_INCLUDE_DIR}" "${XVIDEO_LIBRARY_DIR}")

		IF (XVIDEO_FOUND)
			SET(USE_XV 1)
		ENDIF (XVIDEO_FOUND)

		MESSAGE("")
	ENDIF (XVIDEO)
ELSE (UNIX AND NOT APPLE)
	SET(XVIDEO_CAPABLE FALSE)
ENDIF (UNIX AND NOT APPLE)
