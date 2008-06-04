SET(ADM_osSupport_SRCS 
	ADM_crashHook.cpp  ADM_debug.cpp     ADM_misc.cpp  ADM_queue.cpp  ADM_quota.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_osSupport ${ADM_osSupport_SRCS})

ADD_SOURCE_CFLAGS(ADM_quota.cpp -I${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS})
