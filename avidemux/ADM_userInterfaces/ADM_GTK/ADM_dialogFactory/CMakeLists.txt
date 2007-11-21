SET(ADM_LIB ADM_dialogFactoryGtk)

SET(${ADM_LIB}_SRCS
	DIA_color.cpp
	DIA_filesel.cpp
	DIA_dialogFactory.cpp
	FAC_bitrate.cpp
	FAC_float.cpp
	FAC_hex.cpp
	FAC_menu.cpp
	FAC_readOnlyText.cpp
	FAC_bar.cpp
	FAC_button.cpp
	FAC_frame.cpp
	FAC_integer.cpp
	FAC_notch.cpp
	FAC_toggle.cpp
	FAC_threadCount.cpp
	FAC_matrix.cpp)

ADD_ADM_LIB_GTK_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_GTK_TARGET(${ADM_LIB} ${GTK_CFLAGS})