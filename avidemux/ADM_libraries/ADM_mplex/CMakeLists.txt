################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_mplex)
#
SET(${ADM_LIB}_SRCS 
ac3strm_in.cpp      ADM_mplexout.cpp   audiostrm_out.cpp  dtsstrm_in.cpp  lpcmstrm_in.cpp   multiplexor.cpp   stream_params.cpp  videostrm_in.cpp   zalphastrm_out.cpp
ADM_mplexin.cpp     ADM_mthread.cpp    bits.cpp           inputstrm.cpp   padstrm.cpp       systems.cpp        videostrm_out.cpp
ADM_mplexmuxer.cpp  ADM_transfert.cpp  buffer.cpp         interact.cpp    mpastrm_in.cpp    stillsstream.cpp  vector.cpp         zalphastrm_in.cpp
mpegconsts.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
################################
include_directories(../ADM_libmpeg2enc  ../../ADM_userInterfaces/)

