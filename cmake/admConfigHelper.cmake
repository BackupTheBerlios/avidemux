INCLUDE(CheckLibraryExists)		# required for FIND_HEADER_AND_LIB macro

MACRO(PRINT_LIBRARY_INFO libraryName libraryDetected compilerFlags linkerFlags)
	IF (${libraryDetected})
		MESSAGE(STATUS "Found ${libraryName}")
		
		IF (VERBOSE)
			MESSAGE(STATUS "Compiler Flags: ${compilerFlags}")
			MESSAGE(STATUS "Linker Flags  : ${linkerFlags}")
		ENDIF (VERBOSE)
	ELSE (${libraryDetected})
		MESSAGE(${ARGV4} "Could not find ${libraryName}")
	ENDIF (${libraryDetected})
ENDMACRO(PRINT_LIBRARY_INFO)


MACRO(SDLify _source)
	if(SDL_FOUND)
        SET_SOURCE_FILES_PROPERTIES(${_source} PROPERTIES COMPILE_FLAGS "-I${SDL_INCLUDE_DIR}")
	endif(SDL_FOUND)
ENDMACRO(SDLify)

# ARGV2 = library to check
# ARGV3 = function to check
# ARVG4 = CMAKE_REQUIRED_FLAGS
MACRO(FIND_HEADER_AND_LIB prefix headerFile)
	IF (NOT DEFINED ${prefix}_FOUND)
		SET(${prefix}_FOUND 0 CACHE INTERNAL "")
		SET(PROCEED 1)
		
		IF (NOT ${headerFile} STREQUAL "")
			FIND_PATH(${prefix}_INCLUDE_DIR ${headerFile})
			MARK_AS_ADVANCED(${prefix}_INCLUDE_DIR)
		
			IF (${prefix}_INCLUDE_DIR)
				MESSAGE(STATUS "Found ${headerFile}")
			ELSE (${prefix}_INCLUDE_DIR)
				SET(PROCEED 0)
				MESSAGE("Could not find ${headerFile}")
			ENDIF (${prefix}_INCLUDE_DIR)
		ENDIF (NOT ${headerFile} STREQUAL "")
		
		IF (PROCEED AND NOT ${ARGV2} STREQUAL "")
			FIND_LIBRARY(${prefix}_LIBRARY_DIR ${ARGV2})
			MARK_AS_ADVANCED(${prefix}_LIBRARY_DIR)

			IF (${prefix}_LIBRARY_DIR)
				MESSAGE(STATUS "Found ${ARGV2} library")
				
				IF (NOT ${ARGV3} STREQUAL "")
					SET(CMAKE_REQUIRED_FLAGS_BACKUP "${CMAKE_REQUIRED_FLAGS}")
					SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${ARGV4}")

					CHECK_LIBRARY_EXISTS(${ARGV2} ${ARGV3} ${prefix}_LIBRARY_DIR ${prefix}_FUNCTION_FOUND)

					SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_BACKUP}")

					IF (${prefix}_FUNCTION_FOUND)
						SET(${prefix}_FOUND 1 CACHE INTERNAL "")
					ENDIF (${prefix}_FUNCTION_FOUND)
				ELSE (NOT ${ARGV3} STREQUAL "")
					SET(${prefix}_FOUND 1 CACHE INTERNAL "")
				ENDIF (NOT ${ARGV3} STREQUAL "")
			ELSE (${prefix}_LIBRARY_DIR)
				MESSAGE(STATUS "Cound not find ${ARGV2} library")
			ENDIF (${prefix}_LIBRARY_DIR)
		ELSE (PROCEED AND NOT ${ARGV2} STREQUAL "")
			SET(${prefix}_FOUND 1 CACHE INTERNAL "")
		ENDIF (PROCEED AND NOT ${ARGV2} STREQUAL "")
	ENDIF (NOT DEFINED ${prefix}_FOUND)
ENDMACRO(FIND_HEADER_AND_LIB)


