SET(ADM_LIB ADM_libwrapper)

SET(${ADM_LIB}_SRCS 
	libwrapper_amrnb.cpp  libwrapper.cpp  libwrapper_dca.cpp  libwrapper_global.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})

IF (USE_AMR_NB)
	ADD_SOURCE_CFLAGS(libwrapper_amrnb.cpp -I${AMRNB_INCLUDE_DIR})
	ADD_SOURCE_CFLAGS(libwrapper_global.cpp -I${AMRNB_INCLUDE_DIR})
ENDIF (USE_AMR_NB)

IF (USE_LIBDCA)
	ADD_SOURCE_CFLAGS(libwrapper_dca.cpp -I${LIBDCA_INCLUDE_DIR})
	ADD_SOURCE_CFLAGS(libwrapper_global.cpp -I${LIBDCA_INCLUDE_DIR})
ENDIF (USE_LIBDCA)