########################################
# GTK+
########################################
OPTION(GTK "" ON)

MESSAGE(STATUS "Checking for GTK+")
MESSAGE(STATUS "*****************")

IF (GTK)
	PKG_CHECK_MODULES(GTK gtk+-2.0)
	PRINT_LIBRARY_INFO("GTK+" GTK_FOUND "${GTK_CFLAGS}" "${GTK_LDFLAGS}")

	IF (GTK_FOUND)
		ADM_COMPILE(gtk_x11_check.cpp "${GTK_CFLAGS}" "" "${GTK_LDFLAGS}" GTK_X11_SUPPORTED outputGtkX11Test)

		IF (GTK_X11_SUPPORTED)
			SET(HAVE_GTK_X11 1)

			MESSAGE(STATUS "GTK+ uses X11")
		ELSE (GTK_X11_SUPPORTED)
			MESSAGE(STATUS "GTK+ doesn't use X11")

			IF (VERBOSE)
				MESSAGE("Error Message: ${outputGtkX11Test}")
			ENDIF (VERBOSE)
		ENDIF (GTK_X11_SUPPORTED)
	ELSE (GTK_FOUND)
		SET(ADM_UI_GTK 0)
	ENDIF (GTK_FOUND)
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
INCLUDE(admCheckQt4)

checkQt4()

IF (NOT QT4_FOUND)
	SET(ADM_UI_QT4 0)
ENDIF (NOT QT4_FOUND)