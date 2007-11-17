########################################
# ALSA
########################################
IF (ADM_OS_LINUX)
	OPTION(ALSA "" ON)

	MESSAGE(STATUS "Checking for ALSA")
	MESSAGE(STATUS "*****************")

	IF (ALSA)
		FIND_PACKAGE(Alsa)

		IF (ALSA_FOUND)
			ALSA_VERSION_STRING(alsaVersion)

			MESSAGE(STATUS "  version: ${alsaVersion}")
			PRINT_LIBRARY_INFO("ALSA" ALSA_FOUND "${_ALSA_INCLUDE_DIR}" "${ASOUND_LIBRARY}")

			SET(ALSA_SUPPORT 1)
		ENDIF (ALSA_FOUND)
	ELSE (ALSA)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (ALSA)

	MESSAGE("")
ELSE (ADM_OS_LINUX)
	SET(ALSA_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX)

########################################
# aRts
########################################
IF (ADM_OS_LINUX)
	OPTION(ARTS "" ON)
	
	MESSAGE(STATUS "Checking for aRts")
	MESSAGE(STATUS "*****************")

	IF (ARTS)
		FIND_PACKAGE(Arts)
		PRINT_LIBRARY_INFO("aRts" ARTS_FOUND "${ARTS_CFLAGS}" "${ARTS_LDFLAGS}")
		
		IF (ARTS_FOUND)
			SET(USE_ARTS 1)
		ENDIF (ARTS_FOUND)
	ELSE (ARTS)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (ARTS)
		
	MESSAGE("")
ELSE (ADM_OS_LINUX)
	SET(ARTS_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX)

########################################
# ESD
########################################
IF (ADM_OS_LINUX)
	OPTION(ESD "" ON)

	MESSAGE(STATUS "Checking for ESD")
	MESSAGE(STATUS "****************")

	IF (ESD)
		FIND_HEADER_AND_LIB(ESD esd.h esd esd_close)
		PRINT_LIBRARY_INFO("ESD" ESD_FOUND "${ESD_INCLUDE_DIR}" "${ESD_LIBRARY_DIR}")

		IF (ESD_FOUND)
			SET(USE_ESD 1)
		ENDIF (ESD_FOUND)
	ELSE (ESD)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (ESD)

	MESSAGE("")
ELSE (ADM_OS_LINUX)
	SET(ESD_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX)

########################################
# JACK
########################################
IF (ADM_OS_LINUX OR ADM_OS_DARWIN)
	OPTION(JACK "" ON)

	MESSAGE(STATUS "Checking for JACK")
	MESSAGE(STATUS "*****************")

	IF (JACK)
		FIND_HEADER_AND_LIB(JACK jack/jack.h jack jack_client_close)
		PRINT_LIBRARY_INFO("JACK" JACK_FOUND "${JACK_INCLUDE_DIR}" "${JACK_LIBRARY_DIR}")

		IF (JACK_FOUND)
			SET(USE_JACK 1)
		ENDIF (JACK_FOUND)
	ELSE (JACK)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (JACK)
	
	MESSAGE("")
ELSE (ADM_OS_LINUX OR ADM_OS_DARWIN)
	SET(JACK_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX OR ADM_OS_DARWIN)

########################################
# Secret Rabbit Code
########################################
IF (JACK_FOUND)
	MESSAGE(STATUS "Checking for Secret Rabbit Code")
	MESSAGE(STATUS "*******************************")

	FIND_HEADER_AND_LIB(SRC samplerate.h samplerate src_get_version)
	PRINT_LIBRARY_INFO("Secret Rabbit Code" SRC_FOUND "${SRC_INCLUDE_DIR}" "${SRC_LIBRARY_DIR}")

	IF (SRC_FOUND)
		SET(USE_SRC 1)
	ENDIF (SRC_FOUND)
	
	MESSAGE("")
ENDIF (JACK_FOUND)

########################################
# OSS
########################################
IF (ADM_OS_LINUX)
	OPTION(OSS "" ON)
	
	MESSAGE(STATUS "Checking for OSS")
	MESSAGE(STATUS "****************")
	
	IF (OSS)
		FIND_HEADER_AND_LIB(OSS sys/soundcard.h)
		PRINT_LIBRARY_INFO("OSS" OSS_FOUND "${OSS_INCLUDE_DIR}" "")

		IF (OSS_FOUND)
			SET(OSS_SUPPORT 1)
		ENDIF (OSS_FOUND)
	ELSE (OSS)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (OSS)
	
	MESSAGE("")
ELSE (ADM_OS_LINUX)
	SET(OSS_CAPABLE FALSE)
ENDIF (ADM_OS_LINUX)