MACRO (ADM_COMPILE _file _def _include _lib _varToSet _output)
	IF (NOT DEFINED ${_varToSet}_COMPILED)
		SET(${_varToSet}_COMPILED 1 CACHE INTERNAL "")
		
		TRY_COMPILE(${_varToSet}
			  ${CMAKE_BINARY_DIR}
			  ${CMAKE_SOURCE_DIR}/cmake_compile_check/${_file}
			  CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${_include} -DCOMPILE_DEFINITIONS=${_def} -DLINK_LIBRARIES=${_lib}
			  COMPILE_DEFINITIONS "${_cflags}"
			  OUTPUT_VARIABLE ${_output})
	ENDIF (NOT DEFINED ${_varToSet}_COMPILED)
ENDMACRO (ADM_COMPILE)


MACRO (CHECK_CFLAGS_REQUIRED _file _cflags _include _lib _varToSet)
	IF (NOT DEFINED ${_varToSet}_CFLAGS_CHECKED)
		SET(${_varToSet}_CFLAGS_CHECKED 1 CACHE INTERNAL "")
		
		ADM_COMPILE(${_file} ${_cflags} ${_include} ${_lib} ${_varToSet}_COMPILE_WITH logwith)
		ADM_COMPILE(${_file} "" ${_include} ${_lib} ${_varToSet}_COMPILE_WITHOUT logwithout)

		IF (${_varToSet}_COMPILE_WITH AND NOT ${_varToSet}_COMPILE_WITHOUT)
			SET(${_varToSet} 1 CACHE INTERNAL "")
			
			IF (VERBOSE)
				MESSAGE(STATUS "(${_cflags}) required")
			ENDIF (VERBOSE)
		ELSE (${_varToSet}_COMPILE_WITH AND NOT ${_varToSet}_COMPILE_WITHOUT)
			IF (NOT ${_varToSet}_COMPILE_WITH AND ${_varToSet}_COMPILE_WITHOUT)
				IF (VERBOSE)
					MESSAGE (STATUS "(${_cflags}) not required")
				ENDIF (VERBOSE)
			ELSE (NOT ${_varToSet}_COMPILE_WITH AND ${_varToSet}_COMPILE_WITHOUT)
				MESSAGE(STATUS "Inconsistent compiler output with: ${${_varToSet}_COMPILE_WITH}, without: ${${_varToSet}_COMPILE_WITHOUT}")
				MESSAGE(STATUS "${logwith}")
				MESSAGE(STATUS "")
				MESSAGE(STATUS "${logwithout}")
			endif(NOT ${_varToSet}_COMPILE_WITH AND ${_varToSet}_COMPILE_WITHOUT)
		ENDIF (${_varToSet}_COMPILE_WITH AND NOT ${_varToSet}_COMPILE_WITHOUT)
	ENDIF (NOT DEFINED ${_varToSet}_CFLAGS_CHECKED)
ENDMACRO (CHECK_CFLAGS_REQUIRED)

MACRO (append_flags _flags _varToAppend)
	if (_flags)
		set(_flags "${_flags} ${_varToAppend}")
	else (_flags)
		set(_flags "${_varToAppend}")
	endif (_flags)
ENDMACRO (append_flags)

MACRO (add_link_flags _target _flg)
	GET_TARGET_PROPERTY(_flags ${_target} LINK_FLAGS)

	append_flags("${_flags}" "${_flg}")

	SET_TARGET_PROPERTIES(${_target} PROPERTIES LINK_FLAGS "${_flags}")
ENDMACRO (add_link_flags)

MACRO (add_source_compile_flags _target _flg)
	GET_SOURCE_FILE_PROPERTY(_flags ${_target} COMPILE_FLAGS)

	append_flags("${_flags}" "${_flg}")
   
	SET_SOURCE_FILES_PROPERTIES(${_target} PROPERTIES COMPILE_FLAGS "${_flags}")   
ENDMACRO (add_source_compile_flags)

MACRO (add_target_compile_flags _target _flg)
	GET_TARGET_PROPERTY(_flags ${_target} COMPILE_FLAGS)

	append_flags("${_flags}" "${_flg}")
   
	SET_TARGET_PROPERTIES(${_target} PROPERTIES COMPILE_FLAGS "${_flags}")
ENDMACRO (add_target_compile_flags)