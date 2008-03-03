########################################
# GLib
########################################
MESSAGE(STATUS "Checking for GLib")
MESSAGE(STATUS "*****************")

PKG_CHECK_MODULES(GLIB glib-2.0)
PRINT_LIBRARY_INFO("GLib" GLIB_FOUND "${GLIB_CFLAGS}" "${GLIB_LDFLAGS}" FATAL_ERROR)

MESSAGE("")

########################################
# GTK+
########################################
OPTION(GTK "" ON)

MESSAGE(STATUS "Checking for GTK+")
MESSAGE(STATUS "*****************")

IF (GTK)
	PKG_CHECK_MODULES(GTK gtk+-2.0)
	PRINT_LIBRARY_INFO("GTK+" GTK_FOUND "${GTK_CFLAGS}" "${GTK_LDFLAGS}")

	IF (NOT GTK_FOUND)
		SET(ADM_UI_GTK 0)
	ENDIF (NOT GTK_FOUND)
ELSE (GTK)
	SET(ADM_UI_GTK 0)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (GTK)

MESSAGE("")

########################################
# GThread
########################################
MESSAGE(STATUS "Checking for GThread")
MESSAGE(STATUS "********************")

IF (GTK)
	PKG_CHECK_MODULES(GTHREAD gthread-2.0)
	PRINT_LIBRARY_INFO("GThread" GTHREAD_FOUND "${GTHREAD_CFLAGS}" "${GTHREAD_LDFLAGS}")

	IF (NOT GTHREAD_FOUND)
		SET(ADM_UI_GTK 0)
		MESSAGE(STATUS "Could not find GThread")
	ENDIF(NOT GTHREAD_FOUND)
ELSE (GTK)
	SET(ADM_UI_GTK 0)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (GTK)

MESSAGE("")

########################################
# Qt4
########################################
OPTION(QT4 "" ON)

MESSAGE(STATUS "Checking for Qt 4")
MESSAGE(STATUS "*****************")

IF (QT4)
	FIND_PACKAGE(Qt4)	
	STRING(REGEX REPLACE "[\\]" "/" QT_INCLUDES "${QT_INCLUDES}")	# backslashes aren't taken care of properly on Windows
	PRINT_LIBRARY_INFO("Qt 4" QT4_FOUND "${QT_INCLUDES} ${QT_DEFINITIONS}" "${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}")

	MARK_AS_ADVANCED(LRELEASE_EXECUTABLE)
	MARK_AS_ADVANCED(QT_MKSPECS_DIR)
	MARK_AS_ADVANCED(QT_PLUGINS_DIR)
	MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)

	IF (NOT QT4_FOUND)
		SET(ADM_UI_QT4 0)
	ENDIF (NOT QT4_FOUND)
ELSE (QT4)
	MESSAGE("${MSG_DISABLE_OPTION}")
ENDIF (QT4)

MESSAGE("")