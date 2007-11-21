SET(ADM_matroska_SRCS 
	ADM_mkv_audio.cpp  ADM_mkv.cpp  ADM_mkvEntries.cpp  ADM_mkvIndexer.cpp  ADM_mkvTrackType.cpp  ebml.cpp  mkv_tags.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_matroska ${ADM_matroska_SRCS})