####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libpostproc.a


libpostproc_a_METASOURCES = AUTO

libpostproc_a_SOURCES =  postprocess.c
	


EXTRA_DIST = 

####### kdevelop will overwrite this part!!! (end)############
if BUILD_A64
INCLUDES = $(all_includes)   -I../.. -DHAVE_AV_CONFIG_H -DHAVE_MMX -msse
else
INCLUDES = $(all_includes)   -I../.. -DHAVE_AV_CONFIG_H
endif

noinst_HEADERS =  


