SET(ADM_video_SRCS 
ADM_cache.cpp                  
ADM_vidCNR2.cpp       
ADM_vidFieldUnblend.cpp  
ADM_vidPartial.cpp    
ADM_confCouples.cpp            
ADM_vidColorYuv.cpp   
ADM_vidFieldUtil.cpp     
ADM_vidRaw.cpp        
ADM_genvideo.cpp         
ADM_vidFont.cpp
ADM_interlaced.cpp       
ADM_vidCached.cpp       
ADM_vidMPdelogo.cpp      
ADM_vidClean.cpp        
ADM_vidEqualizer.cpp  
ADM_vidNull.cpp          
)
ADD_LIBRARY(ADM_video STATIC ${ADM_video_SRCS})

if(USE_FREETYPE)
	include_directories(${FREETYPE2_INCLUDE_DIRS})
endif(USE_FREETYPE)

include_directories(../ADM_inputs)
