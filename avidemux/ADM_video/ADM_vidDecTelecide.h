/*
	Telecide plugin for Avisynth -- recovers original progressive
	frames from  telecined streams. The filter operates by matching
	fields and automatically adapts to phase/pattern changes.

	Copyright (C) 2003 Donald A. Graft

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#undef DEBUG_PATTERN_GUIDANCE

#undef WINDOWED_MATCH

#define MAX_CYCLE 6
#define BLKSIZE 24
#define BLKSIZE_TIMES2 (2 * BLKSIZE)
#define GUIDE_NONE 0
#define GUIDE_32 1
#define GUIDE_22 2
#define GUIDE_32322 3
#define AHEAD 0
#define BEHIND 1
#define POST_NONE 0
#define POST_METRICS 1
#define POST_FULL 2
#define POST_FULL_MAP 3
#define POST_FULL_NOMATCH 4
#define POST_FULL_NOMATCH_MAP 5
#define CACHE_SIZE 100000
#define P 0
#define C 1
#define N 2
#define PBLOCK 3
#define CBLOCK 4

#define NO_BACK 0
#define BACK_ON_COMBED 1
#define ALWAYS_BACK 2

#define OutputDebugString(x) printf("%s\n",x)
typedef uint8_t* PVideoFrame ;


struct CACHE_ENTRY
{
	unsigned int frame;
	unsigned int metrics[5];
	unsigned int chosen;
};

struct PREDICTION
{
	unsigned int metric;
	unsigned int phase;
	unsigned int predicted;
	unsigned int predicted_metric;
};

#define GETFRAME(g, fp) \
{ \
	int GETFRAMEf; \
	uint32_t len,flags;\
	GETFRAMEf = (g); \
	if (GETFRAMEf < 0) GETFRAMEf = 0; \
	else if (GETFRAMEf > _info.nb_frames) GETFRAMEf = _info.nb_frames - 1; \
	getFrameNumberNoAlloc(GETFRAMEf, &len, \
				fp,&flags); \
}

typedef struct TelecideParam
{
	int 	order;
	int 	back;
	int 	guide;
	float 	gthresh;
	int 	post;
	bool 	chroma;
	float 	vthresh;
	float 	bthresh;
	float 	dthresh;
	bool 	blend;
	int 	nt;
	int 	y0;
	int 	y1;
	bool 	hints;
	bool 	show;
	bool 	debug;
};

class Telecide : public AVDMGenericVideoStream
{
private:
	TelecideParam *_param;
		
	bool tff;
	
	float  	vthresh_saved;
	int 	back_saved;
	int pitch, dpitch, pitchover2, pitchtimes4;
	int w, h, wover2, hover2, hplus1over2, hminus2;
	int xblocks, yblocks;
#ifdef WINDOWED_MATCH
	unsigned int *matchc, *matchp, highest_matchc, highest_matchp;
#endif
	unsigned int *sumc, *sump, highest_sumc, highest_sump;
	int vmetric;
	
	bool film, override, inpattern, found;
	int force;

	// Used by field matching.
	PVideoFrame fp, fc, fn, dst, final;
	unsigned char *fprp, *fcrp, *fcrp_saved, *fnrp;
	unsigned char *fprpU, *fcrpU, *fcrp_savedU, *fnrpU;
	unsigned char *fprpV, *fcrpV, *fcrp_savedV, *fnrpV;
	unsigned char *dstp, *finalp;
	unsigned char *dstpU, *dstpV;
	int chosen;
	unsigned int p, c, pblock, cblock, lowest, predicted, predicted_metric;
	unsigned int np, nc, npblock, ncblock, nframe;
	float mismatch;
	int pframe, x, y;
	PVideoFrame lc, lp;
	unsigned char *crp, *prp;
	unsigned char *crpU, *prpU;
	unsigned char *crpV, *prpV;
	bool hard;
	char status[80];

	// Metrics cache.
	struct CACHE_ENTRY *cache;

	// Pattern guidance data.
	int cycle;
	struct PREDICTION pred[MAX_CYCLE+1];

	// For output message formatting.
	char buf[255];

public:
	
	void Telecide::CalculateMetrics(int n, unsigned char *crp, unsigned char *crpU, unsigned char *crpV, 
				unsigned char *prp, unsigned char *prpU, unsigned char *prpV);
	void Telecide::Show(PVideoFrame &dst, int frame);
	void Telecide::Debug(int frame);

	Telecide::Telecide(AVDMGenericVideoStream *in,CONFcouple *couples);		
			
	Telecide::~Telecide();

	void Telecide::PutChosen(int frame, unsigned int chosen);
	

	void Telecide::CacheInsert(int frame, unsigned int p, unsigned int pblock,
				unsigned int c, unsigned int cblock);
	
	bool Telecide::CacheQuery(int frame, unsigned int *p, unsigned int *pblock,
				unsigned int *c, unsigned int *cblock);	

	bool Telecide::PredictHardYUY2(int frame, unsigned int *predicted, unsigned int *predicted_metric) ;
	
	struct PREDICTION *Telecide::PredictSoftYUY2(int frame);

	void Telecide::WriteHints(unsigned char *dst, bool film, bool inpattern);
	virtual uint8_t Telecide::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
				uint8_t *data,uint32_t *flags);
	char *Telecide::printConf( void );
	uint8_t Telecide::configure(AVDMGenericVideoStream *in);
};
