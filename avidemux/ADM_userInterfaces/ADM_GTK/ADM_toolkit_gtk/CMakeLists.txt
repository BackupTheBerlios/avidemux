SET(ADM_LIB ADM_toolkitGtk)

SET(${ADM_LIB}_SRCS
	ADM_gladeSupport.cpp  ADM_tray_gtk.cpp  choice.cpp  TLK_filesel.cpp  toolkit.cpp  toolkit_dialog.cpp 
    eggtrayicon.c eggtray_win32.c gtkmarkscale.c jogshuttle.c ADM_jogshuttle.cpp  mediactrl.c)

ADD_ADM_LIB_GTK_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_GTK_TARGET(${ADM_LIB} ${GTK_CFLAGS})