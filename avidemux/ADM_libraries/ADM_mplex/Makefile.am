####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_mplex.a

## INCLUDES were found outside kdevelop specific part

libADM_mplex_a_METASOURCES = AUTO
#INCLUDES = $(all_includes) 

libADM_mplex_a_SOURCES = ac3strm_in.cpp audiostrm.hpp audiostrm_out.cpp \
	aunit.hpp bits.cpp bits.hpp buffer.cpp buffer.hpp dtsstrm_in.cpp inputstrm.cpp \
	inputstrm.hpp interact.cpp interact.hpp lpcmstrm_in.cpp mpastrm_in.cpp \
	mplexconsts.hpp multiplexor.cpp multiplexor.hpp outputstrm.hpp padstrm.cpp padstrm.hpp \
	stillsstream.cpp stillsstream.hpp stream_params.cpp stream_params.hpp systems.cpp \
	systems.hpp vector.cpp vector.hpp videostrm.hpp videostrm_in.cpp videostrm_out.cpp \
	mpegconsts.c ADM_mplexmuxer.cpp ADM_mplexin.cpp ADM_mplexout.cpp ADM_transfert.cpp \
	ADM_mthread.cpp

####### kdevelop will overwrite this part!!! (end)############
INCLUDES= $(all_includes) -I../mpeg2enc 
noinst_HEADERS = ADM_inout.h ADM_transfert.h
EXTRA_DIST=ADM_inout.h         ADM_transfert.cpp  dtsstrm_in.cpp    multiplexor.cpp  \
vector.cpp \
ADM_mplexin.cpp     ADM_transfert.h    inputstrm.cpp     padstrm.cpp        videostrm_in.cpp \
ADM_mplexmuxer.cpp  ac3strm_in.cpp     interact.cpp      stillsstream.cpp   videostrm_out.cpp \
ADM_mplexout.cpp    audiostrm_out.cpp  lpcmstrm_in.cpp   streamType.h       zalphastrm_in.cpp \
ADM_mthread.cpp     bits.cpp           main_example.cpp  stream_params.cpp  zalphastrm_out.cpp \
ADM_mthread.h       buffer.cpp         mpastrm_in.cpp    systems.cpp

