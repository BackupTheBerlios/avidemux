# a small macro to create mo files out of po's
# Taken from sim-im.org

MACRO(FIND_MSGFMT)
    IF(NOT MSGFMT_EXECUTABLE)
        IF(NOT MSGFMT_NOT_FOUND)
            SET(MSGFMT_NAME "msgfmt")
            FIND_PROGRAM(MSGFMT_EXECUTABLE ${MSGFMT_NAME})

            IF (NOT MSGFMT_EXECUTABLE)
              MESSAGE(FATAL_ERROR "${MSGFMT_NAME} not found - po files can't be processed")
              SET(MSGFMT_NOT_FOUND "1")     # to avoid double checking in one cmake run
            ENDIF (NOT MSGFMT_EXECUTABLE)

            MARK_AS_ADVANCED(MSGFMT_EXECUTABLE)
        ENDIF(NOT MSGFMT_NOT_FOUND)
    ENDIF(NOT MSGFMT_EXECUTABLE)
ENDMACRO(FIND_MSGFMT)

MACRO(COMPILE_PO_FILES po_subdir _sources)
    FIND_MSGFMT()
    SET(_basename avidemux2)
	
    IF(MSGFMT_EXECUTABLE)
        FILE(GLOB po_files ${po_subdir}/*.po)

        FOREACH(po_input ${po_files})

            GET_FILENAME_COMPONENT(_in       ${po_input} ABSOLUTE)
            GET_FILENAME_COMPONENT(_basename ${po_input} NAME_WE)

            IF(WIN32)
                FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/po)
                GET_FILENAME_COMPONENT(_out ${CMAKE_BINARY_DIR}/po/${_basename}.mo ABSOLUTE)
            ELSE(WIN32)
                FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/po)
                GET_FILENAME_COMPONENT(_out ${CMAKE_CURRENT_BINARY_DIR}/po/${_basename}.mo ABSOLUTE)
            ENDIF(WIN32)
            
            ADD_CUSTOM_COMMAND(
                OUTPUT ${_out}
                COMMAND ${CMAKE_COMMAND}
                    -E echo
                    "Generating" ${_out} "from" ${_in}
                COMMAND ${MSGFMT_EXECUTABLE}
                    ${_in}
                    -o ${_out}
                DEPENDS ${_in}
            )
                
            SET(mo_files ${mo_files} ${_out})

            INSTALL(FILES ${_out} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/locale/${_basename}/LC_MESSAGES RENAME avidemux.mo)
        ENDFOREACH(po_input ${po_files})

        SET(${_sources} ${${_sources}} ${mo_files})
    ENDIF(MSGFMT_EXECUTABLE)
ENDMACRO(COMPILE_PO_FILES po_subdir)

