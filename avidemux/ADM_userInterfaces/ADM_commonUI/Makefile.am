noinst_LIBRARIES = libADM_CommonUI.a 
libADM_CommonUI_a_METASOURCES = AUTO
libADM_CommonUI_a_SOURCES = GUI_sdlRender.cpp  GUI_xvRender.cpp  DIA_resizeWiz.cpp DIA_builtin.cpp DIA_postproc.cpp


INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I./../../ADM_libraries/ADM_utilities \
		-I./../ADM_commonUI -I../.... -I../..../ADM_libraries/ADM_lavutil \
		-I./../../ADM_inputs -I../..../ADM_outputs -I../../ADM_ocr
