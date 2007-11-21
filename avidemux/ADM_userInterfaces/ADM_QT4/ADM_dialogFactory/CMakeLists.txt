SET(ADM_LIB ADM_dialogFactoryQt4)

SET(${ADM_LIB}_T
	bitrate  
	dialogFactory  
	button
	filesel        
	menu
	toggle
	threadCount
	slider)

SET(${ADM_LIB}_SRCS
	FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp         
	FAC_float.cpp  FAC_hex.cpp    FAC_readOnlyText.cpp
	FAC_matrix.cpp)

INCLUDE(adm_qt4)