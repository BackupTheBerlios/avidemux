MACRO(checkAmrnb)
	OPTION(AMRNB "" ON)

	MESSAGE(STATUS "Checking for amrnb")
	MESSAGE(STATUS "******************")

	IF (AMRNB)
		FIND_HEADER_AND_LIB(AMRNB amrnb/interf_dec.h amrnb GP3Decoder_Interface_Decode)
		PRINT_LIBRARY_INFO("amrnb" AMRNB_FOUND "${AMRNB_INCLUDE_DIR}" "${AMRNB_LIBRARY_DIR}")
	ELSE (AMRNB)
		MESSAGE("${MSG_DISABLE_OPTION}")
	ENDIF (AMRNB)
	
	SET(ENV{ADM_HAVE_AMRNB} ${AMRNB_FOUND})

	MESSAGE("")
ENDMACRO(checkAmrnb)