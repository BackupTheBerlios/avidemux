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
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (XVID)

MESSAGE("")