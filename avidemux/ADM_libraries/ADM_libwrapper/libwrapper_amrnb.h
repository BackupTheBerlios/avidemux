#ifndef LIBWRAPPER_AMRNB
#define LIBWRAPPER_AMRNB

#ifdef USE_AMR_NB
#ifdef __cplusplus
#include "libwrapper.h"

extern "C"
{
#endif	// __cpluscplus

#include "amrnb/interf_dec.h"
#include "amrnb/interf_enc.h"

#ifdef __cplusplus
}
#endif	// __cpluscplus

#ifdef __cplusplus
class ADM_LibWrapperAmrnb : public ADM_LibWrapper
{
	public:
	#ifdef USE_LATE_BINDING
		void (*func_Decoder_Interface_exit)(void *state);
		void* (*func_Decoder_Interface_init)(void);		
		void (*func_Encoder_Interface_exit)(void *state);
		void (*func_GP3Decoder_Interface_Decode)(void *st, unsigned char *bits, short *synth, int bfi);
		int (*func_GP3VADxEncoder_Interface_Encode)(void *st, enum Mode mode, short *speech, unsigned char *serial, int forceSpeech, char vad2_code);
		void* (*func_VADxEncoder_Interface_init)(int dtx, char vad2_code);
	#endif
	
		ADM_LibWrapperAmrnb();
		virtual	~ADM_LibWrapperAmrnb();
};
#else
struct ADM_LibWrapperAmrnb;

void Decoder_Interface_Decode_(struct ADM_LibWrapperAmrnb* amrnb, void *st, unsigned char *bits, short *synth, int bfi);
void Decoder_Interface_exit_(struct ADM_LibWrapperAmrnb* amrnb, void *state);
void* Decoder_Interface_init_(struct ADM_LibWrapperAmrnb* amrnb);
int Encoder_Interface_Encode_(struct ADM_LibWrapperAmrnb* amrnb, void *st, enum Mode mode, short *speech, unsigned char *serial, int forceSpeech);
void Encoder_Interface_exit_(struct ADM_LibWrapperAmrnb* amrnb, void *state);
void* Encoder_Interface_init_(struct ADM_LibWrapperAmrnb* amrnb, int dtx);
#endif	// __cpluscplus

#endif	// USE_AMR_NB
#endif	// LIBWRAPPER_AMRNB
