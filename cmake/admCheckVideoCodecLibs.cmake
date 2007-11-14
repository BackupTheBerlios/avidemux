########################################
# x264
########################################
OPTION(X264 "" ON)

MESSAGE(STATUS "Checking for x264")
MESSAGE(STATUS "*****************")

IF (X264)
	FIND_HEADER_AND_LIB(X264 x264.h x264 x264_encoder_open)
	PRINT_LIBRARY_INFO("x264" X264_FOUND "${X264_INCLUDE_DIR}" "${X264_LIBRARY_DIR}")

	IF (X264_FOUND)
		SET(USE_X264 1)
	ENDIF (X264_FOUND)
ELSE (X264)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (X264)

MESSAGE("")

########################################
# Xvid
########################################
OPTION(XVID "" ON)

MESSAGE(STATUS "Checking for Xvid")
MESSAGE(STATUS "*****************")

IF (XVID)
	FIND_HEADER_AND_LIB(XVID xvid.h xvidcore xvid_plugin_single)
	PRINT_LIBRARY_INFO("Xvid" XVID_FOUND "${XVID_INCLUDE_DIR}" "${XVID_LIBRARY_DIR}")
	
	IF (XVID_FOUND)
		SET(USE_XVID_4 1)
	ENDIF (XVID_FOUND)
ELSE (XVID)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (XVID)

MESSAGE("")