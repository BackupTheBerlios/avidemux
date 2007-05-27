
SET(ADM_filter_SRCS ADM_vidDummyFilter.cpp  filter.cpp  filter_declaration.cpp  filter_dylib.cpp  filter_saveload.cpp  vidVCD.cpp)


add_definitions(${LIBXML2_DEFINITIONS})
include_directories(${LIBXML2_INCLUDE_DIR})



ADD_LIBRARY(ADM_filter STATIC ${ADM_filter_SRCS})
