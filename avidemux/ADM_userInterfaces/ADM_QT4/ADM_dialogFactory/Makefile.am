
noinst_LIBRARIES = libADM_dialogFactory.a 

libADM_dialogFactory_a_METASOURCES = AUTO

libADM_dialogFactory_a_SOURCES = AT_dialogFactory.cpp AT_toggle.cpp FAC_integer.cpp AT_menu.cpp FAC_float.cpp \
AT_filesel.cpp AT_bitrate.cpp \
AT_button.cpp \
FAC_bar.cpp FAC_readOnlyText.cpp FAC_notch.cpp FAC_frame.cpp FAC_hex.cpp


INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
                -I../../../ADM_libraries/ADM_utilities \
                -I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
                -I../../../ADM_inputs -I../../../ADM_outputs @ADM_QTINCLUDES@

include ../Makefile.adm

