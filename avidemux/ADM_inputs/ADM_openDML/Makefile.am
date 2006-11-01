noinst_LIBRARIES = libADM_openDML.a

INCLUDES = $(all_includes)  -I../../ADM_libraries -I../../ADM_libraries/ADM_utilities   -I../../ADM_libraries/ADM_lavutil \
		-I../../ADM_userInterfaces/ADM_GTK

libADM_openDML_a_METASOURCES = AUTO

libADM_openDML_a_SOURCES = ADM_openDML.cpp ADM_riff.cpp ADM_odml_odml.cpp \
		ADM_odml_regular.cpp ADM_opendml_reorder.cpp \
		ADM_opendml_reindex.cpp ADM_openDMLDepack.cpp \
		ADM_odml_audio.cpp


noinst_HEADERS =  ADM_openDML.h ADM_riff.h
EXTRA_DIST=ADM_odml_audio.cpp  ADM_odml_regular.cpp \
ADM_openDMLDepack.cpp    ADM_riff.cpp \
ADM_odml_audio.h    ADM_openDML.cpp       ADM_opendml_reindex.cpp  ADM_riff.h \
ADM_odml_odml.cpp   ADM_openDML.h         ADM_opendml_reorder.cpp

