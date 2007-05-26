#include "config.h"

#ifdef USE_AMR_NB
#include <stdio.h>
#include "default.h"
#include "libwrapper_amrnb.h"

ADM_LibWrapperAmrnb::ADM_LibWrapperAmrnb() : ADM_LibWrapper()
{
#ifdef USE_LATE_BINDING
#ifdef ADM_WIN32
	char* libname = "libamrnb-2.dll";
#else
	char* libname = "libamrnb-2.so";	// ???
#endif

	initialised = (loadLibrary(libname) && getSymbols(6, 
		&func_Decoder_Interface_init, "Decoder_Interface_init", 
		&func_Decoder_Interface_exit, "Decoder_Interface_exit",
		&func_Encoder_Interface_exit, "Encoder_Interface_exit",
		&func_GP3Decoder_Interface_Decode, "GP3Decoder_Interface_Decode",
		&func_GP3VADxEncoder_Interface_Encode, "GP3VADxEncoder_Interface_Encode",
		&func_VADxEncoder_Interface_init, "VADxEncoder_Interface_init"));
#else
	initialised = true;
#endif	
}

ADM_LibWrapperAmrnb::~ADM_LibWrapperAmrnb() {}

extern "C" void* Decoder_Interface_init_(ADM_LibWrapperAmrnb* amrnb)
{
#ifdef USE_LATE_BINDING
	return amrnb->func_Decoder_Interface_init();
#else
	return Decoder_Interface_init();
#endif
}

extern "C" void Decoder_Interface_Decode_(ADM_LibWrapperAmrnb* amrnb, void *st, unsigned char *bits, short *synth, int bfi)
{
#ifdef USE_LATE_BINDING
	amrnb->func_GP3Decoder_Interface_Decode(st, bits, synth, bfi);
#else
	Decoder_Interface_Decode(st, bits, synth, bfi);
#endif
}

extern "C" void Decoder_Interface_exit_(ADM_LibWrapperAmrnb* amrnb, void* state)
{
#ifdef USE_LATE_BINDING
	amrnb->func_Decoder_Interface_exit(state);
#else
	Decoder_Interface_exit(state);
#endif
}

extern "C" int Encoder_Interface_Encode_(ADM_LibWrapperAmrnb* amrnb, void *st, enum Mode mode, short *speech, unsigned char *serial, int forceSpeech)
{
#ifdef USE_LATE_BINDING
	return amrnb->func_GP3VADxEncoder_Interface_Encode(st, mode, speech, serial, forceSpeech, 0);
#else
	return Encoder_Interface_Encode(st, mode, speech, serial, forceSpeech);
#endif
}

extern "C" void Encoder_Interface_exit_(ADM_LibWrapperAmrnb* amrnb, void* state)
{
#ifdef USE_LATE_BINDING
	amrnb->func_Encoder_Interface_exit(state);
#else
	Encoder_Interface_exit(state);
#endif
}

extern "C" void* Encoder_Interface_init_(ADM_LibWrapperAmrnb* amrnb, int dtx)
{
#ifdef USE_LATE_BINDING
	return amrnb->func_VADxEncoder_Interface_init(dtx, 0);
#else
	return Encoder_Interface_init(dtx);
#endif
}

#endif
