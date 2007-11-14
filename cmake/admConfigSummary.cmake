# ARGV2 - capable flag
MACRO(ADM_DISPLAY _name _val)
	SET(PROCEED TRUE)
	
	IF (${ARGC} EQUAL 3)
		IF (NOT ${ARGV2})
			SET(PROCEED FALSE)
		ENDIF (NOT ${ARGV2})
	ENDIF (${ARGC} EQUAL 3)
	
	IF (${PROCEED} STREQUAL "TRUE")
		IF (${_val} EQUAL 1)
			SET(s "Yes")
		ELSE (${_val} EQUAL 1)
			SET(s "No")
		ENDIF (${_val} EQUAL 1)
	
		MESSAGE("   ${_name}  ${s}")
	ENDIF (${PROCEED} STREQUAL "TRUE")
ENDMACRO(ADM_DISPLAY)

MESSAGE("*********************")
MESSAGE("***    SUMMARY    ***")
MESSAGE("*********************")
ADM_DISPLAY("GTK+      " ADM_UI_GTK)
ADM_DISPLAY("Qt4       " ADM_UI_QT4)

MESSAGE("***  Video Codec  ***")
ADM_DISPLAY("x264      " USE_X264)
ADM_DISPLAY("Xvid      " USE_XVID_4)

MESSAGE("***  Audio Codec  ***")
ADM_DISPLAY("Aften     " USE_AFTEN)
ADM_DISPLAY("amrnb     " USE_AMR_NB)
ADM_DISPLAY("LAME      " HAVE_LIBMP3LAME)
ADM_DISPLAY("FAAC      " USE_FAAC)
ADM_DISPLAY("FAAD      " USE_FAAD)
ADM_DISPLAY("libdca    " USE_LIBDCA)
ADM_DISPLAY("Vorbis    " USE_VORBIS)

MESSAGE("*** Audio Device  ***")
ADM_DISPLAY("ALSA      " ALSA_SUPPORT ${ALSA_CAPABLE})
ADM_DISPLAY("aRts      " USE_ARTS ${ARTS_CAPABLE})
ADM_DISPLAY("ESD       " USE_ESD ${ESD_CAPABLE})
ADM_DISPLAY("JACK      " USE_JACK ${JACK_CAPABLE})
ADM_DISPLAY("OSS       " OSS_SUPPORT ${OSS_CAPABLE})
ADM_DISPLAY("SRC       " USE_SRC ${JACK_CAPABLE})

MESSAGE("*** Miscellaneous ***")
ADM_DISPLAY("FontConfig" USE_FONTCONFIG)
ADM_DISPLAY("FreeType2 " FREETYPE2_FOUND)
ADM_DISPLAY("gettext   " HAVE_GETTEXT)
ADM_DISPLAY("SDL       " USE_SDL)
ADM_DISPLAY("XVideo    " USE_XV ${XVIDEO_CAPABLE})

MESSAGE("*********************")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE("***  Debug Build  ***")
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE("*** Release Build ***")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

MESSAGE("*********************")
