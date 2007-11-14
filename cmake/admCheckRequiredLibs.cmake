########################################
# pkg-config
########################################
MESSAGE(STATUS "Checking for pkg-config")
MESSAGE(STATUS "***********************")

FIND_PACKAGE(PkgConfig)

IF (NOT PKG_CONFIG_FOUND)
	MESSAGE(FATAL_ERROR "Could not find pkg-config")
ENDIF (NOT PKG_CONFIG_FOUND)

MESSAGE(STATUS "Found pkg-config")

IF (VERBOSE)
	MESSAGE(STATUS "Path: ${PKG_CONFIG_EXECUTABLE}")
ENDIF (VERBOSE)

MESSAGE("")

########################################
# libiconv
########################################
MESSAGE(STATUS "Checking for libiconv")
MESSAGE(STATUS "*********************")

FIND_HEADER_AND_LIB(LIBICONV iconv.h iconv)
PRINT_LIBRARY_INFO("libiconv" LIBICONV_FOUND "${LIBICONV_INCLUDE_DIR}" "${LIBICONV_LIBRARY_DIR}")

IF (NOT LIBICONV_INCLUDE_DIR)
	MESSAGE(FATAL "iconv.h not found")
ENDIF (NOT LIBICONV_INCLUDE_DIR)

IF (LIBICONV_LIBRARY_DIR)
	MESSAGE(STATUS "libiconv is probably needed")
ELSE (LIBICONV_LIBRARY_DIR)
	MESSAGE(STATUS "libiconv is probably not needed")
ENDIF (LIBICONV_LIBRARY_DIR)

CHECK_CFLAGS_REQUIRED(iconv_check.cpp "-DICONV_NEED_CONST" "${LIBICONV_INCLUDE_DIR}" "${LIBICONV_LIBRARY_DIR}" LIBICONV_CFLAGS_REQUIRED)

IF (LIBICONV_CFLAGS_REQUIRED)
	SET(LIBICONV_DEFINITIONS "-DICONV_NEED_CONST")
	SET(ICONV_NEED_CONST 1)
ENDIF(LIBICONV_CFLAGS_REQUIRED)

MESSAGE("")

########################################
# Libxml2
########################################
MESSAGE(STATUS "Checking for Libxml2")
MESSAGE(STATUS "********************")

SET(LibXml2_FIND_QUIETLY TRUE)

FIND_PACKAGE(LibXml2)
PRINT_LIBRARY_INFO("Libxml2" LIBXML2_FOUND "${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS}" "${LIBXML2_LIBRARIES}" FATAL_ERROR)

SET(USE_LIBXML2 1)
MESSAGE("")

########################################
# libpng
########################################
MESSAGE(STATUS "Checking for libpng")
MESSAGE(STATUS "*******************")

FIND_PACKAGE(png)
PRINT_LIBRARY_INFO("libpng" PNG_FOUND "${PNG_INCLUDE_DIR} ${PNG_DEFINITIONS}" "${PNG_LIBRARIES}" FATAL_ERROR)

SET(USE_PNG 1)
MESSAGE("")