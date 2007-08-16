################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogFactoryQt4)
#
SET(${ADM_LIB}_T
bitrate  
dialogFactory  
button
filesel        
menu
toggle
threadCount
)

SET(${ADM_LIB}_SRCS
FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp         
FAC_float.cpp  FAC_hex.cpp    FAC_readOnlyText.cpp
)

################################
include(adm_qt4)
################################

################################
include_directories(.. ../../ ../../../ADM_inputs)
include_directories(${QT_INCLUDES})

