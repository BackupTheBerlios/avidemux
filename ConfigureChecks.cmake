########################################
# CPU and Host
########################################
MESSAGE(STATUS "Checking CPU and OS")

IF (WIN32)
	SET(ADM_OS_WINDOWS 1)
	
	IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
		SET(ADM_CPU_X86 1)
	ENDIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
ELSEIF (APPLE)
	SET(ADM_OS_APPLE 1)
	
	IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
		SET(ADM_CPU_X86 1)
	ENDIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
ELSEIF (UNIX)
	SET(ADM_OS_UNIX 1)
	
	IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
		SET(ADM_CPU_X86 1)
	ELSEIF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
		SET(ADM_CPU_X86_64 1)
	ENDIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
ENDIF (WIN32)

# Various defines (needs to be removed from Avidemux code one day and be library specific...)
IF (ADM_CPU_X86)
	SET(ARCH_X86 1)
	SET(ARCH_X86_32 1)
ELSEIF (ADM_CPU_X86_64)
	SET(ARCH_X86 1)
	SET(ARCH_X86_64 1)
	SET(ARCH_64_BITS 1)
ENDIF (ADM_CPU_X86)

########################################
# Include CMake scripts
########################################
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(lavcodec)
INCLUDE(CMakeDetermineSystem)
INCLUDE(adm_checkHeaderLib)
INCLUDE(adm_compile)

########################################
# Check functions, includes, symbols
########################################
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)

CHECK_INCLUDE_FILES(inttypes.h      HAVE_INTTYPES_H)                    # simapi.h
CHECK_INCLUDE_FILES(stddef.h        HAVE_STDDEF_H)                      # simapi.h
CHECK_INCLUDE_FILES(stdint.h        HAVE_STDINT_H)                      # simapi.h
CHECK_INCLUDE_FILES(stdlib.h        HAVE_STDLIB_H)                      # simapi.h
CHECK_INCLUDE_FILES(string.h        HAVE_STRING_H)                      # _core/libintl.cpp
CHECK_INCLUDE_FILES(sys/stat.h      HAVE_SYS_STAT_H)                    # gpg/gpg.cpp
CHECK_INCLUDE_FILES(sys/types.h     HAVE_SYS_TYPES_H)                   # simapi.h
CHECK_INCLUDE_FILES(unistd.h        HAVE_UNISTD_H)                      # simapi.h
CHECK_INCLUDE_FILES(malloc.h        HAVE_MALLOC_H)                      # simapi.h
CHECK_INCLUDE_FILES(libintl.h       HAVE_LIBINTL_H)                      # simapi.h

CHECK_SYMBOL_EXISTS(strcasecmp  "strings.h"         HAVE_STRCASECMP)    # simapi.h, various

########################################
# Mangling
########################################
IF (ADM_OS_WINDOWS OR ADM_OS_APPLE)
	SET(CYG_MANGLING 1)
ENDIF (ADM_OS_WINDOWS OR ADM_OS_APPLE)

########################################
# Avidemux OS specific tweaks
########################################
IF (ADM_OS_WINDOWS)
	SET(ADM_WIN32 1)		# needs to be removed one day...
	ADD_DEFINITIONS(-mms-bitfields -mno-cygwin)
ELSE (ADM_OS_WINDOWS)
	IF (ADM_OS_APPLE)
		SET(ADM_BSD_FAMILY 1)
	ENDIF (ADM_OS_APPLE)

	CHECK_FUNCTION_EXISTS(chmod HAVE_CHMOD)         # __homedir/homedir.cpp, gpg/gpg.cpp
ENDIF (ADM_OS_WINDOWS)

########################################
# Standard Avidemux defines
########################################
SET(VERSION 2.4)
SET(PACKAGE_VERSION 2.4)

SET(HAVE_BUILTIN_VECTOR 1)
SET(HAVE_AUDIO 1)

SET(USE_MP3 1)
SET(USE_AC3 1)
SET(USE_FFMPEG 1)
SET(USE_MJPEG 1)
SET(USE_LIBXML2 1)

########################################
# LibMad
########################################
IF (ADM_CPU_X86)
	SET(FPM_INTEL 1)
ELSEIF (ADM_CPU_X86_64)
	SET(FPM_DEFAULT 1)
ENDIF (ADM_CPU_X86)

########################################
# Libavcodec
########################################
SET_LAVCODEC_FLAGS()

########################################
# LibMpeg2Dec
########################################
SET(ACCEL_DETECT 1)

