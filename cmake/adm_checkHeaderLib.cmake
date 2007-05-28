# Small Macro to check head & lib and set output variable
# all in one line, cmake is cool :)
MACRO (ADM_CHECK_HL _banner _includeName _libName _libFunc _varToSet)
MESSAGE(STATUS "<Checking for ${_banner}>")
CHECK_INCLUDE_FILES(${_includeName} HAVE_IT_H)
#MESSAGE("OUT ${HAVE_IT_H}")
if(HAVE_IT_H)
  CHECK_LIBRARY_EXISTS(${_libName} "" ${_libFunc} HAVE_IT_H)
  if(HAVE_IT_H)
        SET(${_varToSet}  1)
  else(HAVE_IT_H)
        MESSAGE("Not lib found")
  endif(HAVE_IT_H)
else(HAVE_IT_H)
  MESSAGE("Header not found(${_includeName})")
endif(HAVE_IT_H)

if(${_varToSet})
   MESSAGE(STATUS "Found")
else(${_varToSet})
     MESSAGE(STATUS "Not Found")
endif(${_varToSet})

ENDMACRO (ADM_CHECK_HL _includeName _libName _libFunc _varToSet)


