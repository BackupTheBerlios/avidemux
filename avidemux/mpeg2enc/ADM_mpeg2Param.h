//
//
//
#ifndef MPEG2ENC_PARM
#define MPEG2ENC_PARM

typedef struct Mpeg2encParam
{	
	int	maxBitrate;
	int	gop_size;	
	int	widescreen;
	int	user_matrix;
	int	interlaced;
	int	bff;

}Mpeg2encParam;


#endif
