
SET(ADM_filtersNone_SRCS filter_none.cpp)
ADD_LIBRARY(ADM_filtersNone STATIC ${ADM_filtersNone_SRCS})
include_directories(../ADM_commonUI)
