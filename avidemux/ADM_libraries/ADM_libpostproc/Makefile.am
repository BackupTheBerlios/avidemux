####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libpostproc.a


libpostproc_a_METASOURCES = AUTO

libpostproc_a_SOURCES =  postprocess.c
	


EXTRA_DIST = 

####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes)   -I../.. -DHAVE_AV_CONFIG_H  -I../../../ADM_libraries/ADM_lavutil/
if BUILD_A64
INCLUDES +=-DHAVE_MMX -msse 
endif
noinst_HEADERS =  


