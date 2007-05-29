
SET(ADM_infoExtractor_SRCS 
ADM_infoextractor.cpp
)
ADD_LIBRARY(ADM_infoExtractor STATIC ${ADM_infoExtractor_SRCS})
include_directories(../ADM_inputs)
