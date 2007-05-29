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
)

SET(${ADM_LIB}_SRCS
FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp         FAC_toggle.cpp
FAC_float.cpp  FAC_hex.cpp    FAC_menu.cpp     FAC_readOnlyText.cpp
)

################################
include(adm_qt4)
################################

################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. ../../ ../../../ADM_inputs)
include_directories(${QT_INCLUDES})

