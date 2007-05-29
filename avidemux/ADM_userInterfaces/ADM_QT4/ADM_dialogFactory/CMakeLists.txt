################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogFactoryQt4)
#
SET(${ADM_LIB}_SRCS
AT_bitrate.cpp  AT_dialogFactory.cpp  FAC_bar.cpp    FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp         FAC_toggle.cpp
AT_button.cpp   AT_filesel.cpp        FAC_float.cpp  FAC_hex.cpp    FAC_menu.cpp     FAC_readOnlyText.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. ../../ ../../../ADM_inputs)
include_directories(${QT_INCLUDES})

