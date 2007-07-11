SUBDIRS= ADM_dialog  ADM_gui2 ADM_toolkit_gtk ADM_filters ADM_dialogFactory ADM_ocr

noinst_LIBRARIES = libADM_GTK.a 
libADM_GTK_a_METASOURCES = AUTO
libADM_GTK_a_SOURCES = 

INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I./../../ADM_libraries/ADM_utilities \
		-I./../ADM_commonUI -I../.... -I../..../ADM_libraries/ADM_lavutil \
		-I./../../ADM_inputs -I../..../ADM_outputs -I../../ADM_ocr
