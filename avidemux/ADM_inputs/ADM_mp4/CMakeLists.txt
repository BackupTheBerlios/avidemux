
SET(ADM_mp4_SRCS 
ADM_atom.cpp  ADM_mp4Analyzer.cpp  ADM_mp4audio.cpp  ADM_mp4.cpp  ADM_mp4Indexer.cpp  ADM_mp4Leaf.cpp)
ADD_LIBRARY(ADM_mp4 STATIC ${ADM_mp4_SRCS})
include_directories(..)
