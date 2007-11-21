SET(ADM_colorspace_SRCS ADM_rgb.cpp  rgb2yuv.cpp  yuv.cpp ADM_colorspace.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_colorspace ${ADM_colorspace_SRCS})
