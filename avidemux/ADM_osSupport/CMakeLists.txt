
SET(ADM_osSupport_SRCS 
ADM_cpuCap.cpp     ADM_debug.cpp   ADM_memcpy.cpp  ADM_memsupport.cpp  ADM_queue.cpp  ADM_threads.cpp  win32.cpp
ADM_crashdump.cpp  ADM_fileio.cpp  ADM_memory.cpp  ADM_misc.cpp        ADM_quota.cpp  TLK_clock.cpp)

ADD_LIBRARY(ADM_osSupport STATIC ${ADM_osSupport_SRCS})
add_definitions(${LIBXML2_DEFINITIONS})
