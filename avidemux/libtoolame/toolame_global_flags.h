#ifndef TOOLAME_GLOBAL_H
#define TOOLAME_GLOBAL_H

/***************************************************************************************
psycho 1 mem struct
****************************************************************************************/

typedef struct {
  int line;
  FLOAT bark, hear, x;
} g_thres, *g_ptr;

typedef struct {
  FLOAT x;
  int type, next, map;
} mask, *mask_ptr;

typedef struct psycho_1_mem_struct {
  int off[2];
  FLOAT fft_buf[2][1408];
  int *cbound;
  int crit_band;
  int sub_size;
  mask_ptr power;
  g_ptr ltg;
} psycho_1_mem;

/***************************************************************************************
Psycho3 memory structure
****************************************************************************************/
#define HBLKSIZE 513

#define SUBSIZE 136
typedef struct psycho_3_mem_struct{
  int off[2];
  int freq_subset[SUBSIZE];
  FLOAT bark[HBLKSIZE];
  FLOAT ath[HBLKSIZE];
  FLOAT fft_buf[2][1408];
#define CRITBANDMAX 32 /* this is much higher than it needs to be. really only about 24 */
  int cbands; /* How many critical bands there really are */
  int cbandindex[CRITBANDMAX]; /* The spectral line index of the start of
				each critical band */
} psycho_3_mem;



/***************************************************************************************
Psycho2 & 4 memory structure
****************************************************************************************/

#define LOGBLKSIZE      10
#define BLKSIZE         1024
#define HBLKSIZE        513
#define CBANDS          64
typedef int ICB[CBANDS];
typedef int IHBLK[HBLKSIZE];
typedef FLOAT F32[32];
typedef FLOAT F2_32[2][32];
typedef FLOAT FCB[CBANDS];
typedef FLOAT FCBCB[CBANDS][CBANDS];
typedef FLOAT FBLK[BLKSIZE];
typedef FLOAT FHBLK[HBLKSIZE];
typedef FLOAT F2HBLK[2][HBLKSIZE];
typedef FLOAT F22HBLK[2][2][HBLKSIZE];
typedef FLOAT DCB[CBANDS];

typedef struct psycho_4_mem_struct {
  int new;
  int old;
  int oldest;

  FLOAT grouped_c[CBANDS];
  FLOAT grouped_e[CBANDS];
  FLOAT nb[CBANDS];
  FLOAT cb[CBANDS];
  FLOAT tb[CBANDS];
  FLOAT ecb[CBANDS];
  FLOAT bc[CBANDS];
  FLOAT cbval[CBANDS];
  FLOAT rnorm[CBANDS];
  FLOAT wsamp_r[BLKSIZE], phi[BLKSIZE], energy[BLKSIZE], window[BLKSIZE];
  FLOAT ath[HBLKSIZE], thr[HBLKSIZE], c[HBLKSIZE];
  FLOAT fthr[HBLKSIZE], absthr[HBLKSIZE]; // psy2 only
  int numlines[CBANDS];
  int partition[HBLKSIZE];
  FLOAT *tmn;
  FCB *s;
  FHBLK *lthr;
  F2HBLK *r, *phi_sav;
  FLOAT snrtmp[2][32];
} psycho_4_mem, psycho_2_mem;


/***************************************************************************************
 Header and frame information
 MFC July 03: All this stuff should be hidden from the user
 FIX: Just expose to the user functions for changing values.
      e.g. int toolame_setBitrate(toolame_options *options, int bitrate)
****************************************************************************************/

/* Structure for Reading Layer II Allocation Tables from File */
typedef struct {
  unsigned int steps;
  unsigned int bits;
  unsigned int group;
  unsigned int quant;
} sb_alloc, *alloc_ptr;

typedef sb_alloc al_table[32][16];

/* Header Information Structure */
typedef struct {
  int version;
  int lay;
  int error_protection;
  int bitrate_index;
  int sampling_frequency_idx;
  int sampling_frequency;
  int padding;
  int private_bit;
  int mode;
  int mode_ext;
  int copyright;
  int original;
  int emphasis;
} frame_header;

