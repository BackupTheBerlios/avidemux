#
# Low level demuxers for mpeg TS/PTS/ES/MS
#
#

SET(ADMcoreDemuxerMpeg_SRCS
#dmx_demuxer.cpp
#dmx_demuxerES.cpp
#dmx_demuxerMSDVR.cpp
#dmx_demuxerPS.cpp
#dmx_demuxerTS.cpp
#dmx_io.cpp
)	
#*************************************************
ADD_LIBRARY(ADM_coreDemuxerMpeg SHARED ${ADMcoreDemuxerMpeg_SRCS})
TARGET_LINK_LIBRARIES(ADM_coreDemuxerMpeg ADM_core)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INSTALL(TARGETS ADM_coreDemuxerMpeg RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
