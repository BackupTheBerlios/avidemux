#if(QT4_FOUND)
#add_subdirectory(ADM_QT4)
#endifif(QT4_FOUND)

add_subdirectory(ADM_NONE)
add_subdirectory(ADM_commonUI)

include_directories(ADM_commonUI)
