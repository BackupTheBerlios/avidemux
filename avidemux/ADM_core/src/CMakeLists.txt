SET(ADM_core_SRCS 
	ADM_cpuCap.cpp  ADM_memcpy.cpp  ADM_memsupport.cpp  ADM_threads.cpp  ADM_win32.cpp
	ADM_memory.cpp  ADM_misc.cpp  TLK_clock.cpp  ADM_crashdump.cpp  ADM_fileio.cpp ADM_dynamicLoading.cpp
)

ADD_LIBRARY(ADM_core SHARED ${ADM_core_SRCS})
ADD_TARGET_DEFINITIONS(ADM_core ADM_DEBUG FIND_LEAKS)

IF (MINGW)
	TARGET_LINK_LIBRARIES(ADM_core "${PTHREAD_LIBRARIES}" imagehlp ws2_32)
	ADD_TARGET_CFLAGS(ADM_core "-I${PTHREAD_INCLUDE_DIR}")
ELSEIF (APPLE)
	ADD_TARGET_LDFLAGS(ADM_core "-framework Carbon")
ENDIF (MINGW)

REMOVE_DEFINITIONS(-DHAVE_CONFIG_H)

INSTALL(TARGETS ADM_core RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