/* Parent Structure Interpreting some Frame Parameters in Header */
typedef struct {
  frame_header *header;		/* raw header information */
  int actual_mode;		/* when writing IS, may forget if 0 chs */
  al_table *alloc;		/* bit allocation table read in */
  int tab_num;			/* number of table as loaded */
  int nch;			/* num channels: 1 for mono, 2 for stereo */
  int jsbound;			/* first band of joint stereo coding */
  int sblimit;			/* total number of sub bands */
} frame_info;

/***************************************************************************************
 toolame Global Options structure.
 Defaults shown in []
 ++ means it is an advanced option. Only use it if you know what you're doing.    
****************************************************************************************/
struct toolame_options_struct
{
  // Input PCM audio File Information
  int sampling_frequency;   // mpeg1: 32000 [44100] 48000 
			    // mpeg2: 16000  22050  24000  
  int sampling_frequency_idx;// ++ [0] NB User sets samplerate, toolame converts it to index in initOptions

  // Output MP2 File Information
  int version;              //  0 mpeg2  [1] mpeg1                                 
  int bitrate;              //  for mpeg1:32, 48, 56, 64, 80, 96,112,128,160,[192], 224, 256, 320, 384 
		            //  for mpeg2: 8, 16, 24, 32, 40, 48, 56, 64, 80, [96], 112, 128, 144, 160 
  int bitrate_index;        // ++ [10]. NB User sets bitrate, toolame converts it to index in initOptions
  int mode;                /* [MPG_MD_STEREO]
			      MPG_MD_DUAL_CHANNEL
			      MPG_MD_JOINT_STEREO
			      MPG_MD_MONO */
  int padding;             // [FALSE] 
  int doEnergyLevels;      // Write energy level information into the end of the frame [FALSE]
  int numAncillaryBits;    // Number of reserved ancillary bits [0] (Currently only available for non-VBR modes)

  // Psychoacoustic Model options
  int psymodel;            // -1, 0, 1, 2, [3], 4   Psy model number
  FLOAT athlevel;          // Adjust the Absolute Threshold of Hearing curve by [0] dB
  int quickmode;           // Only calculate psy model ever X frames [FALSE] 
  int quickcount;          // Only calculate psy model every [10] frames

  // VBR Options
  int vbr;                 // turn on VBR mode TRUE [FALSE] 
  int upperbitrateindex;   // ++ [0] means no upper bitrate set for VBR mode. valid 1-15 depending on mode
  FLOAT vbrlevel;          // Set VBR quality. [0.0] (sensible range -10.0 -> 10.0)

  // Miscellaneous Options That Nobody Ever Uses
  int emphasis;            // [n]one, 5(50/15 microseconds), c(ccitt j.17)  
  int copyright;           // [FALSE] 
  int original;            // [FALSE] 
  int private_bit;         // [0] Your very own bit in the header.
  int error_protection;    // [FALSE] 

  // Digital Audio Broadcasting Extensions
  int doDAB;               // Allocate space for the DigitalAudioBroadcasting info [FALSE] 
  int dabCrcLength;        // Number of CRC bytes for DAB [2], 4 
  int dabCrc[4];           // DAB CRC bytes are inserted here. User must insert them in frame
  int dabXpadLength;       // Number of bytes in the XPAD

  // Processing Options
  int verbosity;           // Verbosity of output 0(never output a thing) [2] 100(output everything)


  // Unless you're me, don't touch anything below here
  // Init flags for contexts
  int toolame_init;
  int psycho1init;
  psycho_1_mem *p1mem;
  int psycho2init;
  psycho_2_mem *p2mem;
  int psycho3init;
  psycho_3_mem *p3mem;
  int psycho4init;
  psycho_4_mem *p4mem;

  // Frame info
  frame_info frame;
  frame_header header;

};

#endif
