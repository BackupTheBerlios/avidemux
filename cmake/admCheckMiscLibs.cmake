########################################
# FontConfig
########################################
MESSAGE(STATUS "Checking for FontConfig")
MESSAGE(STATUS "***********************")

FIND_HEADER_AND_LIB(FONTCONFIG fontconfig/fontconfig.h fontconfig FcStrSetCreate)
PRINT_LIBRARY_INFO("FontConfig" FONTCONFIG_FOUND "${FONTCONFIG_INCLUDE_DIR}" "${FONTCONFIG_LIBRARY_DIR}")

IF (FONTCONFIG_FOUND)
	SET(HAVE_FONTCONFIG 1)
	SET(USE_FONTCONFIG 1)
ENDIF (FONTCONFIG_FOUND)

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
	MESSAGE(STATUS "<disabled per request>")
ENDIF (FREETYPE2)

MESSAGE("")

########################################
# gettext
########################################
MESSAGE(STATUS "Checking for gettext")
MESSAGE(STATUS "********************")

OPTION(GETTEXT "" ON)

IF (GETTEXT)
	FIND_HEADER_AND_LIB(GETTEXT libintl.h intl)
	PRINT_LIBRARY_INFO("Gettext" GETTEXT_FOUND "${GETTEXT_INCLUDE_DIR}" "${GETTEXT_LIBRARY_DIR}")

	IF (GETTEXT_INCLUDE_DIR)
		# Try linking without -lintl
		ADM_COMPILE(gettext.cpp "" "-I${GETTEXT_INCLUDE_DIR}" "" LIBINTL_NOT_REQUIRED outputWithoutLibintl)

		IF (LIBINTL_NOT_REQUIRED)
			SET(GETTEXT_LIBRARY_DIR "")
			SET(HAVE_GETTEXT 1)

			MESSAGE(STATUS "libintl not required for gettext support")
		ELSE (LIBINTL_NOT_REQUIRED)
			ADM_COMPILE(gettext.cpp "" "-I${GETTEXT_INCLUDE_DIR}" "${GETTEXT_LIBRARY_DIR}" LIBINTL_REQUIRED outputWithLibintl)

			IF (LIBINTL_REQUIRED)
				SET(HAVE_GETTEXT 1)

				MESSAGE(STATUS "libintl required for gettext support")
			ELSE (LIBINTL_REQUIRED)
				SET(GETTEXT_FOUND 0 CACHE INTERNAL "")

				MESSAGE("gettext support failed with or without libintl")
				
				IF (VERBOSE)
					MESSAGE(STATUS "Compilation error with libintl:")
					MESSAGE(STATUS ${outputWithLibintl})

					MESSAGE(STATUS "Compilation error without libintl:")
					MESSAGE(STATUS ${outputWithoutLibintl})
				ENDIF (VERBOSE)
			ENDIF (LIBINTL_REQUIRED)
		ENDIF (LIBINTL_NOT_REQUIRED)
	ENDIF (GETTEXT_INCLUDE_DIR)
ELSE (GETTEXT)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (GETTEXT)

SET(ADM_LOCALE "${CMAKE_INSTALL_PREFIX}/share/locale")

MESSAGE("")

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
	MESSAGE(STATUS "<disabled per request>")
ENDIF (SDL)

MESSAGE("")

########################################
# XVideo
########################################
IF (ADM_OS_LINUX OR ADM_OS_DARWIN)	
	OPTION(XVIDEO "" ON)
	
	IF (X11_FOUND AND XVIDEO)
		MESSAGE(STATUS "Checking for XVideo")
		MESSAGE(STATUS "*******************")

		FIND_HEADER_AND_LIB(XVIDEO X11/extensions/Xvlib.h Xv XvShmPutImage "-include X11/Xlib.h ${X11_INCLUDE_DIR} ${X11_LIBRARIES}")
		PRINT_LIBRARY_INFO("XVideo" XVIDEO_FOUND "${XVIDEO_INCLUDE_DIR}" "${XVIDEO_LIBRARY}")
		
		IF (XVIDEO_FOUND)
			SET(USE_XV 1)
		ENDIF (XVIDEO_FOUND)
	
		MESSAGE("")
	ENDIF (X11_FOUND AND XVIDEO)
ELSE (ADM_OS_LINUX OR ADM_OS_DARWIN)
	SET(XVIDEO_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX OR ADM_OS_DARWIN)
