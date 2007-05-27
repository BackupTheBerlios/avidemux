
SET(ADM_colorspace_SRCS ADM_rgb.cpp  rgb2yuv.cpp  yuv.cpp)
ADD_LIBRARY(ADM_colorspace STATIC ${ADM_colorspace_SRCS})
