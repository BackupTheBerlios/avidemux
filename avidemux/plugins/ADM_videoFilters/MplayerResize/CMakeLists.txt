INCLUDE(vf_plugin)


SET(resizeCommon_SRCS ADM_vidMPLResize.cpp)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../")


INCLUDE(admCheckGtk)
INCLUDE(admCheckGettext)
INCLUDE(admPluginVideoEncoder)

checkGtk()

IF (GTK_FOUND AND GTHREAD_FOUND)
	checkGettext()

        SET(resizeUIGtk_SRCS gtk/gtk_resize.cpp)
        SET(resizeGtk_SRCS ${resizeCommon_SRCS} ${resizeUIGtk_SRCS})
	IF (GETTEXT_FOUND)
		ADD_DEFINITIONS("-DHAVE_GETTEXT")
		INCLUDE_DIRECTORIES(${GETTEXT_INCLUDE_DIR})
	ENDIF (GETTEXT_FOUND)
        
        ADD_DEFINITIONS(${GTK_CFLAGS})

        ADD_LIBRARY(ADM_vf_mplayerResize_gtk SHARED ${resizeGtk_SRCS})
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ ../../../ADM_UIs/ADM_GTK/include/)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libswscale)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavcodec)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavutil)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_UIGtk)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ${GTK_LDFLAGS} ${GTHREAD_LDFLAGS})
        	
	IF (GETTEXT_FOUND)
		TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ${GETTEXT_LIBRARY_DIR})
	ENDIF(GETTEXT_FOUND)
        INIT_VIDEOFILTER_PLUGIN(ADM_vf_mplayerResize_gtk)
        INSTALL_VIDEOFILTER(ADM_vf_mplayerResize_gtk)
ENDIF (GTK_FOUND AND GTHREAD_FOUND)
