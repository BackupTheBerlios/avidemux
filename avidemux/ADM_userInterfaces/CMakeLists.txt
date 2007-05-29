subdirs(PREORDER ADM_NONE)
subdirs(PREORDER ADM_commonUI)

if(GTK_FOUND)
subdirs(PREORDER ADM_GTK)
endif(GTK_FOUND)

if(QT4_FOUND)
subdirs(PREORDER ADM_QT4)
endif(QT4_FOUND)

include_directories(ADM_commonUI)
