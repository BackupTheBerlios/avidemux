SET(ADM_oplug_avi_SRCS 
	avilist.cpp  op_avisavecopy_pack.cpp   op_avisave.cpp   op_aviwrite.cpp   op_savesmart.cpp
	op_avisavecopy.cpp  op_avisavecopy_unpack.cpp  op_avisavedual.cpp  op_saveprocess.cpp)

ADD_ADM_LIB_ALL_TARGETS(oplug_avi ${ADM_oplug_avi_SRCS})
