MESSAGE("#####################################")
MESSAGE("Configure Started")
MESSAGE("#####################################")
MESSAGE("")

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

PROJECT(Avidemux)

IF (${Avidemux_SOURCE_DIR} MATCHES ${Avidemux_BINARY_DIR})
	MESSAGE("Please do an out-of-tree build:")
	MESSAGE("rm CMakeCache.txt; mkdir build; cd build; cmake ..; make")
	MESSAGE(FATAL_ERROR "in-tree-build detected")
ENDIF (${Avidemux_SOURCE_DIR} MATCHES ${Avidemux_BINARY_DIR})

INCLUDE(CheckCXXCompilerFlag)
INCLUDE(admConfigHelper)

MESSAGE("")

########################################
# Global options
########################################
OPTION(XMINGW "" OFF)
OPTION(VERBOSE "" OFF)
OPTION(LATE_BINDING "" OFF)

IF (XMINGW)
	SET(WIN32 1)
ENDIF (XMINGW)

IF (LATE_BINDING)
	SET(USE_LATE_BINDING 1)
ENDIF (LATE_BINDING)

########################################
# Avidemux OS specific tweaks
########################################
INCLUDE(admDetermineSystem)

IF (ADM_OS_WINDOWS)
	SET(CYG_MANGLING 1)
	SET(ADM_WIN32 1)	# needs to be removed one day...
ENDIF (ADM_OS_WINDOWS)

IF (ADM_OS_DARWIN)
	SET(CYG_MANGLING 1)
ENDIF (ADM_OS_DARWIN)

IF (ADM_OS_LINUX)
	SET(USE_JOG 1)		# jog shuttle is only available on Linux due to its interface
ENDIF (ADM_OS_LINUX)

########################################
# Standard Avidemux defines
########################################
SET(VERSION 2.5)

SET(HAVE_BUILTIN_VECTOR 1)
SET(HAVE_AUDIO 1)

SET(USE_MP3 1)
SET(USE_AC3 1)
SET(USE_FFMPEG 1)
SET(USE_MJPEG 1)

# ARCH_XXX need to be removed from Avidemux code one day and made library specific...
IF (ADM_CPU_X86)
	SET(ARCH_X86 1)
	SET(ARCH_X86_32 1)
ELSEIF (ADM_CPU_X86_64)
	SET(ARCH_X86 1)
	SET(ARCH_X86_64 1)
	SET(ARCH_64_BITS 1)
ELSEIF (ADM_CPU_PPC)
	SET(ADM_BIG_ENDIAN 1)
	SET(WORDS_BIGENDIAN 1)

	OPTION(ALTIVEC ""  ON)

	IF (ALTIVEC)
		SET(ADM_CPUCAP_ALTIVEC 1)
		SET(ARCH_PPC 1)
		SET(ARCH_POWERPC 1)
		SET(USE_ALTIVEC 1)
		SET(HAVE_ALTIVEC 1)
	ENDIF (ALTIVEC)
ENDIF (ADM_CPU_X86)

# LibMad
IF (ADM_CPU_X86)
	SET(FPM_INTEL 1)
ELSEIF (ADM_CPU_X86_64)
	SET(FPM_DEFAULT 1)
ELSEIF (ADM_CPU_PPC)
	SET(FPM_PPC 1)
ENDIF (ADM_CPU_X86)

# Libavcodec
INCLUDE(admSetLavcodecFlags)

# LibMpeg2Dec
SET(ACCEL_DETECT 1)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET(ADM_DEBUG 1)
ENDIF (NOT CMAKE_BUILD_TYPE)

###########################################
# CPU Specific
###########################################
IF (ADM_CPUCAP_ALTIVEC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mabi=altivec -maltivec")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mabi=altivec -maltivec")

	IF (ADM_OS_APPLE)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -faltivec -force_cpusubtype_ALL")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faltivec -force_cpusubtype_ALL")
	ENDIF (ADM_OS_APPLE)
