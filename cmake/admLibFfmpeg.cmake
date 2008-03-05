# This is a hack. FFmpeg libraries should have one config file.
# This will be easier if automake is ever dropped.

MACRO (GET_FFMPEG_CFLAGS varToSet)
	IF (ADM_CPU_X86_32)
		SET(${varToSet} "-DARCH_X86 -DARCH_X86_32")
	ELSEIF (ADM_CPU_X86_64)
		SET(${varToSet} "-DARCH_X86 -DARCH_X86_64")
	ELSEIF (ADM_CPU_PPC)
		SET(${varToSet} "-DARCH_POWERPC")

		IF (ADM_CPU_ALTIVEC)
			SET(${varToSet} "${${varToSet}} -DHAVE_ALTIVEC")

			IF (NOT APPLE)
				SET(${varToSet} "${${varToSet}} -DHAVE_ALTIVEC_H")
			ENDIF (NOT APPLE)
		ENDIF (ADM_CPU_ALTIVEC)
	ENDIF (ADM_CPU_X86_32)

	IF (ADM_CPU_SSSE3)
		SET(${varToSet} "${${varToSet}} -DHAVE_SSSE3")
	ENDIF (ADM_CPU_SSSE3)

	IF (ADM_BIG_ENDIAN)
		SET(${varToSet} "${${varToSet}} -DWORDS_BIGENDIAN")
	ENDIF (ADM_BIG_ENDIAN)
ENDMACRO (GET_FFMPEG_CFLAGS)