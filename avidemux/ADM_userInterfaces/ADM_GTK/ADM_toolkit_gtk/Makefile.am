
noinst_LIBRARIES = libADM_toolkit_gtk.a 

INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs -I../../../ADM_osSupport

libADM_toolkit_gtk_a_METASOURCES = AUTO

libADM_toolkit_gtk_a_SOURCES = ADM_gladeSupport.cpp  ADM_tray_gtk.cpp  choice.cpp  TLK_filesel.cpp  toolkit.cpp  toolkit_dialog.cpp \
					eggtrayicon.c eggtray_win32.c gtkmarkscale.c


EXTRA_DIST = 


