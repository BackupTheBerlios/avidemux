
noinst_LIBRARIES = libADM_gui2.a 

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs \
		@ADM_QTINCLUDES@

libADM_gui2_a_METASOURCES = AUTO

libADM_gui2_a_SOURCES = gui_none.cpp file_qt4.cpp avidemux_rsc.cpp \
			AQ_gui2.cpp   AT_preview.cpp 

AQ_%.cpp : Q_%.cpp  %.ui
	$(UIC) $*.ui -o ui_$*.h 
	$(MOC) Q_$*.cpp -o Q_$*.moc
	cp Q_$*.cpp AQ_$*.cpp

AT_%.cpp : T_%.cpp  
	$(MOC) T_$*.cpp -o T_$*.moc
	cp T_$*.cpp T_$*.cpp



avidemux_rsc.cpp : avidemux.qrc
	rcc  $< -o $@	-name avidemux
EXTRA_DIST =  
