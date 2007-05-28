MACRO (ADD_ADM_LIB _libName _father)
GET_TARGET_PROPERTY(Foo ${_libName} LOCATION)
STRING(REGEX REPLACE "${_father}.${_libName}" "${_father}" NewFoo "${Foo}" )
#SET_TARGET_PROPERTIES(${_libName} PROPERTIES 
        #PREFIX "../lib"
        #)

#MESSAGE ("LOCATION:"${Foo}"->"${NewFoo})
#GET_TARGET_PROPERTY(Foo ${_libName} LOCATION)
#MESSAGE ("AFTER LOCATION:"${Foo}"->"${NewFoo})
ENDMACRO (ADD_ADM_LIB _libName)

