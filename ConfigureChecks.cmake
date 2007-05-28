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
SET(USE_FFMPEG    1)
SET(USE_MJPEG    1)
SET(USE_LIBXML2    1)
SET(HAVE_LRINTF    1)
SET(EMULATE_FAST_INT    1)
SET(RUNTIME_CPUDETECT    1)
########################################
# CPU and Host
########################################
SET(HAVE_AUDIO    1)
MESSAGE("Checking CPU and OS")
MESSAGE("<CPU:${CMAKE_SYSTEM_PROCESSOR}>")
# windows is always X86 for now on...
 if(WIN32)
   MESSAGE(STATUS WIN32)
   SET(ADM_WIN32 1)
   SET(CYG_MANGLING 1)
   SET(ARCH_X86    1)
   SET(HAVE_MMX    1)
   SET(ARCH_X86_32    1)
   SET(FPM_INTEL    1)
 else(WIN32)
      if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
          MESSAGE(STATUS "x86 CPU")
          SET(ARCH_X86    1)
          SET(HAVE_MMX    1)
          SET(ARCH_X86_32    1)
          SET(FPM_INTEL    1)
     endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i586" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
     if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
         MESSAGE(STATUS "AMD64 CPU")
         SET(ARCH_X86_64    1)
         SET(FPM_DEFAULT    1)
         SET(ARCH_64_BITS    1)
     endif( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
 endif(WIN32)
MESSAGE("End of CPU and OS Check")
#
SET(ADM_DEBUG 1)
# EOF
