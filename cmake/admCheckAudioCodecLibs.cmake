########################################
# Aften
########################################
OPTION(AFTEN "" ON)

MESSAGE(STATUS "Checking for Aften")
MESSAGE(STATUS "******************")

IF (AFTEN)
	FIND_HEADER_AND_LIB(AFTEN aften/aften.h aften aften_encode_init)
	PRINT_LIBRARY_INFO("Aften" AFTEN_FOUND "${AFTEN_INCLUDE_DIR}" "${AFTEN_LIBRARY_DIR}")

	IF (AFTEN_FOUND)
		SET(USE_AFTEN 1)
		
		IF (NOT DEFINED AFTEN_TEST_RUN_RESULT)
			TRY_RUN(AFTEN_TEST_RUN_RESULT
				AFTEN_TEST_COMPILE_RESULT
				${CMAKE_BINARY_DIR}
				"${CMAKE_SOURCE_DIR}/cmake_compile_check/aften_check.cpp"
				CMAKE_FLAGS -DLINK_LIBRARIES=${AFTEN_LIBRARY_DIR})
		ENDIF (NOT DEFINED AFTEN_TEST_RUN_RESULT)

		IF (AFTEN_TEST_RUN_RESULT EQUAL 8)
			MESSAGE(STATUS "  version: 0.0.8")
			SET(USE_AFTEN_08 1)
		ELSEIF (AFTEN_TEST_RUN_RESULT EQUAL 7)
			MESSAGE(STATUS "  version: 0.07")
			SET(USE_AFTEN_07 1)
		ELSE (AFTEN_TEST_RUN_RESULT EQUAL 8)
			MESSAGE(STATUS "Warning: Unable to determine Aften version - support for Aften will be turned off")
			SET(USE_AFTEN 0)
		ENDIF (AFTEN_TEST_RUN_RESULT EQUAL 8)
	ENDIF (AFTEN_FOUND)
ELSE (AFTEN)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (AFTEN)

MESSAGE("")

########################################
# amrnb
########################################
OPTION(AMRNB "" ON)

MESSAGE(STATUS "Checking for amrnb")
MESSAGE(STATUS "******************")

IF (AMRNB)
	IF (LATE_BINDING)
		FIND_HEADER_AND_LIB(AMRNB amrnb/interf_dec.h)
	ELSE (LATE_BINDING)
		FIND_HEADER_AND_LIB(AMRNB amrnb/interf_dec.h amrnb GP3Decoder_Interface_Decode)
	ENDIF (LATE_BINDING)

	PRINT_LIBRARY_INFO("amrnb" AMRNB_FOUND "${AMRNB_INCLUDE_DIR}" "${AMRNB_LIBRARY_DIR}")
	
	IF (AMRNB_FOUND)
		SET(USE_AMR_NB 1)
		SET(CONFIG_AMR_NB 1)
	ENDIF (AMRNB_FOUND)
ELSE (AMRNB)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (AMRNB)

MESSAGE("")

########################################
# LAME
########################################
OPTION(LAME "" ON)

MESSAGE(STATUS "Checking for LAME")
MESSAGE(STATUS "*****************")

IF (LAME)
	FIND_HEADER_AND_LIB(LAME lame/lame.h mp3lame lame_init)
	PRINT_LIBRARY_INFO("LAME" LAME_FOUND "${LAME_INCLUDE_DIR}" "${LAME_LIBRARY_DIR}")
	
	IF (LAME_FOUND)
		SET(HAVE_LIBMP3LAME 1)
	ENDIF (LAME_FOUND)
ELSE (LAME)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (LAME)

MESSAGE("")

########################################
# libdca
########################################
OPTION(LIBDCA "" ON)

MESSAGE(STATUS "Checking for libdca")
MESSAGE(STATUS "*******************")

