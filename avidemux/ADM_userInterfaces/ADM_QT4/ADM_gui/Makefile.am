
noinst_LIBRARIES = libADM_gui2.a 

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs \
		@ADM_QTINCLUDES@

libADM_gui2_a_METASOURCES = AUTO

libADM_gui2_a_SOURCES = Q_gui2.cpp gui_none.cpp file_qt4.cpp preview_none.cpp

#Q_%.moc : Q_%.cpp
#	$(MOC) $< -o $@
ui_%.h: %.ui
	$(UIC) $< -o $@
	$(MOC) Q_$*.cpp -o Q_$*.moc

Q_%.cpp : ui_%.h 
	
EXTRA_DIST =  
