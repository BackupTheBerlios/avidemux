####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libswscale.a

#INCLUDES = $(all_includes)

libswscale_a_METASOURCES = AUTO

libswscale_a_SOURCES = swscale.c yuv2rgb.c rgb2rgb.c yuv2rgb_altivec.c ADM_mp.cpp


EXTRA_DIST = ADM_mp.cpp  ADM_mp.h    rgb2rgb.h  swscale.h  swscale_internal.h  wrapper.h \
rgb2rgb.c swscale_altivec_template.c  yuv2rgb_altivec.c \
rgb2rgb_template.c  swscale_template.c  yuv2rgb_template.c \
swscale.c yuv2rgb.c


####### kdevelop will overwrite this part!!! (end)############

INCLUDES= $(all_includes)  -I../ADM_lavutil -I../ADM_utilities -I.. -DCONFIG_GPL
noinst_HEADERS = 
