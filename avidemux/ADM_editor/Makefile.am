####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_editor.a

INCLUDES = $(all_includes)  $(SMONKEY_CFLAGS) \
	-I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil 	\
	-I../ADM_inputs


libADM_editor_a_METASOURCES = AUTO

libADM_editor_a_SOURCES = ADM_Video.cpp ADMedAVIAUD.cpp ADM_edStub.cpp \
	ADM_edSearch.cpp ADM_edRender.cpp ADM_edit.cpp ADM_edAudio.cpp ADM_edIdentify.cpp \
	ADM_edLoadSave.cpp ADM_edFrameType.cpp ADM_edCache.cpp


EXTRA_DIST = ADM_Video.cpp    ADM_edCache.cpp      \
ADM_edLoadSave.cpp  ADM_edit.cpp \
ADM_Video.h      ADM_edCache.h        ADM_edRender.cpp    ADM_edit.hxx \
ADM_edAudio.cpp  ADM_edFrameType.cpp  ADM_edSearch.cpp    ADM_outputfmt.h \
ADM_edAudio.hxx  ADM_edIdentify.cpp   ADM_edStub.cpp      ADMedAVIAUD.cpp


####### kdevelop will overwrite this part!!! (end)############
noinst_HEADERS = ADM_edCache.h ADM_outputfmt.h