IF (LIBDCA)
	IF (USE_LATE_BINDING)
		FIND_HEADER_AND_LIB(LIBDCA dts.h)
	ELSE (USE_LATE_BINDING)
		FIND_HEADER_AND_LIB(LIBDCA dts.h dts dts_init)
	ENDIF (USE_LATE_BINDING)
	
	PRINT_LIBRARY_INFO("libdca" LIBDCA_FOUND "${LIBDCA_INCLUDE_DIR}" "${LIBDCA_LIBRARY_DIR}")

	IF (LIBDCA_FOUND)
		SET(USE_LIBDCA 1)
	ENDIF (LIBDCA_FOUND)
ELSE (LIBDCA)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (LIBDCA)

MESSAGE("")

########################################
# FAAC
########################################
OPTION(FAAC "" ON)

MESSAGE(STATUS "Checking for FAAC")
MESSAGE(STATUS "*****************")

IF (FAAC)
	FIND_HEADER_AND_LIB(FAAC faac.h faac faacEncClose)
	PRINT_LIBRARY_INFO("FAAC" FAAC_FOUND "${FAAC_INCLUDE_DIR}" "${FAAC_LIBRARY_DIR}")
	
	IF (FAAC_FOUND)
		SET(USE_FAAC 1)
	ENDIF (FAAC_FOUND)
ELSE (FAAC)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (FAAC)

MESSAGE("")

########################################
# FAAD
########################################
OPTION (FAAD "" ON)

MESSAGE(STATUS "Checking for FAAD")
MESSAGE(STATUS "*****************")

IF (FAAD)
	FIND_HEADER_AND_LIB(FAAD faad.h faad)

	IF (FAAD_FOUND)
		CHECK_LIBRARY_EXISTS(faad faacDecInit FAAD_LIBRARY_DIR FAACDEC_FUNCTION_FOUND)

		IF (NOT FAACDEC_FUNCTION_FOUND)
			CHECK_LIBRARY_EXISTS(faad NeAACDecInit FAAD_LIBRARY_DIR NEAACDEC_FUNCTION_FOUND)

			IF (NOT NEAACDEC_FUNCTION_FOUND)
				SET(FAAD_FOUND 0)
			ENDIF (NOT NEAACDEC_FUNCTION_FOUND)
		ENDIF (NOT FAACDEC_FUNCTION_FOUND)
	ENDIF (FAAD_FOUND)

	PRINT_LIBRARY_INFO("FAAD" FAAD_FOUND "${FAAD_INCLUDE_DIR}" "${FAAD_LIBRARY_DIR}")

	IF (FAAD_FOUND)
		SET(USE_FAAD 1)

		CHECK_CFLAGS_REQUIRED(faad_check.cpp "-DOLD_FAAD_PROTO" "${FAAD_INCLUDE_DIR}" "${FAAD_LIBRARY_DIR}" FAAD_CFLAGS_REQUIRED)
		
		IF (FAAD_CFLAGS_REQUIRED)
			SET(OLD_FAAD_PROTO 1)
		ENDIF (FAAD_CFLAGS_REQUIRED)
	ENDIF (FAAD_FOUND)
ELSE (FAAD)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (FAAD)

MESSAGE("")

########################################
# Vorbis
########################################
OPTION(VORBIS "" ON)

MESSAGE(STATUS "Checking for Vorbis")
MESSAGE(STATUS "*******************")

IF (VORBIS)
	FIND_HEADER_AND_LIB(VORBIS vorbis/vorbisenc.h vorbis vorbis_info_init)
	FIND_HEADER_AND_LIB(VORBISENC "" vorbisenc vorbis_encode_init)
	
	IF (VORBIS_FOUND AND VORBISENC_FOUND)
		SET(USE_VORBIS 1)
	ELSE (VORBIS_FOUND AND VORBISENC_FOUND)
		SET(VORBIS_FOUND 0 CACHE INTERNAL "")
	ENDIF (VORBIS_FOUND AND VORBISENC_FOUND)

	PRINT_LIBRARY_INFO("Vorbis" VORBIS_FOUND "${VORBIS_INCLUDE_DIR}" "${VORBIS_LIBRARY_DIR} ${VORBISENC_LIBRARY_DIR}")
ELSE (VORBIS)
	MESSAGE(STATUS "<disabled per request>")
ENDIF (VORBIS)

MESSAGE ("")