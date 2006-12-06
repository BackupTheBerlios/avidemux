
noinst_LIBRARIES = libADM_dialog.a 
libADM_dialog_a_SOURCES = DIA_busy.cpp  alert_qt4.cpp DIA_none.cpp OCR_none.cpp DIA_indexing.cpp \
AQ_props.cpp AQ_working.cpp AQ_encoding.cpp

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs @ADM_QTINCLUDES@

libADM_dialog_a_METASOURCES = AUTO


AQ_%.cpp : Q_%.cpp %.ui
	$(UIC) $*.ui -o ui_$*.h
	$(MOC) Q_$*.cpp -o Q_$*.moc
	cp Q_$*.cpp AQ_$*.cpp

AT_%.cpp : T_%.cpp  
	$(MOC) T_$*.cpp -o T_$*.moc
	cp T_$*.cpp AT_$*.cpp



EXTRA_DIST =  
qclean:
	rm -f ui_*.h AT_*.cpp AQ_*.cpp  *.moc

