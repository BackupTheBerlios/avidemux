SET(ADM_core_SRCS 
	ADM_cpuCap.cpp  ADM_memcpy.cpp  ADM_memsupport.cpp  ADM_threads.cpp  ADM_win32.cpp
	ADM_memory.cpp  ADM_misc.cpp  TLK_clock.cpp  ADM_crashdump.cpp  ADM_fileio.cpp ADM_dynamicLoading.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_core ${ADM_core_SRCS})
ADD_SOURCE_CFLAGS(ADM_fileio.cpp ${GLIB_CFLAGS})

#ADD_TARGET_DEFINITIONS(ADM_core ADM_DEBUG FIND_LEAKS)