ENDIF (ADM_CPUCAP_ALTIVEC)

########################################
# Subversion
########################################
OPTION(SVN "" ON)

IF (SVN AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND EXISTS "${CMAKE_SOURCE_DIR}/.svn")
	MESSAGE(STATUS "Checking for Subversion")
	MESSAGE(STATUS "***********************")

	SET(Subversion_FIND_REQUIRED OFF)
	FIND_PACKAGE(Subversion)

	IF (Subversion_FOUND)
		Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
		MESSAGE(STATUS "  revision: ${Project_WC_REVISION}")
		SET(ADM_SUBVERSION ${Project_WC_REVISION})
	ELSE (Subversion_FOUND)
		MESSAGE(STATUS "Could not find Subversion")
	ENDIF (Subversion_FOUND)
	
	MESSAGE("")
ENDIF (SVN AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND EXISTS "${CMAKE_SOURCE_DIR}/.svn")

IF (NOT Subversion_FOUND)
	SET(ADM_SUBVERSION 0)
ENDIF (NOT Subversion_FOUND)

########################################
# Check for libraries
########################################
# Define internal flags for GTK+ and Qt4 builds.  These are turned off
# if a showstopper is found.  CLI is automatically assumed as possible
# since it uses the minimum set of required libraries and CMake will 
# fail if these aren't met.
SET(ADM_UI_GTK 1)
SET(ADM_UI_QT4 1)

SET(MSG_DISABLE_OPTION "Disabled per request")

INCLUDE(admCheckRequiredLibs)
INCLUDE(admCheckUserInterfaceLibs)
INCLUDE(admCheckVideoCodecLibs)
INCLUDE(admCheckAudioCodecLibs)
INCLUDE(admCheckAudioDeviceLibs)
INCLUDE(admCheckMiscLibs)

########################################
# Check functions and includes
########################################
MESSAGE(STATUS "Checking system headers")
MESSAGE(STATUS "***********************")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)

CHECK_INCLUDE_FILES(inttypes.h   HAVE_INTTYPES_H)	# internal use, mpeg2enc, mplex
CHECK_INCLUDE_FILES(stdint.h     HAVE_STDINT_H)		# internal use, mpeg2enc, mplex
CHECK_INCLUDE_FILES(sys/types.h  HAVE_SYS_TYPES_H)	# mad, mpeg2enc
CHECK_INCLUDE_FILES(unistd.h     HAVE_UNISTD_H)		# internal use, ass, mad
CHECK_INCLUDE_FILES(malloc.h     HAVE_MALLOC_H)		# libavcodec, libavutil, libpostproc, libswscale, mplex

IF (ADM_CPUCAP_ALTIVEC)
	CHECK_INCLUDE_FILES(altivec.h    HAVE_ALTIVEC_H)	# libavcodec, libpostproc, libswscale, mpeg2dec, mpeg2enc
ENDIF (ADM_CPUCAP_ALTIVEC)

MESSAGE("")

########################################
# Print Summary
########################################
IF (VERBOSE)
	INCLUDE(adm_log)
	MESSAGE("")
ENDIF (VERBOSE)

INCLUDE(admConfigSummary)
MESSAGE("")

########################################
# Generate config.h
########################################
SET(CONFIG_HEADER_TYPE ADM_BUILD_CLI)
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config/cli/config.h")
MESSAGE(STATUS "CLI config.h generated")

SET(CONFIG_HEADER_TYPE ADM_BUILD_GTK)
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config/gtk/config.h")
MESSAGE(STATUS "GTK config.h generated")

SET(CONFIG_HEADER_TYPE ADM_BUILD_QT4)
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config/qt4/config.h")
MESSAGE(STATUS "QT4 config.h generated")

########################################
# Directories to build
########################################
ADD_SUBDIRECTORY(avidemux)
ADD_SUBDIRECTORY(po)
