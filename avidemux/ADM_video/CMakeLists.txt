SET(ADM_video_SRCS 
ADM_vidCNR2.cpp       
ADM_vidFieldUnblend.cpp  
ADM_vidPartial.cpp    
ADM_vidColorYuv.cpp   
ADM_vidFieldUtil.cpp     
ADM_vidRaw.cpp        
ADM_vidFont.cpp
ADM_interlaced.cpp       
ADM_vidMPdelogo.cpp      
ADM_vidClean.cpp        
ADM_vidEqualizer.cpp  
ADM_vidNull.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_video ${ADM_video_SRCS})

IF (USE_FREETYPE)
	ADD_CFLAGS_ALL_TARGETS(ADM_video ${FREETYPE2_CFLAGS})
ENDIF (USE_FREETYPE)

ADD_CFLAGS_ALL_TARGETS(ADM_video "-I../ADM_inputs")
