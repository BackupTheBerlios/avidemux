##########################################################
# main cmake file for avidemux
# Derived from the one available in sim-im & kde4
##########################################################
PROJECT(Avidemux2)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
SET(CMAKE_COLOR_MAKEFILE ON)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(UsePkgConfig)
include(MacroEnsureOutOfSourceBuild)

include(Po)
include(CheckCXXCompilerFlag)
include(FindLibXml2)
#**************************************************
# MESSAGE
#**************************************************
MESSAGE("#####################################")
MESSAGE("Configure Started")
MESSAGE("#####################################")
#**************************************************
# libxml2
#**************************************************
if(NOT LIBXML2_FOUND)
  MESSAGE(FATAL_ERROR "Could not find Libxml2")
endif(NOT LIBXML2_FOUND)
SET(LIBXML2_DEFINITIONS,${LIBXML2_DEFINITIONS})
SET(LIBXML2_INCLUDE_DIR,${LIBXML2_INCLUDE_DIR})
#**************************************************
# PKG Config
#**************************************************
MESSAGE("Checking for PKG-CONFIG")
if(NOT PKGCONFIG_EXECUTABLE)
             MESSAGE(FATAL_ERROR "Could not find pkg-config")
endif(NOT PKGCONFIG_EXECUTABLE)
MESSAGE(STATUS OK)
#**************************************************
# Search for FREETYPE
#**************************************************
MESSAGE("Checking for FreeType through PKG-CONFIG")
PKGCONFIG(freetype2 FT_INCLUDE FT_LIB FT_LDFLAGS FT_CFLAGS)

IF(NOT FT_INCLUDE)
    MESSAGE(STATUS "Could not find Freetype ")
ELSE(NOT FT_INCLUDE)
    SET(FT_FOUND TRUE)
    SET(FT_CFLAGS ${FT_CFLAGS})
    SET(FT_INCLUDE ${FT_INCLUDE})
    MESSAGE(STATUS OK)
    MESSAGE("FreeType Found ("${FT_CFLAGS} ${FT_INCLUDE}")")
    
ENDIF(NOT FT_INCLUDE)
#**************************************************
# Search for  X11 (not on windows)
#**************************************************
if(NOT WIN32)
        MESSAGE("Checking for X11")
        include(FindX11)
        if(NOT X11_FOUND)
             MESSAGE(FATAL_ERROR "Could not find X11")
        endif(NOT X11_FOUND)
        MESSAGE(STATUS OK)
endif(NOT WIN32)
#**************************************************
# Search for GLIB through PKG-Config
#**************************************************
MESSAGE("Checking for GLIB")
PKGCONFIG(glib-2.0 GLIB_INCLUDE GLIB_LIB GLIB_LDFLAGS GLIB_CFLAGS)

IF(NOT GLIB_INCLUDE)
    MESSAGE(STATUS "Could not find GLIB - cannot compile avidemux2_gtk here")
ELSE(NOT GLIB_INCLUDE)
    SET(GLIB_FOUND,TRUE)
    SET(GLIB_CFLAGS,${GLIB_CFLAGS})
    SET(GLIB_LDFLAGS,${GLIB_LDFLAGS})
    SET(GLIB_LIB,${GLIB_LIB})
    MESSAGE(STATUS OK)
    MESSAGE("GLIB Found ("${GLIB_CFLAGS}")")
    
ENDIF(NOT GLIB_INCLUDE)
#**************************************************
# Search for GTK through PKG-Config
#**************************************************
MESSAGE("Checking for GTK")
PKGCONFIG(gtk+-2.0 GTK_INCLUDE GTK_LIB GTK_LDFLAGS GTK_CFLAGS)

IF(NOT GTK_CFLAGS)
    MESSAGE(STATUS "Could not find GTK - cannot compile avidemux2_gtk here")
ELSE(NOT GTK_CFLAGS)
    SET(GTK_FOUND TRUE)
    SET(GTK_CFLAGS,${GTK_CFLAGS})
    SET(GTK_LDFLAGS,${GTK_LDFLAGS})
    SET(GTK_LIB,${GTK_LIB})
    MESSAGE(STATUS OK)
ENDIF(NOT GTK_CFLAGS)
#**************************************************
# Search for Gthread through PKG-Config
#**************************************************
MESSAGE("Checking for Gthread")
PKGCONFIG(gthread-2.0 GTHREAD_INCLUDE GTHREAD_LIB GTHREAD_LDFLAGS GTHREAD_CFLAGS)

IF(NOT GTHREAD_CFLAGS)
    MESSAGE(STATUS "Could not find GTHREAD")
    SET(GTK_FOUND FALSE)
ELSE(NOT GTHREAD_CFLAGS)
    SET(GTHREAD_LDFLAGS,${GTHREAD_LDFLAGS})
    MESSAGE(STATUS OK)
ENDIF(NOT GTHREAD_CFLAGS)
#**************************************************
# Search for QT4 
#**************************************************
MESSAGE("Checking for QT4")
include(FindQt4)

# do config checks
include(ConfigureChecks.cmake)

# create config.h
CONFIGURE_FILE(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
#
include_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/avidemux ${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_utilities/ ${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_lavutil)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_lavcodec  )
#**********************************************
# Some CFLAGS
#**********************************************
add_definitions (-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
add_definitions( -DHAVE_CONFIG_H )

ADD_SUBDIRECTORY(avidemux)

