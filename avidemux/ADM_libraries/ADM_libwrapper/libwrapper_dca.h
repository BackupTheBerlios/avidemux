#ifndef LIBWRAPPER_DCA
#define LIBWRAPPER_DCA

#ifdef USE_LIBDCA
#include "libwrapper.h"

extern "C"
{
#include "dts.h"
#include "dts_internal.h"
}

class ADM_LibWrapperDca : public ADM_LibWrapper
{
	protected:
	#ifdef USE_LATE_BINDING
		int (*func_dts_block)(dts_state_t* state);
		int (*func_dts_blocks_num) (dts_state_t* state);
		int (*func_dts_frame) (dts_state_t* state, uint8_t* buf, int* flags, level_t* level, sample_t bias);
		void (*func_dts_free) (dts_state_t* state);
		dts_state_t* (*func_dts_init) (uint32_t mm_accel);
		sample_t* (*func_dts_samples) (dts_state_t* state);
		int (*func_dts_syncinfo) (dts_state_t* state, uint8_t* buf, int* flags, int* sample_rate, int* bit_rate, int* frame_length);
	#endif

	public:
		ADM_LibWrapperDca();
		virtual	~ADM_LibWrapperDca();
		int dts_block_ (dts_state_t* state);
		int dts_blocks_num_ (dts_state_t* state);
		int dts_frame_ (dts_state_t* state, uint8_t* buf, int* flags, level_t* level, sample_t bias);
		void dts_free_ (dts_state_t* state);
		dts_state_t* dts_init_ (uint32_t mm_accel);
		sample_t* dts_samples_ (dts_state_t* state);
		int dts_syncinfo_ (dts_state_t* state, uint8_t* buf, int* flags, int* sample_rate, int* bit_rate, int* frame_length);
};
#endif
#endif
