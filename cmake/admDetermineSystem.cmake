# Determine CPU and Operating System for GCC
#  ADM_BSD_FAMILY        - BSD family operating system was detected
#  ADM_CPU_ALTIVEC       - PowerPC CPU with AltiVec architecture was detected
#  ADM_CPU_PPC           - PowerPC CPU architecture was detected
#  ADM_CPU_X86           - x86 CPU architecture was detected
#  ADM_CPU_X86_64        - x86-64 CPU architecture was detected
#  CMAKE_WORDS_BIGENDIAN - big endian CPU detected

INCLUDE(CMakeDetermineSystem)
INCLUDE(TestBigEndian)

MESSAGE(STATUS "Checking GCC support")
MESSAGE(STATUS "********************")

########################################
# Check OS support
########################################
ADM_COMPILE(bsd_check.cpp "" "" "" BSD_SUPPORTED outputBsdTest)

IF (BSD_SUPPORTED)
	SET(ADM_BSD_FAMILY 1)

	MESSAGE(STATUS "Check if GCC is BSD family - Yes")
ELSE (BSD_SUPPORTED)
	MESSAGE(STATUS "Check if GCC is BSD family - No")

	IF (VERBOSE)
		MESSAGE("Error Message: ${outputBsdTest}")
	ENDIF (VERBOSE)
ENDIF (BSD_SUPPORTED)

########################################
# Check CPU support
########################################
# x86_64
ADM_COMPILE(cpu_x86-64_check.cpp "" "" "" X86_64_SUPPORTED outputx86_64Test)

IF (X86_64_SUPPORTED)
	SET(ADM_CPU_X86_64 1)

	MESSAGE(STATUS "Check if GCC is x86 64-bit - Yes")
ELSE (X86_64_SUPPORTED)
	MESSAGE(STATUS "Check if GCC is x86 64-bit - No")

	IF (VERBOSE)
		MESSAGE("Error Message: ${outputx86_64Test}")
	ENDIF (VERBOSE)

	# x86
	ADM_COMPILE(cpu_x86_check.cpp "" "" "" X86_SUPPORTED outputX86Test)

	IF (X86_SUPPORTED)
		SET(ADM_CPU_X86 1)

		MESSAGE(STATUS "Check if GCC is x86 32-bit - Yes")
	ELSE (X86_SUPPORTED)
		MESSAGE(STATUS "Check if GCC is x86 32-bit - No")

		IF (VERBOSE)
			MESSAGE("Error Message: ${outputX86Test}")
		ENDIF (VERBOSE)

		# PowerPC
		ADM_COMPILE(cpu_ppc_check.cpp "" "" "" POWERPC_SUPPORTED outputPowerPcTest)

		IF (POWERPC_SUPPORTED)
			SET(ADM_CPU_PPC 1)

			SET(ADM_ALTIVEC_FLAGS "-mabi=altivec -maltivec")

			IF (APPLE)
				SET(ADM_ALTIVEC_FLAGS "${ADM_ALTIVEC_FLAGS} -faltivec")
			ENDIF (APPLE)

			MESSAGE(STATUS "Check if GCC is PowerPC - Yes")	
			ADM_COMPILE(cpu_altivec_check.cpp "${ADM_ALTIVEC_FLAGS}" "" "" ALTIVEC_SUPPORTED outputAltivecTest)

			IF (ALTIVEC_SUPPORTED)
				SET(ADM_CPU_ALTIVEC 1)
				MESSAGE(STATUS "Check if GCC is AltiVec - Yes")
			ELSE (ALTIVEC_SUPPORTED)
				MESSAGE(STATUS "Check if GCC is AltiVec - No")

				IF (VERBOSE)
					MESSAGE("Error Message: ${outputAltivecTest}")
				ENDIF (VERBOSE)
			ENDIF (ALTIVEC_SUPPORTED)
		ELSE (POWERPC_SUPPORTED)
			MESSAGE(STATUS "Check if GCC is PowerPC - No")

			IF (VERBOSE)
				MESSAGE("Error Message: ${outputPowerPcTest}")
			ENDIF (VERBOSE)
		ENDIF (POWERPC_SUPPORTED)
	ENDIF (X86_SUPPORTED)
ENDIF (X86_64_SUPPORTED)

TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

IF (NOT ADM_CPU_X86 AND NOT ADM_CPU_X86_64 AND NOT ADM_CPU_PPC)
	MESSAGE(FATAL_ERROR "CPU not supported")
ENDIF (NOT ADM_CPU_X86 AND NOT ADM_CPU_X86_64 AND NOT ADM_CPU_PPC)