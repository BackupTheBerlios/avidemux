noinst_LIBRARIES = libADM_ocr.a

INCLUDES = $(all_includes)  $(GTK_CFLAGS) \
	 -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil -I../ADM_inputs \
	-I../ADM_userInterfaces/ADM_commonUI \
	-I../ADM_userInterfaces/ADM_GTK

libADM_ocr_a_METASOURCES = AUTO

libADM_ocr_a_SOURCES =   adm_glyph.cpp adm_leftturn.cpp ADM_vob2vobsub.cpp ADM_ocr.cpp


EXTRA_DIST =  ADM_leftturn.h     ADM_vob2vobsub.cpp  adm_glyph.h  \
ADM_ocrLoadSave.h  adm_glyph.cpp       adm_leftturn.cpp


noinst_HEADERS = ADM_ocrLoadSave.h
