# Small Macro to check head & lib and set output variable
# all in one line, cmake is cool :)
MACRO (ADM_COMPILE _file _cflags _lib _varToSet)
TRY_COMPILE(${_varToSet}
      ${CMAKE_BINARY_DIR}
      ${CMAKE_SOURCE_DIR}/cmake_compile_check/${_file}
      CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${_lib}
      COMPILE_DEFINITIONS  ${_cflags}
      
      OUTPUT_VARIABLE MYOUTPUT) 
#MESSAGE( ./compile.log "Compile:${MYOUTPUT}")
ENDMACRO (ADM_COMPILE _file _cflags _varToSet)
#########################################################
MACRO (ADM_COMPILE_WITH_WITHOUT _file _cflags _lib _varToSet)

ADM_COMPILE( ${_file} ${_cflags} ${_lib} CWITH)
ADM_COMPILE( ${_file} "" ${_lib}  CWITHOUT)

if(CWITH AND NOT CWITHOUT)
  SET(${_varToSet}  1)
  MESSAGE (STATUS "<${_cflags}> needed")
else(CWITH AND NOT CWITHOUT)
  if(NOT CWITH AND CWITHOUT)
    MESSAGE (STATUS "<${_cflags}> Not needed")
  else(NOT CWITH AND CWITHOUT)
    MESSAGE(FATAL "Inconsistent compiler output with:${CWITH}, without:${CWITHOUT}")
  endif(NOT CWITH AND CWITHOUT)
endif(CWITH AND NOT CWITHOUT)
ENDMACRO (ADM_COMPILE_WITH_WITHOUT _file _cflags _varToSet)




