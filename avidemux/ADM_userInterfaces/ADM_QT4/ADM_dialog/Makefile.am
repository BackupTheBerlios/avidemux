
noinst_LIBRARIES = libADM_dialog.a 

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs @ADM_QTINCLUDES@

libADM_dialog_a_METASOURCES = AUTO

libADM_dialog_a_SOURCES = DIA_busy.cpp  DIA_encoding.cpp  DIA_working.cpp alert_qt4.cpp DIA_none.cpp OCR_none.cpp DIA_indexing.cpp Q_props.cpp



Q_%.moc : Q_%.cpp
	$(MOC) $< -o $@
%.moc : %.cpp
	$(MOC) $< -o $@
ui_%.h: %.ui
	$(UIC) $< -o $@
	$(MOC) Q_$*.cpp -o Q_$*.moc

Q_%.cpp : ui_%.h 



EXTRA_DIST =  
