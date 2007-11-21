SET(ADM_LIB ADM_libwrapper)

SET(${ADM_LIB}_SRCS 
	libwrapper_amrnb.cpp  libwrapper.cpp  libwrapper_dca.cpp  libwrapper_global.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})