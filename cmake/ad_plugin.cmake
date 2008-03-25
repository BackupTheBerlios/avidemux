MACRO(INIT_AUDIO_PLUGIN _lib)
	INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/config")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_core/include")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")
ENDMACRO(INIT_AUDIO_PLUGIN)

MACRO(INSTALL_AUDIODECODER _lib)
	INSTALL(TARGETS ${_lib} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ADM_plugins/audioDecoder/")
ENDMACRO(INSTALL_AUDIODECODER)