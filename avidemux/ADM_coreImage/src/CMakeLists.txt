SET(ADM_coreImage_SRCS 
	ADM_confCouple.cpp  
        ADM_videoFilter.cpp  
        ADM_videoFilterCache.cpp
        ADM_image.cpp  
        ADM_imageUtils.cpp
        ADM_imageResizer.cpp
        
)

ADD_LIBRARY(ADM_coreImage SHARED ${ADM_coreImage_SRCS})
TARGET_LINK_LIBRARIES(ADM_coreImage ADM_core)

INSTALL(TARGETS ADM_coreImage RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
