####################################
#
#
#
#
####################################
PROJECT(AdmPlugins)

MESSAGE("#####################################")
MESSAGE("Plugins Configure Started")
MESSAGE("#####################################")
MESSAGE("")

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

if (COMMAND cmake_policy)
	cmake_policy(VERSION 2.4)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake" "${CMAKE_MODULE_PATH}")

IF (NOT CMAKE_CXX_COMPILER_WORKS MATCHES "^${CMAKE_CXX_COMPILER_WORKS}$")
	SET(FRESH_BUILD 1)
ENDIF (NOT CMAKE_CXX_COMPILER_WORKS MATCHES "^${CMAKE_CXX_COMPILER_WORKS}$")


IF (${AdmPlugins_SOURCE_DIR} MATCHES ${AdmPlugins_BINARY_DIR})
	MESSAGE("Please do an out-of-tree build:")
	MESSAGE("rm CMakeCache.txt; mkdir build; cd build; cmake ../plugins; make")
	MESSAGE(FATAL_ERROR "in-tree-build detected")
ENDIF (${AdmPlugins_SOURCE_DIR} MATCHES ${AdmPlugins_BINARY_DIR})

include(admConfigHelper)

IF (FRESH_BUILD)
	MESSAGE("")
ENDIF (FRESH_BUILD)

########################################
# Where is the top dir
########################################
IF(NOT AVIDEMUX_SOURCE_DIR)
	MESSAGE(FATAL_ERROR "Please add -DAVIDEMUX_SOURCE_DIR=path_to_avidemux_source")
ENDIF(NOT AVIDEMUX_SOURCE_DIR)
INCLUDE_DIRECTORIES(${AVIDEMUX_SOURCE_DIR})
########################################
# Where is coreConfig ?
########################################
IF(NOT AVIDEMUX_CORECONFIG_DIR)
	MESSAGE(FATAL_ERROR "Please add -DAVIDEMUX_CORECONFIG_DIR=path_to_ADM_coreConfig.h")
ENDIF(NOT AVIDEMUX_CORECONFIG_DIR)
INCLUDE_DIRECTORIES(${AVIDEMUX_CORECONFIG_DIR})
########################################
# Global options
########################################
OPTION(VERBOSE "" OFF)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE)
########################################
# Avidemux system specific tweaks
########################################
INCLUDE(admDetermineSystem)

IF (CYGWIN)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwin32")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32")
ENDIF (CYGWIN)

IF (ADM_CPU_ALTIVEC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ADM_ALTIVEC_FLAGS}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADM_ALTIVEC_FLAGS}")
ENDIF (ADM_CPU_ALTIVEC)
IF (WIN32)
	SET(BIN_DIR .)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-enable-auto-image-base -Wl,-enable-auto-import")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-enable-auto-import")

	IF (CMAKE_BUILD_TYPE STREQUAL "Release")
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-s")
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-s")
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")
ELSE (WIN32)
	SET(BIN_DIR bin)
ENDIF (WIN32)

########################################
# Standard Avidemux defines
########################################
SET(VERSION 2.5.0)

SET(ADM_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET(ADM_DEBUG 1)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

INCLUDE(admCheckRequiredLibs)

INCLUDE(admCheckMiscLibs)

########################################
# Actual work to do..
########################################

ADD_SUBDIRECTORY(ADM_audiodecoder)
ADD_SUBDIRECTORY(ADM_videoEncoder)
ADD_SUBDIRECTORY(ADM_videoFilters)
#
# End of file
#

