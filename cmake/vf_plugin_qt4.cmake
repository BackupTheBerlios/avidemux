MACRO(INIT_VIDEOFILTER_PLUGIN_QT4  lib  _srcsQt _srcQt_ui _srcsCommon)
INCLUDE(admCheckQt4)

checkQt4()

IF (QT4_FOUND)

	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/qt4 )
        QT4_WRAP_UI(qt4_ui qt4/${_srcQt_ui}.ui )
        QT4_WRAP_CPP(qt4_cpp  ${qt4_ui})
        MESSAGE("ZZ UI" ${qt4_ui})
        MESSAGE("ZZ CPP" ${qt4_cpp})
        ADD_LIBRARY(${lib} SHARED ${_srcsCommon} ${qt4_cpp}  ${qt4_ui} ${x264Qt_source})
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ ../../../ADM_UIs/ADM_QT4/include/)
        TARGET_LINK_LIBRARIES( ${lib} ADM_UIQT4)
        TARGET_LINK_LIBRARIES(${lib}  ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
        	
        INIT_VIDEOFILTER_PLUGIN(${lib})
        INSTALL_VIDEOFILTER(${lib})
ENDIF (QT4_FOUND)
ENDMACRO(INIT_VIDEOFILTER_PLUGIN_QT4)
