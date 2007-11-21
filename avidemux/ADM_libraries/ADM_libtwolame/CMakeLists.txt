SET(ADM_LIB ADM_libtwolame)

SET(${ADM_LIB}_SRCS 
	ath.c        bitbuffer.c  crc.c  encode.c      energy.c  get_set.c  psycho_0.c  psycho_2.c  psycho_4.c   subband.c  twolame.c
	availbits.c  dab.c   fft.c     mem.c      psycho_1.c  psycho_3.c  psycho_n1.c     util.c)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DNDEBUG -DINLINE=inline")