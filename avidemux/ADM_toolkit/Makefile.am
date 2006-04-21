####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_toolkit.a

## INCLUDES were found outside kdevelop specific part

libADM_toolkit_a_METASOURCES = AUTO

libADM_toolkit_a_SOURCES = automation.cpp choice.cpp toolkit.cpp filesel.cpp \
	toolkit_gtk.h toolkit_dialog.cpp toolkit_gtk_include.h TLK_filesel.cpp TLK_clock.h \
	TLK_clock.cpp ADM_debug.h ADM_debug.cpp ADM_debugID.h ADM_cpuCap.cpp win32.cpp \
	ADM_memory.cpp ADM_memcpy.cpp ADM_quota.cpp eggtrayicon.c eggtray_win32.c


EXTRA_DIST = filesel.cpp filesel.h toolkit.cpp toolkit.hxx bitmap.h choice.cpp automation.cpp  win32.cpp ADM_quota.h

####### kdevelop will overwrite this part!!! (end)############
INCLUDES= $(all_includes) $(GTK_CFLAGS) $(SMONKEY_CFLAGS)
noinst_HEADERS =  ADM_cpuCap.h
