SET(ADM_outputs_SRCS 
oplug_avi/op_avisavedual.cpp
oplug_avi/op_savesmart.cpp
oplug_avi/avilist.cpp
oplug_avi/op_avisavecopy_pack.cpp
oplug_avi/op_avisavecopy.cpp
oplug_avi/op_aviwrite.cpp
oplug_avi/op_avisavecopy_unpack.cpp
oplug_avi/op_saveprocess.cpp
oplug_avi/op_avisave.cpp
oplug_mp4/oplug_mp4.cpp
oplug_ogm/op_ogpage.cpp
oplug_ogm/op_ogsaveprocess.cpp
oplug_ogm/op_ogsave.cpp
oplug_ogm/op_ogaudio.cpp
oplug_ogm/op_ogsavecopy.cpp
oplug_mpegFF/oplug_vcdff.cpp
oplug_mpeg/op_mpegpass.cpp
)
ADD_LIBRARY(ADM_outputs STATIC ${ADM_outputs_SRCS})
include_directories(./ ../ADM_inputs )

