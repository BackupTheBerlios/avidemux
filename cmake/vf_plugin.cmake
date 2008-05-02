MACRO(INIT_VIDEOFILTER_PLUGIN _lib)
	INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/config")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_core/include")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_coreUI/include")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_coreImage/include")
ENDMACRO(INIT_VIDEOFILTER_PLUGIN)

MACRO(INSTALL_VIDEOFILTER _lib)
	TARGET_LINK_LIBRARIES(${_lib} ADM_core ADM_coreUI ADM_coreImage ${AMRNB_LIBRARY_DIR})
	INSTALL(TARGETS ${_lib} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ADM_plugins/videoFilter/")
ENDMACRO(INSTALL_VIDEOFILTER)