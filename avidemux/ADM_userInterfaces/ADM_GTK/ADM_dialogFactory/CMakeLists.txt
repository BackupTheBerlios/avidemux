################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogFactoryGtk)
#
SET(${ADM_LIB}_SRCS
DIA_color.cpp          DIA_filesel.cpp  FAC_bitrate.cpp  FAC_float.cpp  FAC_hex.cpp      FAC_menu.cpp   FAC_readOnlyText.cpp
DIA_dialogFactory.cpp  FAC_bar.cpp      FAC_button.cpp   FAC_frame.cpp  FAC_integer.cpp  FAC_notch.cpp  FAC_toggle.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(${GTK_CFLAGS})
include_directories(.. ../..)
#../../../ADM_osSupport)
