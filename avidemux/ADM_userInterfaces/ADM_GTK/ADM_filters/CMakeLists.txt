SET(ADM_LIB ADM_filtersGtk)

SET(${ADM_LIB}_SRCS
	gui_filterlist.cpp  gui_filtermanager.cpp  gui_filtermanager_dialog.cpp  gui_filtersub.cpp)

ADD_ADM_LIB_GTK_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_GTK_TARGET(${ADM_LIB} ${GTK_CFLAGS})