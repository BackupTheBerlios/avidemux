//
//	Use by TS scanner / match Pid etc...
//

#ifndef ADM_TS
#define ADM_TS
	typedef struct ADM_TsTrackInfo
	{
		uint16_t pid;
		uint16_t es;
		uint64_t pts;	
	};
uint8_t ADM_TsParse(char *file,uint32_t *nbTrack, ADM_TsTrackInfo **token);
#endif
