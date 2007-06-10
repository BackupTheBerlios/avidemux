###############################
# Part Of avidemux cmake conf #
###############################
##
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
include(lavcodec)
include(CMakeDetermineSystem)
include(adm_checkHeaderLib)
include(adm_compile)
#INCLUDE(CheckStructMember)
check_function_exists(gettimeofday    HAVE_GETTIMEOFDAY)
# Header
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
# Set lavcodec/util/format configuration
SET_LAVCODEC_FLAGS()

# Symbols
CHECK_SYMBOL_EXISTS(strcasecmp  "strings.h"         HAVE_STRCASECMP)    # simapi.h, various

# Functions
IF(NOT WIN32) # there is a chmod function on win32, but not usable the way we want...
  CHECK_FUNCTION_EXISTS(chmod     HAVE_CHMOD)         # __homedir/homedir.cpp, gpg/gpg.cpp
ENDIF(NOT WIN32)
# VERSION(s)
SET(VERSION 2.4)
SET(PACKAGE_VERSION 2.4)

# We use FFMPEG
SET(USE_MP3    1)
SET(USE_AC3    1)
SET(USE_FFMPEG    1)
SET(USE_MJPEG    1)
SET(USE_LIBXML2    1)
SET(HAVE_LRINTF    1)
SET(EMULATE_FAST_INT    1)
SET(RUNTIME_CPUDETECT    1)


########################################
# taist : Should fail!
########################################
ADM_CHECK_HL(teist larreme.h mzfdgp3lame lagggme_icnit HAVE_LzertgIBMP3LAME)
if(HAVE_LzertgIBMP3LAME)
  MESSAGE(FATAL "THAT TEST SHOULD HAVE FAILED EXPECT PROBLEM")
endif(HAVE_LzertgIBMP3LAME)

########################################
# Gettext
########################################
if(HAVE_LIBINTL_H)
  MESSAGE(STATUS "<Checking gettext >")
  MESSAGE(STATUS "<**************** >")
# Try linking without -lintl
  ADM_COMPILE( gettext.cpp "" "" WITHOUT_LIBINTL outputWithoutLibintl)
  if(WITHOUT_LIBINTL)
      SET(HAVE_GETTEXT 1)
      MESSAGE(STATUS "Ok, No lib needed (${ADM_GETTEXT_LIB})")
  else(WITHOUT_LIBINTL)
      ADM_COMPILE( gettext.cpp "" "-lintl" WITH_LIBINTL outputWithLibintl)
      if(WITH_LIBINTL)
        SET(ADM_GETTEXT_LIB "-lintl")
        SET(HAVE_GETTEXT 1)
        MESSAGE(STATUS "Ok, libintl needed")
      else(WITH_LIBINTL)
        MESSAGE(STATUS "Does not work, without ${outputWithoutLibintl}")
        MESSAGE(STATUS "Does not work, with    ${outputWithLibintl}")
      endif(WITH_LIBINTL)
  endif(WITHOUT_LIBINTL)
endif(HAVE_LIBINTL_H)
########################################
# WIN32
########################################
if(WIN32)
SET(WIN32_DEF 1)
else(WIN32)
SET(WIN32_DEF 0)
endif(WIN32)
########################################
# MacIntel
########################################
if(CMAKE_OSX_SYSROOT)
  SET(ADM_BSD_FAMILY 1)
#  SET(CYG_MANGLING 1)
endif(CMAKE_OSX_SYSROOT)

########################################
# ALSA
########################################
if(NOT WIN32 AND NOT CMAKE_OSX_SYSROOT)
  MESSAGE(STATUS "<Checking for ALSA>")
  MESSAGE(STATUS "<*****************>")
  include(FindAlsa)
  if(ALSA_FOUND)
    ALSA_VERSION_STRING(alsaVersion)
    MESSAGE("Found alsa version :${alsaVersion}") 
    MESSAGE("Found alsa lib     :${ASOUND_LIBRARY}")
    SET(ALSA_SUPPORT 1)
    SET(ALSA_1_0_SUPPORT 1)
  endif(ALSA_FOUND)
endif(NOT WIN32 AND NOT CMAKE_OSX_SYSROOT)
########################################
# SDL
########################################
MESSAGE(STATUS "<Checking for SDL>")
MESSAGE(STATUS "<*****************>")
#include(FindSDL)
include(admSDL)
ADM_FIND_SDL()
if(SDL_FOUND)
 SET(USE_SDL 1)
 MESSAGE(STATUS "Found")
else(SDL_FOUND)
  MESSAGE(STATUS "Not Found")
endif(SDL_FOUND)
########################################
# FONTCONFIG
########################################
ADM_CHECK_HL(FontConfig fontconfig/fontconfig.h fontconfig FcStrSetCreate USE_FONTCONFIG)
if(USE_FONTCONFIG)
SET(HAVE_FONTCONFIG 1)
endif(USE_FONTCONFIG)
########################################
# Xvideo
########################################
if(NOT WIN32)

