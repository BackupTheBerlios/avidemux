//
//
//
#ifndef MPEG2ENC_PARM
#define MPEG2ENC_PARM

typedef struct Mpeg2encParam
{	
	unsigned int	maxBitrate;
	unsigned int	gop_size;	
	unsigned int	widescreen;
	unsigned int	user_matrix;
	unsigned int	interlaced;
	unsigned int	bff;

}Mpeg2encParam;


#endif
