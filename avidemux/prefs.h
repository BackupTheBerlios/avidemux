#ifndef _preferences_h_
#define _preferences_h_

#include "ADM_library/default.h"

#define RC_OK     1
#define RC_FAILED 0

// <prefs_gen>
typedef enum {
	CODECS_MPEG2ENC_SVCD_MAXBITRATE,
	CODECS_MPEG2ENC_SVCD_QUANTISATION,
	CODECS_MPEG2ENC_SVCD_EXTRAPARAMS,
	CODECS_MPEG2ENC_DVD_MAXBITRATE,
	CODECS_MPEG2ENC_DVD_QUANTISATION,
	CODECS_MPEG2ENC_DVD_EXTRAPARAMS,
	TEST_STRING,
	TEST_INT,
	TEST_UINT,
	TEST_LONG,
	TEST_ULONG,
	TEST_FLOAT,
	CODECS_PREFERREDCODEC,
	DEVICE_AUDIODEVICE,
	LASTFILES_FILE1,
	LASTFILES_FILE2,
	LASTFILES_FILE3,
	LASTFILES_FILE4,
	LASTDIR,
	LAME_CLI,
	PIPE_CMD,
	PIPE_PARAM,
	LAME_PATH,
	TOOLAME_PATH,
	LVEMUX_PATH,
	REQUANT_PATH,
	FEATURE_SWAP_IF_A_GREATER_THAN_B,
	FEATURE_SVCDRES_PREFEREDSOURCERATIO,
	FEATURE_SAVEPREFSONEXIT
} options;
// </prefs_gen>

class preferences {
	private:
		char *internal_lastfiles[5];
		int save_xml_to_file();
	public:
		preferences();
		~preferences();
		int load();
		int save();
		int get(options option, unsigned int *val);
		int get(options option,          int *val);
		int get(options option, unsigned long *val);
		int get(options option,          long *val);
		int get(options option, float *val);
		int get(options option, char **val);
		int get(options option, uint8_t *val);
		int get(options option, uint16_t *val);
		const char * get_str_min(options option);
		const char * get_str_max(options option);
		// handled by compiler: const uint8_t is full handled by any 
		//    method that can handle const unsigned int
		// int set(options option, const uint8_t val);
		// int set(options option, const uint16_t val);
		int set(options option, const unsigned int val);
		int set(options option, const          int val);
		int set(options option, const unsigned long val);
		int set(options option, const          long val);
		int set(options option, const          float val);
		int set(options option, const          char * val);
		int set_lastfile(const char* file);
		const char **get_lastfiles(void);
};

extern preferences *prefs;

#endif