########################################
# Gettext
########################################
IF (HAVE_LIBINTL_H)
	MESSAGE(STATUS "<Checking gettext>")
	MESSAGE(STATUS "<****************>")

	IF (NO_NLS)
		MESSAGE(STATUS "<disabled per request>")
	ELSE (NO_NLS)
		# Try linking without -lintl
		ADM_COMPILE(gettext.cpp "" "" WITHOUT_LIBINTL outputWithoutLibintl)
		
		IF (WITHOUT_LIBINTL)
			SET(HAVE_GETTEXT 1)
			MESSAGE(STATUS "Ok, No lib needed (${ADM_GETTEXT_LIB})")
		ELSE (WITHOUT_LIBINTL)
			ADM_COMPILE(gettext.cpp "" "-lintl" WITH_LIBINTL outputWithLibintl)
			
			IF (WITH_LIBINTL)
				SET(ADM_GETTEXT_LIB "-lintl")
				SET(HAVE_GETTEXT 1)
				
				MESSAGE(STATUS "Ok, libintl needed")
			ELSE (WITH_LIBINTL)
				MESSAGE(STATUS "Does not work, without ${outputWithoutLibintl}")
				MESSAGE(STATUS "Does not work, with ${outputWithLibintl}")
			ENDIF (WITH_LIBINTL)
		ENDIF (WITHOUT_LIBINTL)
	ENDIF (NO_NLS)
ENDIF (HAVE_LIBINTL_H)

########################################
# Locale
########################################
SET(ADM_LOCALE "${CMAKE_INSTALL_PREFIX}/share/locale")

########################################
# ALSA
########################################
IF (ADM_OS_UNIX)
	MESSAGE(STATUS "<Checking for ALSA>")
	MESSAGE(STATUS "<*****************>")
	IF (NO_ALSA)
		MESSAGE(STATUS "<disabled per request>")
	ELSE (NO_ALSA)
		INCLUDE(FindAlsa)
		
		IF (ALSA_FOUND)
			ALSA_VERSION_STRING(alsaVersion)
			
			MESSAGE("Found alsa version :${alsaVersion}") 
			MESSAGE("Found alsa lib     :${ASOUND_LIBRARY}")
			
			SET(ALSA_SUPPORT 1)
			SET(ALSA_1_0_SUPPORT 1)
		ENDIF (ALSA_FOUND)
	ENDIF (NO_ALSA)
ENDIF (ADM_OS_UNIX)

########################################
# SDL
########################################
MESSAGE(STATUS "<Checking for SDL>")
MESSAGE(STATUS "<*****************>")

INCLUDE(admSDL)

IF (NO_SDL)
	MESSAGE(STATUS "<disabled per request>")
ELSE (NO_SDL)
	ADM_FIND_SDL()

	IF (SDL_FOUND)
		SET(USE_SDL 1)
		
		MESSAGE(STATUS "Found")
	ELSE (SDL_FOUND)
		MESSAGE(STATUS "Not Found")
	ENDIF (SDL_FOUND)
ENDIF (NO_SDL)

########################################
# FONTCONFIG
########################################
ADM_CHECK_HL(FontConfig fontconfig/fontconfig.h fontconfig FcStrSetCreate USE_FONTCONFIG)

IF (USE_FONTCONFIG)
	SET(HAVE_FONTCONFIG 1)
ENDIF (USE_FONTCONFIG)

########################################
# Xvideo
########################################
IF (NOT ADM_OS_WINDOWS)
	SET(CMAKE_REQUIRED_FLAGS "-include X11/Xlib.h")
	SET(CMAKE_REQUIRED_LIBRARIES "${X11_LIBRARIES}")
	SET(CMAKE_REQUIRED_INCLUDE "${X11_INCLUDE_DIR}")

	ADM_CHECK_HL(Xvideo X11/extensions/Xvlib.h Xv XvShmPutImage USE_XV)
	
	MESSAGE(STATUS "(X11 Flags: ${CMAKE_REQUIRED_FLAGS} ${X11_INCLUDE_DIR})")
	MESSAGE(STATUS "(X11 Libraries: ${X11_LIBRARIES})")

	SET(CMAKE_REQUIRED_FLAGS)
	SET(CMAKE_REQUIRED_LIBRARIES)
	SET(CMAKE_REQUIRED_INCLUDE)

ENDIF (NOT ADM_OS_WINDOWS)

########################################
# OSS
########################################
IF (NOT ADM_OS_WINDOWS)
	MESSAGE(STATUS "<Checking for OSS>")
	MESSAGE(STATUS "<*****************>")
	
	IF (NO_OSS)
		MESSAGE(STATUS "<disabled per request>")
	ELSE (NO_OSS)
		CHECK_INCLUDE_FILES(sys/soundcard.h OSS_SUPPORT)
		
		IF (OSS_SUPPORT)
			MESSAGE(STATUS "Found")
		ELSE (OSS_SUPPORT)
			MESSAGE(STATUS "Not found")
		ENDIF (OSS_SUPPORT)
	ENDIF (NO_OSS)
