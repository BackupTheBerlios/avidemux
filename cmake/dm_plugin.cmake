MACRO(INIT_DEMUXER _lib)
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_core/include")
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_coreAudio/include")
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_coreDemuxer/include")
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_coreUI/include")
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_coreImage/include")
    INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_coreUtils/include")
ENDMACRO(INIT_DEMUXER)

MACRO(INSTALL_DEMUXER _lib)
	INSTALL(TARGETS ${_lib} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ADM_plugins/demuxers/")
ENDMACRO(INSTALL_DEMUXER)