SET(CMAKE_REQUIRED_FLAGS "-include X11/Xlib.h")
SET(CMAKE_REQUIRED_LIBRARIES "${X11_LIBRARIES}")
SET(CMAKE_REQUIRED_INCLUDE "${X11_INCLUDE_DIR}")

ADM_CHECK_HL(Xvideo X11/extensions/Xvlib.h Xv XvShmPutImage USE_XV)
MESSAGE(STATUS "(i used ${CMAKE_REQUIRED_FLAGS}**${X11_INCLUDE_DIR})")
MESSAGE(STATUS "(and ${CMAKE_REQUIRED_LIBRARIES}**${X11_LIBRARIES})")

SET(CMAKE_REQUIRED_FLAGS )
SET(CMAKE_REQUIRED_LIBRARIES )
SET(CMAKE_REQUIRED_INCLUDE )


endif(NOT WIN32)
########################################
# OSS
########################################

if(NOT WIN32)
MESSAGE(STATUS "<Checking for OSS>")
MESSAGE(STATUS "<*****************>")
CHECK_INCLUDE_FILES(sys/soundcard.h      OSS_SUPPORT)
if(OSS_SUPPORT)
MESSAGE(STATUS "Found")
else(OSS_SUPPORT)
MESSAGE(STATUS "Not found")
endif(OSS_SUPPORT)

endif(NOT WIN32)
########################################
# ESD
########################################
if(NOT WIN32)
ADM_CHECK_HL(Esd esd.h  esd esd_close USE_ESD)
endif(NOT WIN32)
########################################
# Aften
########################################
if(WIN32)
SET(CMAKE_REQUIRED_FLAGS "-lm -lpthreadGC2")
else(WIN32)
SET(CMAKE_REQUIRED_FLAGS "-lm -lpthread")
endif(WIN32)

ADM_CHECK_HL(Aften aften.h  aften aften_encode_init USE_AFTEN)
if(USE_AFTEN)
# TODO! Check aften version 5/6/7!
SET(USE_AFTEN_07 1)
endif(USE_AFTEN)
SET(CMAKE_REQUIRED_FLAGS "")
########################################
# ICONV
########################################
# search ICON.h
MESSAGE(STATUS "<Checking for iconv.h>")
MESSAGE(STATUS "<******************************>")
CHECK_INCLUDE_FILES(iconv.h HAVE_ICONV_H)
if(NOT HAVE_ICONV_H)
  MESSAGE(FATAL "iconv.h not found")
endif(NOT HAVE_ICONV_H)

# need libiconv ?
CHECK_LIBRARY_EXISTS(iconv libiconv "" LINK_WITH_ICONV)
if(LINK_WITH_ICONV)
  SET(NEED_LIBICONV 1)
  MESSAGE(STATUS "libiconv found, probably needed")
else(LINK_WITH_ICONV)
  MESSAGE(STATUS "libiconv not found, probably not needed")
endif(LINK_WITH_ICONV)

#
MESSAGE(STATUS "<Checking if iconv needs const>")

if(NEED_LIBICONV)
ADM_COMPILE_WITH_WITHOUT(iconv_check.cpp "-DICONV_NEED_CONST" "-liconv" ICONV_WITH)
else(NEED_LIBICONV)
ADM_COMPILE_WITH_WITHOUT(iconv_check.cpp "-DICONV_NEED_CONST" "-lm" ICONV_WITH)
endif(NEED_LIBICONV)

if(ICONV_WITH)
  MESSAGE(STATUS "Yes")
  SET(ICONV_NEED_CONST 1)
else(ICONV_WITH)
    MESSAGE(STATUS "No")
endif(ICONV_WITH)
########################################
# LAME
########################################
SET(CMAKE_REQUIRED_LIBRARIES "-lm")
ADM_CHECK_HL(Lame lame/lame.h mp3lame lame_init HAVE_LIBMP3LAME)
SET(CMAKE_REQUIRED_LIBRARIES )
########################################
# Xvid
########################################
ADM_CHECK_HL(Xvid xvid.h xvidcore xvid_plugin_single USE_XVID_4)
########################################
# AMR_NB
########################################
ADM_CHECK_HL(AMRNB amrnb/interf_dec.h amrnb GP3Decoder_Interface_Decode USE_AMR_NB)
if(USE_AMR_NB)
SET(CONFIG_AMR_NB 1)
endif(USE_AMR_NB)
########################################
# Libdca
########################################
SET(CMAKE_REQUIRED_FLAGS "-include stdint.h")
SET(CMAKE_REQUIRED_LIBRARIES "-lm")
ADM_CHECK_HL(libdca dts.h dts dts_init USE_LIBDCA)
SET(CMAKE_REQUIRED_LIBRARIES)
SET(CMAKE_REQUIRED_FLAGS)
########################################
# X264
########################################
SET(CMAKE_REQUIRED_FLAGS "-include stdint.h")
ADM_CHECK_HL(x264 x264.h x264 x264_encoder_open USE_X264)
SET(CMAKE_REQUIRED_FLAGS)
########################################
# PNG
########################################
ADM_CHECK_HL(libPNG png.h png png_malloc USE_PNG)


