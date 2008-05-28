INCLUDE(admCheckFreeType)
checkFreeType()
IF(USE_FREETYPE)
        ADD_SUBDIRECTORY(ADM_libass)
        
        INCLUDE(vf_plugin)
        SET(ADM_vf_ssa_SRCS ADM_vidASS.cpp)

        ADD_LIBRARY(ADM_vf_ssa SHARED ${ADM_vf_ssa_SRCS})
        TARGET_LINK_LIBRARIES(ADM_vf_ssa ADM_libass)
        INIT_VIDEOFILTER_PLUGIN(ADM_vf_ssa)
        INSTALL_VIDEOFILTER(ADM_vf_ssa)
ENDIF(USE_FREETYPE)
