SET(ADM_mp4_SRCS 
	ADM_atom.cpp  ADM_mp4Analyzer.cpp  ADM_mp4audio.cpp  ADM_mp4.cpp  ADM_mp4Indexer.cpp  ADM_mp4Leaf.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(ADM_mp4 ${ADM_mp4_SRCS})