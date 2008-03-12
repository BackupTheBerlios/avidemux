# Cygwin and MSYS require escaping but standalone MinGW doesn't.
IF (MINGW AND NOT MSYS)
	SET(VERBATIM "")
ELSE (MINGW AND NOT MSYS)
	SET(VERBATIM "VERBATIM")
ENDIF(MINGW AND NOT MSYS)

##############################################################"
# FOR AQ
# Q_foo.cpp and foo.ui => AQ_foo.cpp, ui_foo.h and Q_foo.moc
##############################################################"
MACRO(Q_UIMOCIFY _source)
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/AQ_${_source}.cpp
		COMMAND  ${QT_UIC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_source}.ui -o ui_${_source}.h
		COMMAND  ${QT_MOC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Q_${_source}.cpp -o Q_${_source}.moc
		COMMAND  echo "#include \"Q_${_source}.cpp\"" > AQ_${_source}.cpp
		COMMAND  echo "#include \"Q_${_source}.moc\"" >> AQ_${_source}.cpp
		DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/${_source}.ui  ${CMAKE_CURRENT_SOURCE_DIR}/Q_${_source}.cpp
		${VERBATIM})
ENDMACRO(Q_UIMOCIFY _source)

##############################################################"
# FOR 
# foo.qrc->foo_rsc.cpp
##############################################################"
# $(RCC)  $< -o $@   -name filter
MACRO(Q_RCC _source)
	ADD_CUSTOM_COMMAND(OUTPUT   ${CMAKE_CURRENT_BINARY_DIR}/${_source}_rsc.cpp
		COMMAND  ${QT_RCC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_source}.qrc -o ${_source}_rsc.cpp -name ${_source}
		DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/${_source}.qrc
		${VERBATIM})
ENDMACRO(Q_RCC _source)

##############################################################"
# FOR AT
# T_foo.cpp i => AT_foo.cpp and T_foo.moc
##############################################################"
MACRO(Q_MOCIFY _source)
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/AT_${_source}.cpp
		COMMAND  ${QT_MOC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/T_${_source}.cpp -o T_${_source}.moc 
		COMMAND  echo "#include \"T_${_source}.cpp\"" > AT_${_source}.cpp
		COMMAND  echo "#include \"T_${_source}.moc\"" >> AT_${_source}.cpp
		DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/T_${_source}.cpp
		${VERBATIM})
ENDMACRO(Q_MOCIFY _source)

# Add to our Targets..
FOREACH(_current ${${ADM_LIB}_Q})
	Q_UIMOCIFY(${_current})
	SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/AQ_${_current}.cpp)
ENDFOREACH(_current ${${ADM_LIB}_Q})

FOREACH(_current ${${ADM_LIB}_T})
	Q_MOCIFY(${_current})
	SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/AT_${_current}.cpp)
ENDFOREACH(_current ${${ADM_LIB}_T})

ADD_ADM_LIB_QT4_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_QT4_TARGET(${ADM_LIB} -I${QT_INCLUDE_DIR} -I${QT_QTCORE_INCLUDE_DIR} -I${QT_QTGUI_INCLUDE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_BINARY_DIR} ${QT_DEFINITIONS})