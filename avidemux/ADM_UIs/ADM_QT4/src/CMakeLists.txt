SET(ADM_LIB ADM_UIQT4)

QT4_WRAP_CPP(${ADM_LIB}_source
	T_bitrate.h  T_button.h  T_dialogFactory.h  T_filesel.h  T_menu.h  T_slider.h
	T_threadCount.h  T_toggle.h)

SET(${ADM_LIB}_T
	T_bitrate.cpp  T_button.cpp  T_dialogFactory.cpp  T_filesel.cpp  T_menu.cpp
	T_slider.cpp  T_threadCount.cpp  T_toggle.cpp
        T_flyDialogQt4.cpp T_QCanvas.cpp
)

SET(${ADM_LIB}_SRCS
	FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp
	FAC_float.cpp  FAC_hex.cpp    FAC_readOnlyText.cpp
	FAC_matrix.cpp  DIA_none.cpp  ${${ADM_LIB}_source}  ${${ADM_LIB}_T})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
ADD_SOURCE_CFLAGS(T_flyDialogQt4.cpp " -DADM_UI_TYPE_BUILD=4")
ADD_SOURCE_CFLAGS(T_QCanvas.cpp " -DADM_UI_TYPE_BUILD=4")
TARGET_LINK_LIBRARIES(${ADM_LIB} ADM_core ADM_coreUI ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ADM_render_qt4)
INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
