#ifndef TOOLAME_H
#define TOOLAME_H

#ifdef __cplusplus
extern "C"{
#endif

#ifndef FLOAT
#define FLOAT double
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

/* MPEG Header Definitions - Mode Values */
#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3

/* Version. MPEG2 is for Lower Sampling Frequencies - LSF < 32000 */
#define MPEG2_LSF 0
#define MPEG1     1

struct toolame_options_struct;
typedef struct toolame_options_struct toolame_options;

/* The main functions of this library */
toolame_options *toolame_init(void);
int toolame_init_params(toolame_options *glopts);
//MEANX
int toolame_encode_buffer_interleaved(toolame_options *glopts, short int interpcm[],
			  int num_samples, unsigned char *mp2buffer, int mp2buffer_size, 
			  int *mp2fill_size);
//MEANX
int toolame_encode_buffer(toolame_options *glopts, short int leftpcm[], short int rightpcm[],
			  int num_samples, unsigned char *mp2buffer, int mp2buffer_size, 
			  int *mp2fill_size);
int toolame_encode_flush(toolame_options *glopts, unsigned char *mp2buffer, int mp2buffer_size, int *mp2fill_size);
void toolame_deinit(toolame_options *glopts);


/* MFC July 2003: generic get/set functions for MP2 encoding. */
char *toolame_getModeName(int mode);
char *toolame_getVersionName(int version);
int toolame_getBitrateIndex (int version, int bitrate);// Given the bitrate in kbps, return the bitrateindex
int toolame_getSmpFrqIndex (long sampleRate, int *version);//Given a samplerate, return the correct mpegaudio index
void toolame_newExtension(char *inpath, char *extension, char *outpath);


int toolame_js_bound (int modeext);
void toolame_hdr_to_frps (toolame_options *glopts);


int toolame_setPsymodel (toolame_options *glopts, int psymodel);
int toolame_getPsymodel (toolame_options *glopts);

// Set Sampling Frequency of Input PCM audio 
//  mpeg1: 32000 [44100] 48000 
//  mpeg2: 16000  22050  24000  */
int toolame_setSampleFreq (toolame_options *glopts, int sampling_frequency);
int toolame_getSampleFreq (toolame_options *glopts);

// Set the bitrate for the audio
//  for mpeg1:32, 48, 56, 64, 80, 96,112,128,160,[192], 224, 256, 320, 384 
//  for mpeg2: 8, 16, 24, 32, 40, 48, 56, 64, 80, [96], 112, 128, 144, 160 
int toolame_setBitrate (toolame_options *glopts, int bitrate);
int toolame_getBitrate (toolame_options *glopts);

// MPEG Audio Mode
// [MPG_MD_STEREO] MPG_MD_DUAL_CHANNEL  MPG_MD_JOINT_STEREO  MPG_MD_MONO 
int toolame_setMode (toolame_options *glopts, int mode);
int toolame_getMode (toolame_options *glopts);

// Frame padding. i.e. adjust frame sizes to achieve overall target bitrate
// [FALSE]
int toolame_setPadding (toolame_options *glopts, int padding);
int toolame_getPadding (toolame_options *glopts);

// Energy Level Extensions
// [FALSE]
int toolame_setEnergyLevels (toolame_options *glopts, int energylevels );
int toolame_getEnergyLevels (toolame_options *glopts);

// Reserve Ancillary Bits at end of frame
// [0]
int toolame_setNumAncillaryBits (toolame_options *glopts, int num);
int toolame_getNumAncillaryBits (toolame_options *glopts);

// Set the type of pre-emphasis to be applied to the decoded audio
// [0]none, 1, 3
int toolame_setEmphasis (toolame_options *glopts, int emphasis) ;
int toolame_getEmphasis (toolame_options *glopts);

// Use CRC error protection
// [FALSE]
int toolame_setErrorProtection (toolame_options *glopts, int error_protection);
int toolame_getErrorProtection (toolame_options *glopts);

// Set the copyright flag
// [FALSE]
int toolame_setCopyright (toolame_options *glopts, int copyright);
int toolame_getCopyright (toolame_options *glopts);

// Set the original flag
// [FALSE]
int toolame_setOriginal (toolame_options *glopts, int original) ;
int toolame_getOriginal (toolame_options *glopts);

// Set the VBR flag
// [FALSE]
int toolame_setVBR (toolame_options *glopts, int vbr) ;
int toolame_getVBR (toolame_options *glopts) ;

// Set the level/quality of the VBR audio
// [0.0]
int toolame_setVBRLevel (toolame_options *glopts, FLOAT level);
FLOAT toolame_getVBRLevel (toolame_options *glopts);

// Set the adjustment (in dB) applied to the ATH for Psycho3/4
// [0.0]
int toolame_setATHLevel (toolame_options *glopts, FLOAT level);
FLOAT toolame_getATHLevel (toolame_options *glopts);

// Set the verbosity of the library
// [2]
int toolame_setVerbosity (toolame_options *glopts, int verbosity);
int toolame_getVerbosity (toolame_options *glopts);

// Set the upper bitrate in VBR mode
// [0]=off
int toolame_setVBRUpperBitrate (toolame_options *glopts, int bitrate);
int toolame_getVBRUpperBitrate (toolame_options *glopts);

// Set the quickmode for psycho model calculation
// [FALSE]
int toolame_setQuickMode (toolame_options *glopts, int quickmode);
int toolame_getQuickMode (toolame_options *glopts);

// Set how often the psy model is calculated
// [0]
int toolame_setQuickCount (toolame_options *glopts, int quickcount );
int toolame_getQuickCount (toolame_options *glopts);

// Set the MPEG version
// 0 - MPEG2 LSF  [1] - MPEG1
int toolame_setVersion (toolame_options *glopts, int version);
int toolame_getVersion (toolame_options *glopts);

// Set the DAB flag
// [FALSE]
int toolame_setDAB (toolame_options *glopts, int dab);
int toolame_getDAB (toolame_options *glopts);

// Set the DAB XPAD length
// [0]
int toolame_setDABXPADLength (toolame_options *glopts, int length);
int toolame_getDABXPADLength (toolame_options *glopts);

// Set the DAB CRC Length
// [2]
int toolame_setDABCRCLength (toolame_options *glopts, int length);
int toolame_getDABCRCLength (toolame_options *glopts);

#ifdef __cplusplus
};
#endif

#endif
