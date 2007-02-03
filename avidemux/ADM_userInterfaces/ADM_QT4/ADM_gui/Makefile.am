
noinst_LIBRARIES = libADM_gui2.a 

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs \
		@ADM_QTINCLUDES@

libADM_gui2_a_METASOURCES = AUTO

libADM_gui2_a_SOURCES = gui_none.cpp file_qt4.cpp avidemux_rsc.cpp \
			AQ_gui2.cpp   AT_preview.cpp 

include ../Makefile.adm

avidemux_rsc.cpp : avidemux.qrc
	$(RCC)  $< -o $@	-name avidemux
EXTRA_DIST =

