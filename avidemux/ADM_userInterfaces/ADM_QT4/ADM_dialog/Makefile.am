
noinst_LIBRARIES = libADM_dialog.a 
libADM_dialog_a_SOURCES = DIA_busy.cpp  alert_qt4.cpp DIA_none.cpp OCR_none.cpp  \
AQ_props.cpp AQ_working.cpp AQ_encoding.cpp  DIA_xvid4.cpp AQ_resizing.cpp AT_flyDialog.cpp \
AQ_jobs.cpp \
AQ_crop.cpp AT_QCanvas.cpp \
AQ_asharp.cpp AQ_chromashift.cpp \
AQ_contrast.cpp AQ_hue.cpp \
AQ_srt.cpp \
AT_index_pg.cpp DIA_x264.cpp \
DIA_srt.cpp

INCLUDES = $(all_includes)  $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs @ADM_QTINCLUDES@

libADM_dialog_a_METASOURCES = AUTO


include ../Makefile.adm


EXTRA_DIST =  