########################################
# FAAD
########################################

ADM_CHECK_HL(FAAD faad.h faad faacDecInit USE_FAAD_P)
if(NOT USE_FAAD_P)
 MESSAGE(STATUS "Trying neaac variant")
 ADM_CHECK_HL(NeAAC faad.h faad NeAACDecInit USE_FAAD_A)
endif(NOT USE_FAAD_P)
if(USE_FAAD_P OR USE_FAAD_A)
  SET(USE_FAAD  1)
endif(USE_FAAD_P OR USE_FAAD_A)
# See if we need old FAAD or NEW
if(USE_FAAD)
MESSAGE(STATUS "<Checking if faad needs old proto>")
ADM_COMPILE_WITH_WITHOUT(faad_check.cpp "-DOLD_FAAD_PROTO" "-lfaad" FAAD_WITH)
if(FAAD_WITH)
  MESSAGE(STATUS "Yes")
  SET(OLD_FAAD_PROTO 1)
else(FAAD_WITH)
    MESSAGE(STATUS "No")
endif(FAAD_WITH)
endif(USE_FAAD)
########################################
# FAAC
########################################
ADM_CHECK_HL(FAAC faac.h faac faacEncClose USE_FAAC)


########################################
# FreeType
########################################
if(FT_FOUND)
SET(USE_FREETYPE 1)
endif(FT_FOUND)
########################################
# Vorbis
########################################

 ADM_CHECK_HL(Vorbis vorbis/vorbisenc.h vorbis vorbis_info_init USE_VORBIS1)
 ADM_CHECK_HL(Vorbis vorbis/vorbisenc.h vorbisenc vorbis_encode_init USE_VORBIS2)
 if(USE_VORBIS1 AND USE_VORBIS2)
   SET(USE_VORBIS 1)
 endif(USE_VORBIS1 AND USE_VORBIS2)

########################################
# End test
########################################

 ADM_CHECK_HL(Invalid dummy_header.h dummy_libxyz dummy_func_tyu DUMMY_TEST)
 ADM_CHECK_HL(Invalid stdio.h dummy_libxyz dummy_func_tyu DUMMY_TEST2)
 if(DUMMY_TEST OR DUMMY_TEST2)
   MESSAGE(FATAL "This test should have failed!!!")
   MESSAGE(FATAL "This test should have failed!!!")
   MESSAGE(FATAL "This test should have failed!!!")
 endif(DUMMY_TEST OR DUMMY_TEST2)

########################################
# CPU and Host
########################################
# ASSUME GCC >=2
SET(HAVE_BUILTIN_VECTOR 1)
SET(HAVE_AUDIO    1)
MESSAGE(STATUS "<Checking CPU and OS>")
MESSAGE(STATUS "<*******************>")
MESSAGE("<CPU:${CMAKE_SYSTEM_PROCESSOR}>")
# windows is always X86 for now on...
 if(WIN32)
   MESSAGE(STATUS WIN32)
   SET(ADM_WIN32 1)
   SET(CYG_MANGLING 1)
   SET(ARCH_X86    1)
   SET(HAVE_MMX    1)
   SET(ARCH_X86    1)
   SET(HAVE_MMX    1)
   SET(ARCH_X86_32    1)
   SET(FPM_INTEL    1)
   SET(MEMALIGN_HACK    1)
   add_definitions(-mms-bitfields -mno-cygwin)
 else(WIN32)
      if(CMAKE_OSX_SYSROOT AND  ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
        SET(CYG_MANGLING 1)
        MESSAGE(STATUS "x86 CPU, MACINTEL")
        SET(ARCH_X86    1)
        SET(HAVE_MMX    1)
        SET(ARCH_X86_32    1)
        SET(FPM_INTEL    1)
      endif(CMAKE_OSX_SYSROOT AND  ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")

      if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
          MESSAGE(STATUS "x86 CPU")
          SET(ARCH_X86    1)
          SET(HAVE_MMX    1)
          SET(ARCH_X86_32    1)
          SET(FPM_INTEL    1)
     endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
     if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
         MESSAGE(STATUS "AMD64 CPU")
         SET(ARCH_X86    1)
         SET(ARCH_X86_64    1)
         SET(FPM_DEFAULT    1)
         SET(ARCH_64_BITS    1)
         SET(HAVE_MMX    1)
     endif( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
 endif(WIN32)
MESSAGE(STATUS "<End of CPU and OS Check>")
MESSAGE(STATUS "<******************************>")
#
include(adm_log)
#
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
MESSAGE(STATUS "** DEBUG BUILD (${CMAKE_BUILD_TYPR})**")
add_definitions(-DADM_DEBUG)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
MESSAGE(STATUS "** RELEASE BUILD (${CMAKE_BUILD_TYPR})**")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

MESSAGE("LINK_FLAGS ${CMAKE_LD_FLAGS}")
# EOF
