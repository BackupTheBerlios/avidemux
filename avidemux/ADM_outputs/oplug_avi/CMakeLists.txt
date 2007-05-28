
SET(ADM_oplug_avi_SRCS 
avilist.cpp         op_avisavecopy_pack.cpp    op_avisave.cpp      op_aviwrite.cpp     op_savesmart.cpp
op_avisavecopy.cpp  op_avisavecopy_unpack.cpp  op_avisavedual.cpp  op_saveprocess.cpp
)
ADD_LIBRARY(ADM_oplug_avi STATIC ${ADM_oplug_avi_SRCS})
