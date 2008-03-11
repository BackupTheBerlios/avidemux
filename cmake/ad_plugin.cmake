MACRO(ADM_audio_plugin  _lib)
	INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/config")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_core/include")
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")

	INSTALL(TARGETS ${_lib} LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ADM_plugins/audioDecoder/")
ENDMACRO(ADM_audio_plugin)