ENDIF (NOT ADM_OS_WINDOWS)

########################################
# ARTS
########################################
IF (NOT ADM_OS_WINDOWS)
	INCLUDE(FindArts)
ENDIF (NOT ADM_OS_WINDOWS)

########################################
# ESD
########################################
IF (NOT ADM_OS_WINDOWS)
	ADM_CHECK_HL(Esd esd.h  esd esd_close USE_ESD)
ENDIF (NOT ADM_OS_WINDOWS)

########################################
# JACK
########################################
IF (NOT ADM_OS_WINDOWS)
	ADM_CHECK_HL(Jack jack/jack.h  jack jack_client_close USE_JACK)
ENDIF (NOT ADM_OS_WINDOWS)

########################################
# Aften
########################################
IF (ADM_OS_WINDOWS)
	SET(CMAKE_REQUIRED_FLAGS "-lm -lpthreadGC2")
ELSE (ADM_OS_WINDOWS)
	SET(CMAKE_REQUIRED_FLAGS "-lm -lpthread")
ENDIF (ADM_OS_WINDOWS)

ADM_CHECK_HL(Aften aften/aften.h aften aften_encode_init USE_AFTEN)

IF (USE_AFTEN)
	FIND_LIBRARY(AFTEN_LIB_PATH NAMES aften)

	TRY_RUN(AFTEN_TEST_RUN_RESULT
		AFTEN_TEST_COMPILE_RESULT
		${CMAKE_BINARY_DIR}
		"${CMAKE_SOURCE_DIR}/cmake_compile_check/aften_check.cpp"
		CMAKE_FLAGS -DLINK_LIBRARIES=${AFTEN_LIB_PATH})

	IF (AFTEN_TEST_RUN_RESULT EQUAL 8)
		MESSAGE(STATUS "Aften Version: 0.0.8")
		SET(USE_AFTEN_08 1)
	ELSEIF (AFTEN_TEST_RUN_RESULT EQUAL 7)
		MESSAGE(STATUS "Aften Version: 0.07")
		SET(USE_AFTEN_07 1)
	ELSE (AFTEN_TEST_RUN_RESULT EQUAL 8)
		SET(USE_AFTEN 0)
	ENDIF (AFTEN_TEST_RUN_RESULT EQUAL 8)
ENDIF (USE_AFTEN)

SET(CMAKE_REQUIRED_FLAGS "")

########################################
# Secret Rabbit Code
########################################
ADM_CHECK_HL(libsamplerate samplerate.h samplerate src_get_version USE_SRC)

########################################
# ICONV
########################################
MESSAGE(STATUS "<Checking for iconv.h>")
MESSAGE(STATUS "<******************************>")

CHECK_INCLUDE_FILES(iconv.h HAVE_ICONV_H)

IF (NOT HAVE_ICONV_H)
	MESSAGE(FATAL "iconv.h not found")
ENDIF (NOT HAVE_ICONV_H)

# need libiconv ?
CHECK_LIBRARY_EXISTS(iconv libiconv "" LINK_WITH_ICONV)

IF (LINK_WITH_ICONV)
	SET(NEED_LIBICONV 1)
	MESSAGE(STATUS "libiconv found, probably needed")
ELSE (LINK_WITH_ICONV)
	MESSAGE(STATUS "libiconv not found, probably not needed")
ENDIF (LINK_WITH_ICONV)

MESSAGE(STATUS "<Checking if iconv needs const>")

IF (NEED_LIBICONV)
	ADM_COMPILE_WITH_WITHOUT(iconv_check.cpp "-DICONV_NEED_CONST" "-liconv" ICONV_WITH)
ELSE (NEED_LIBICONV)
	ADM_COMPILE_WITH_WITHOUT(iconv_check.cpp "-DICONV_NEED_CONST" "-lm" ICONV_WITH)
ENDIF (NEED_LIBICONV)

IF (ICONV_WITH)
	MESSAGE(STATUS "Yes")
	SET(ICONV_NEED_CONST 1)
ELSE (ICONV_WITH)
    MESSAGE(STATUS "No")
ENDIF(ICONV_WITH)

########################################
# LAME
########################################
SET(CMAKE_REQUIRED_LIBRARIES "-lm")
ADM_CHECK_HL(Lame lame/lame.h mp3lame lame_init HAVE_LIBMP3LAME)
SET(CMAKE_REQUIRED_LIBRARIES)

########################################
# Xvid
########################################
ADM_CHECK_HL(Xvid xvid.h xvidcore xvid_plugin_single USE_XVID_4)

########################################
# AMR_NB
########################################
IF (USE_LATE_BINDING)
	CHECK_INCLUDE_FILES(amrnb/interf_dec.h USE_AMR_NB)
