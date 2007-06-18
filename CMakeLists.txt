##########################################################
# main cmake file for avidemux
# Derived from the one available in sim-im & kde4
##########################################################
MESSAGE("#####################################")
MESSAGE("Configure Started")
MESSAGE("#####################################")
PROJECT(Avidemux2)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
SET(CMAKE_COLOR_MAKEFILE ON)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# XMingw ?
OPTION(XMINGW FALSE)
if(XMINGW)
MESSAGE(STATUS "Xmingw set, forcing win32 target")
SET(WIN32 1)
endif(XMINGW)

#**************************************************
# MESSAGE
#**************************************************
SET(CMAKE_C_FLAGS $ENV{CFLAGS})
SET(CMAKE_CXX_FLAGS $ENV{CXXFLAGS})

MESSAGE("EXTRA Cflags:${CMAKE_C_FLAGS}")
MESSAGE("EXTRA CXXflags:${CMAKE_CXX_FLAGS}")
#*************************************************
include(UsePkgConfig)
include(MacroEnsureOutOfSourceBuild)

include(Po)
include(CheckCXXCompilerFlag)

#**************************************************
# Subversion
#**************************************************
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE(STATUS "<Checking for Subversion>")
	MESSAGE(STATUS "<***********************>")
	include(FindSubversion)

	FIND_PACKAGE(Subversion)
	IF(Subversion_FOUND)
		Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
		MESSAGE(STATUS "Current revision is ${Project_WC_REVISION}")
		SET(ADM_SUBVERSION ${Project_WC_REVISION})
	ELSE(Subversion_FOUND)
		MESSAGE(STATUS "Could not find Subversion")
	ENDIF(Subversion_FOUND)
ENDIF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")

IF(NOT Subversion_FOUND)
	SET(ADM_SUBVERSION 0)
ENDIF(NOT Subversion_FOUND)

#**************************************************
# libxml2
#**************************************************
MESSAGE(STATUS "<Checking for LibXML2>")
MESSAGE(STATUS "<*********************>")
include(FindLibXml2)
if(NOT LIBXML2_FOUND)
  MESSAGE(FATAL_ERROR "Could not find Libxml2")
endif(NOT LIBXML2_FOUND)
MESSAGE(STATUS "LibXML2 OK")
SET(LIBXML2_DEFINITIONS,${LIBXML2_DEFINITIONS})
SET(LIBXML2_INCLUDE_DIR,${LIBXML2_INCLUDE_DIR})
#**************************************************
# PKG Config
#**************************************************
MESSAGE(STATUS "<Checking for PKG-CONFIG>")
MESSAGE(STATUS "<***********************>")
if(NOT PKGCONFIG_EXECUTABLE)
             MESSAGE(FATAL_ERROR "Could not find pkg-config")
endif(NOT PKGCONFIG_EXECUTABLE)
MESSAGE(STATUS "OK ${PKGCONFIG_EXECUTABLE}")
#**************************************************
# Search for FREETYPE
#**************************************************
MESSAGE(STATUS "<Checking for FreeType through PKG-CONFIG>")
MESSAGE(STATUS "<****************************************>")
PKGCONFIG(freetype2 FT_INCLUDE FT_LIB FT_LDFLAGS FT_CFLAGS)

IF(NOT FT_INCLUDE)
    MESSAGE(STATUS "Could not find Freetype ")
ELSE(NOT FT_INCLUDE)
    SET(FT_FOUND TRUE)
    SET(FT_CFLAGS ${FT_CFLAGS})
    SET(FT_LDFLAGS ${FT_LDFLAGS})
    SET(FT_INCLUDE ${FT_INCLUDE})
    MESSAGE(STATUS "LDFLAGS <${FT_LDFLAGS}>")
    MESSAGE(STATUS "LIBS <${FT_LIB}>")
    MESSAGE(STATUS OK)
    MESSAGE(STATUS "FreeType Found ("${FT_CFLAGS} ${FT_INCLUDE}")")
    
ENDIF(NOT FT_INCLUDE)
#**************************************************
# Search for  X11 (not on windows)
#**************************************************
if(NOT WIN32)
        MESSAGE(STATUS "<Checking for X11>")
        MESSAGE(STATUS "<******************************>")
        include(FindX11)
        if(NOT X11_FOUND)
             MESSAGE(FATAL_ERROR "Could not find X11")
        endif(NOT X11_FOUND)
        MESSAGE(STATUS OK)
endif(NOT WIN32)
#**************************************************
# Search for GLIB through PKG-Config
#**************************************************
MESSAGE(STATUS "<Checking for GLIB>")
MESSAGE(STATUS "<******************************>")
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
MESSAGE(STATUS "<Checking for GTK>")
MESSAGE(STATUS "<******************************>")
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
MESSAGE(STATUS "<Checking for Gthread>")
MESSAGE(STATUS "<******************************>")
PKGCONFIG(gthread-2.0 GTHREAD_INCLUDE GTHREAD_LIB GTHREAD_LDFLAGS GTHREAD_CFLAGS)

IF(NOT GTHREAD_CFLAGS)
    MESSAGE(STATUS "Could not find GTHREAD")
    SET(GTK_FOUND FALSE)
ELSE(NOT GTHREAD_CFLAGS)
    SET(GTHREAD_LDFLAGS,${GTHREAD_LDFLAGS})
    MESSAGE(STATUS "OK ${GTHREAD_LDFLAGS}")
ENDIF(NOT GTHREAD_CFLAGS)
#**************************************************
# Search for QT4 
#**************************************************
MESSAGE(STATUS "<Checking for QT4>")
MESSAGE(STATUS "<******************************>")
include(FindQt4)
if(QT4_FOUND AND MINGW)
if(XMINGW) # Cross compile ?
        SET(QTXM /rot2/qt_4.3)
        #
	SET(QT_QMAKE_EXECUTABLE qmake)
	SET(QT_UIC_EXECUTABLE uic-qt4)
	SET(QT_MOC_EXECUTABLE moc-qt4)
	SET(QT_RCC_EXECUTABLE rcc)
	SET(QT_QTGUI_LIBRARY QtGui4 ) # FIXME
        SET(QT_QTCORE_LIBRARY QtCore4 ) # FIXME

        SET(CMAKE_CXX_FLAGS "-I${QTXM}/include ${CMAKE_CXX_FLAGS}")
        LINK_DIRECTORIES(${QTXM}/bin)
        LINK_DIRECTORIES(${QTXM}/lib)

else(XMINGW)
	MESSAGE(STATUS "** MINGW ** OVERRIDING PATH FOR QT4 You may have to edit CMakeLists.txt!!!")
	SET(QT_QMAKE_EXECUTABLE qmake)
	SET(QT_UIC_EXECUTABLE uic)
	SET(QT_MOC_EXECUTABLE moc)
	SET(QT_RCC_EXECUTABLE rcc)
endif(XMINGW)
endif(QT4_FOUND AND MINGW)

# do config checks
include(ConfigureChecks.cmake)

# create config.h
CONFIGURE_FILE(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
# #
 include_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/avidemux ${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries)
 include_directories(${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_utilities/ ${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_lavutil)
 include_directories(${CMAKE_CURRENT_BINARY_DIR}/avidemux/ADM_libraries/ADM_lavcodec  )
#**********************************************
# Some CFLAGS
#**********************************************
add_definitions (-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
add_definitions( -DHAVE_CONFIG_H )

ADD_SUBDIRECTORY(avidemux)

###############################
# Summary
###############################
include(conf_summary)
