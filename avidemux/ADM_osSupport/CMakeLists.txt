SET(ADM_osSupport_SRCS 
	ADM_cpuCap.cpp     ADM_debug.cpp   ADM_memcpy.cpp  ADM_memsupport.cpp  ADM_queue.cpp  ADM_threads.cpp  win32.cpp
	ADM_crashdump.cpp  ADM_fileio.cpp  ADM_memory.cpp  ADM_misc.cpp        ADM_quota.cpp  TLK_clock.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_osSupport ${ADM_osSupport_SRCS})
ADD_SOURCE_CFLAGS(ADM_crashdump.cpp ${GLIB_CFLAGS})
ADD_SOURCE_CFLAGS(ADM_fileio.cpp ${GLIB_CFLAGS})
ADD_SOURCE_CFLAGS(ADM_quota.cpp -I${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS})