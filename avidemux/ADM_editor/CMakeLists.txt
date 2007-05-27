
SET(ADM_editor_SRCS ADM_edAudio.cpp  ADM_edCache.cpp      ADM_edIdentify.cpp  ADM_edLoadSave.cpp  ADM_edSearch.cpp  ADM_Video.cpp
ADMedAVIAUD.cpp  ADM_edFrameType.cpp  ADM_edit.cpp        ADM_edRender.cpp    ADM_edStub.cpp)
ADD_LIBRARY(ADM_editor STATIC ${ADM_editor_SRCS})
