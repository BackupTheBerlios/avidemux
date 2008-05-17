SET(ADM_LIB ADM_UIQT4)

QT4_WRAP_CPP(${ADM_LIB}_source
	T_bitrate.h  T_button.h  T_dialogFactory.h  T_filesel.h  T_menu.h  T_slider.h
	T_threadCount.h  T_toggle.h)

SET(${ADM_LIB}_T
	T_bitrate.cpp  T_button.cpp  T_dialogFactory.cpp  T_filesel.cpp  T_menu.cpp
	T_slider.cpp  T_threadCount.cpp  T_toggle.cpp)

SET(${ADM_LIB}_SRCS
	FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp
	FAC_float.cpp  FAC_hex.cpp    FAC_readOnlyText.cpp
	FAC_matrix.cpp  ${${ADM_LIB}_source}  ${${ADM_LIB}_T})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
