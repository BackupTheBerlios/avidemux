MACRO(ADM_audio_plugin  _lib)
        ADD_TARGET_CFLAGS(${_lib} "-I${CMAKE_BINARY_DIR}/config/cli")
        ADD_TARGET_CFLAGS(${_lib} "-I${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")
        INSTALL(TARGETS ${_lib} LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ADM_plugins/audioDecoder/")
ENDMACRO(ADM_audio_plugin)