ELSE (USE_LATE_BINDING)
	ADM_CHECK_HL(AMRNB amrnb/interf_dec.h amrnb GP3Decoder_Interface_Decode USE_AMR_NB)
ENDIF (USE_LATE_BINDING)

IF (USE_AMR_NB)
	SET(CONFIG_AMR_NB 1)
ENDIF (USE_AMR_NB)

########################################
# Libdca
########################################
SET(CMAKE_REQUIRED_FLAGS "-include stdint.h")
SET(CMAKE_REQUIRED_LIBRARIES "-lm")

IF (USE_LATE_BINDING)
	CHECK_INCLUDE_FILES(dts.h USE_LIBDCA)
ELSE (USE_LATE_BINDING)
	ADM_CHECK_HL(libdca dts.h dts dts_init USE_LIBDCA)
ENDIF (USE_LATE_BINDING)

SET(CMAKE_REQUIRED_LIBRARIES)
SET(CMAKE_REQUIRED_FLAGS)

########################################
# X264
########################################
SET(CMAKE_REQUIRED_FLAGS "-include stdint.h")

IF (ADM_OS_WINDOWS)
	SET(CMAKE_REQUIRED_LIBRARIES "-lm -lpthreadGC2")
ELSE (ADM_OS_WINDOWS)
	SET(CMAKE_REQUIRED_LIBRARIES "-lm -lpthread")
ENDIF (ADM_OS_WINDOWS)

ADM_CHECK_HL(x264 x264.h x264 x264_encoder_open USE_X264)
SET(CMAKE_REQUIRED_FLAGS)
SET(CMAKE_REQUIRED_LIBRARIES)

########################################
# PNG
########################################
ADM_CHECK_HL(libPNG png.h png png_malloc USE_PNG)

########################################
# FAAD
########################################
ADM_CHECK_HL(FAAD faad.h faad faacDecInit USE_FAAD_P)

IF(NOT USE_FAAD_P)
	MESSAGE(STATUS "Trying neaac variant")
	ADM_CHECK_HL(NeAAC faad.h faad NeAACDecInit USE_FAAD_A)
ENDIF(NOT USE_FAAD_P)

IF(USE_FAAD_P OR USE_FAAD_A)
	SET(USE_FAAD  1)
ENDIF(USE_FAAD_P OR USE_FAAD_A)

# See if we need old FAAD or new
IF (USE_FAAD)
	MESSAGE(STATUS "<Checking if faad needs old proto>")
	ADM_COMPILE_WITH_WITHOUT(faad_check.cpp "-DOLD_FAAD_PROTO" "-lfaad" FAAD_WITH)
	
	IF(FAAD_WITH)
		MESSAGE(STATUS "Yes")
		SET(OLD_FAAD_PROTO 1)
	ELSE (FAAD_WITH)
		MESSAGE(STATUS "No")
	ENDIF (FAAD_WITH)
ENDIF(USE_FAAD)

########################################
# FAAC
########################################
SET(CMAKE_REQUIRED_FLAGS "-include stdint.h")
ADM_CHECK_HL(FAAC faac.h faac faacEncClose USE_FAAC)
SET(CMAKE_REQUIRED_FLAGS)

########################################
# FreeType
########################################
IF (FT_FOUND)
	SET(USE_FREETYPE 1)
ENDIF (FT_FOUND)

########################################
# Vorbis
########################################
ADM_CHECK_HL(Vorbis vorbis/vorbisenc.h vorbis vorbis_info_init USE_VORBIS1)
ADM_CHECK_HL(Vorbis vorbis/vorbisenc.h vorbisenc vorbis_encode_init USE_VORBIS2)

IF (USE_VORBIS1 AND USE_VORBIS2)
	SET(USE_VORBIS 1)
ENDIF (USE_VORBIS1 AND USE_VORBIS2)

########################################
# End test
########################################
ADM_CHECK_HL(Invalid dummy_header.h dummy_libxyz dummy_func_tyu DUMMY_TEST)
ADM_CHECK_HL(Invalid stdio.h dummy_libxyz dummy_func_tyu DUMMY_TEST2)

IF (DUMMY_TEST OR DUMMY_TEST2)
	MESSAGE(FATAL "This test should have failed!!!")
	MESSAGE(FATAL "This test should have failed!!!")
	MESSAGE(FATAL "This test should have failed!!!")
ENDIF (DUMMY_TEST OR DUMMY_TEST2)

INCLUDE(adm_log)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE(STATUS "** DEBUG BUILD (${CMAKE_BUILD_TYPE})**")
	
	SET(ADM_DEBUG 1)
	ADD_DEFINITIONS(-DADM_DEBUG)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE(STATUS "** RELEASE BUILD (${CMAKE_BUILD_TYPE})**")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

MESSAGE("LINK_FLAGS ${CMAKE_LD_FLAGS}")
# EOF
