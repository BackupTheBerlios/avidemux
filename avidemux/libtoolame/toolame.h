/*
 *  tooLAME: an optimized mpeg 1/2 layer 2 audio encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 */
 

#ifndef	_toolame_H_
#define	_toolame_H_

#ifndef FLOAT
#define FLOAT	double
#endif

#ifndef TRUE
#define TRUE	(1)
#endif

#ifndef FALSE
#define FALSE	(0)
#endif


/** MPEG modes */
typedef enum {
	STEREO = 0,		/**< Stereo */
	JOINT_STEREO,	/**< Joint Stereo */
	DUAL_CHANNEL,	/**< Dual Channel */
	MONO,			/**< Mono */
	NOT_SET
} MPEG_mode;


/** MPEG Version.
 *
 *	MPEG2 is for Lower Sampling Frequencies - LSF < 32000.
 */
typedef enum {
	MPEG2_LSF = 0,	/**< MPEG-2  - for sample rates less than 32k */
	MPEG1			/**< MPEG-1 */
} MPEG_version;


/** Padding types. */
typedef enum {
	PAD_NO = 0,		/**< No Padding */
	PAD_ALL			/**< Pad all frames */
//	PAD_ADJUST		// unsupported by toolame
} Padding_type;

/** Emphasis types. */
typedef enum {
	EMPHASIS_N = 0,	/**< No Emphasis */
	EMPHASIS_5 = 1,	/**< 50/15 ms */
					// reserved
	EMPHASIS_C = 3,	/**< CCIT J.17 */ 
} Emphasis_type;


/** Opaque structure for the toolame encoder options. */
struct toolame_options_struct;

/** Opaque data type for the toolame encoder options. */
typedef struct toolame_options_struct toolame_options;





/** Get version number of the toolame library. 
 *  eg "0.2n".
 *
 *  \return The version number as a string
 */
const char* get_toolame_version( void );


/** Initialise the toolame encoder.
 *
 *  Sets defaults for all parameters.
 *	Will return NULL if malloc() failed, otherwise 
 *	returns a pointer which you then need to pass to 
 *	all future API calls.
 *  
 *  \return a pointer to your new options data structure
 */
toolame_options *toolame_init(void);


/** Prepare to start encoding.
 *
 *  You must call toolame_init_params() before you start encoding.
 *	It will check call your parameters to make sure they are valid,
 *  as well as allocating buffers and initising internally used
 *	variables.
 *  
 *  \param glopts 		Options pointer created by toolame_init()
 *  \return 			0 if all patameters are valid, 
 *						non-zero if something is invalid
 */
int toolame_init_params(toolame_options *glopts);


/** Encode some PCM audio to MP2.
 *
 *  Takes 16-bit PCM audio samples from seperate left and right
 *  buffers and places encoded audio into mp2buffer.
 *  
 *  \param glopts 			toolame options pointer
 *  \param leftpcm			Left channel audio samples
 *  \param rightpcm			Right channel audio samples
 *  \param num_samples		Number of samples per channel
 *  \param mp2buffer		Buffer to place encoded audio into
 *  \param mp2buffer_size	Size of the output buffer
 *  \return 				The number of bytes put in output buffer
 */
int toolame_encode_buffer(
		toolame_options *glopts,
		const short int leftpcm[],
		const short int rightpcm[],
		int num_samples,
		unsigned char *mp2buffer,
		int mp2buffer_size );


/** Encode some PCM audio to MP2.
 *
 *  Takes interleaved 16-bit PCM audio samples from a single 
 *  buffer and places encoded audio into mp2buffer.
 *  
 *  \param glopts 			toolame options pointer
 *  \param pcm				Audio samples for left AND right channels
 *  \param num_samples		Number of samples per channel
 *  \param mp2buffer		Buffer to place encoded audio into
 *  \param mp2buffer_size	Size of the output buffer
 *  \return 				The number of bytes put in output buffer
 */
int toolame_encode_buffer_interleaved(
		toolame_options *glopts,
		const short int pcm[],
		int num_samples,
		unsigned char *mp2buffer,
		int mp2buffer_size );


/** Encode any remains buffered PCM audio to MP2.
 *
 *  Encodes any remaining audio samples in the libtoolame
 *  internal sample buffer. This function will return at
 *  most a single frame of MPEG Audio, and at least 0 frames.
 *  
 *  \param glopts 			toolame options pointer
 *  \param mp2buffer		Buffer to place encoded audio into
 *  \param mp2buffer_size	Size of the output buffer
 *  \return 				The number of bytes put in output buffer
 */
int toolame_encode_flush(
		toolame_options *glopts,
		unsigned char *mp2buffer,
		int mp2buffer_size);


/** Shut down the toolame encoder.
 *
 *  Shuts down the toolame encoder and frees all memory
 *  that it previously allocated. You should call this
 *  once you have finished all your encoding. This function
 *  will set your glopts pointer to NULL for you.
 *
 *  \param glopts 			pointer to toolame options pointer
 */
void toolame_close(toolame_options **glopts);



/** Set the verbosity of the encoder.
 *
 *  Sets how verbose the encoder is with the debug and
 *  informational messages it displays. The higher the
 *  number, the more messages it will display.
 *
 *	Default: 2
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param verbosity 		integer between 0 and 100
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_verbosity(toolame_options *glopts, int verbosity);


/** Get the verbosity of the encoder.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			integer indicating the verbosity of the encoder
 */
int toolame_get_verbosity(toolame_options *glopts);


/** Get the bitrate index for a bitrate.
 *
 *  Returns the index associated with a bitrate for
 *	the specified version of MPEG.
 *
 *  \param version 			The version of MPEG 
 *							[MPEG1 or MPEG2_LSF]
 *  \param bitrate 			The bitrate to lookup in kbps
 *  \return 				The bitrate index if successful, 
 *							-1 if invalid bitrate
 */
int toolame_get_bitrate_index(MPEG_version version, int bitrate);


/** Get the sample rate index for a samplerate.
 *
 *  Returns the index associated with a bitrate for
 *	the specified version of MPEG.
 *
 *  \param sampleRate 		The sample rate to lookup (Hz)
 *  \param version 			The version of MPEG to use is returned in this integer pointer
 *  \return 				The sample rate index if successful, 
 *							-1 if invalid sample rate
 */
int toolame_get_samplerate_index(long sampleRate, MPEG_version *version);


/** Set the MPEG Audio Mode (Mono, Stereo, etc) for 
 *  the output stream.
 *
 *	Default: STEREO
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param mode 			the mode to set to
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_mode(toolame_options *glopts, MPEG_mode mode);


/** Get the MPEG Audio mode of the output stream.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the MPEG audio mode
 */
MPEG_mode toolame_get_mode(toolame_options *glopts);


/** Get a string name for the current MPEG Audio mode.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the name of the MPEG audio mode as a string
 */
const char *toolame_get_mode_name(toolame_options *glopts);


/** Set the MPEG version of the MPEG audio stream. 
 *
 *	Default: MPEG1
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param version 			the version to set to
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_version(toolame_options *glopts, MPEG_version version);


/** Get the MPEG version of the output stream.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the MPEG version
 */
MPEG_version toolame_get_version(toolame_options *glopts);


/** Get a string name for the current MPEG version.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the name of the MPEG version as a string
 */
const char *toolame_get_version_name( toolame_options *glopts );


/** Set the Psychoacoustic Model used to encode the audio.
 *
 *	Default: 3
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param psymodel 		the psychoacoustic model number
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_psymodel(toolame_options *glopts, int psymodel);


/** Get the Psychoacoustic Model used to encode the audio.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the psychoacoustic model number
 */
int toolame_get_psymodel(toolame_options *glopts);


/** Set the number of channels in the input stream.
 *
 *  If this is different the number of channels in
 *  the output stream (set by mode) then the encoder
 *  will automatically downmix/upmix the audio.
 *
 *	Default: 2
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param num_channels 	the number of input channels
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_num_channels(toolame_options* glopts, int num_channels);


/** Get the number of channels in the input stream.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the number of channels
 */
int toolame_get_num_channels(toolame_options* glopts);


/** Set the samplerate of the PCM audio input.
 *
 *	Default: 44100
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param samplerate	 	the samplerate in Hz
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_in_samplerate(toolame_options *glopts, int samplerate);


/** Get the samplerate of the PCM audio input.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the input samplerate
 */
int toolame_get_in_samplerate(toolame_options *glopts);


/** Set the samplerate of the MPEG audio output.
 *
 *	Default: 44100
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param samplerate	 	the samplerate in Hz
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_out_samplerate(toolame_options *glopts, int samplerate);


/** Get the samplerate of the MPEG audio output.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the output samplerate
 */
int toolame_get_out_samplerate(toolame_options *glopts);


/** Set the bitrate of the MPEG audio output stream.
 *
 *	Default: 192
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param bitrate		 	the bitrate in kbps
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_bitrate(toolame_options *glopts, int bitrate);


/** Get the bitrate of the MPEG audio output.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the output bitrate in kbps
 */
int toolame_get_bitrate(toolame_options *glopts);


/** Set the bitrate of the MPEG audio output stream (LAME style).
 *
 *  same as toolame_set_bitrate()
 */
int toolame_set_brate(toolame_options *glopts, int bitrate);


/** Get the bitrate of the MPEG audio output stream (LAME style).
 *
 *  same as toolame_get_bitrate()
 */
int toolame_get_brate(toolame_options *glopts);


/** Set frame padding for the MPEG audio output stream.
 *
 *  i.e. adjust frame sizes to achieve overall target bitrate
 *
 *	Default: PAD_NO
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param padding	 		the padding type
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_padding(toolame_options *glopts, Padding_type padding);

/** Get the padding type of the MPEG audio output.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the output bitrate in kbps
 */
Padding_type toolame_get_padding(toolame_options *glopts);


/** Enable/Disable Energy Level Extension.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param energylevels	 	energy level extension state
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_energy_levels(toolame_options *glopts, int energylevels );


/** Get the Energy Level Extension state.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			state of the Energy Level Extension (TRUE/FALSE)
 */
int toolame_get_energy_levels(toolame_options *glopts);


/** Set number of Ancillary Bits at end of frame.
 *
 *	Default: 0
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param num			 	number of bits to reserve
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_num_ancillary_bits(toolame_options *glopts, int num);


/** Get the number of Ancillary Bits at end of frame.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			number of Ancillary Bits at end of frame
 */
int toolame_get_num_ancillary_bits(toolame_options *glopts);



/** Set the type of pre-emphasis to be applied to the decoded audio.
 *
 *	Default: EMPHASIS_N
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param emphasis			the type of pre-emphasis
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_emphasis(toolame_options *glopts, Emphasis_type emphasis);


/** Get the type of pre-emphasis to be applied to the decoded audio.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the type of pre-emphasis
 */
Emphasis_type toolame_get_emphasis(toolame_options *glopts);


/** Enable/Disable CRC Error Protection.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param err_protection	error protection state (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_error_protection(toolame_options *glopts, int err_protection);


/** Get the CRC Error Protection state.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			state of Error Protection (TRUE/FALSE)
 */
int toolame_get_error_protection(toolame_options *glopts);


/** Set the MPEG Audio Copyright flag.
 *
 *  Indicates that MPEG stream is copyrighted.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param copyright		copyright flag state (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_copyright(toolame_options *glopts, int copyright);


/** Get the copright flag state
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			state of the copyright flag (TRUE/FALSE)
 */
int toolame_get_copyright(toolame_options *glopts);


/** Set the MPEG Audio Original flag.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param original			original flag state (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_original(toolame_options *glopts, int original);


/** Get the origianl flag state.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			state of the original flag (TRUE/FALSE)
 */
int toolame_get_original(toolame_options *glopts);


/** Enable/Disable VBR (Variable Bit Rate) mode.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param vbr				VBR state (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_VBR(toolame_options *glopts, int vbr);


/** Get the VBR state.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			state of VBR (TRUE/FALSE)
 */
int toolame_get_VBR(toolame_options *glopts);


/** Set the level/quality of the VBR audio.
 *
 *  The level value can is a measurement of quality - the higher 
 *  the level the higher the average bitrate of the resultant file.
 *
 *	Default: 0.0
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param level			quality level (-? -> ?)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_VBR_q(toolame_options *glopts, FLOAT level);


/** Get the level/quality of the VBR audio.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			quality value for VBR 
 */
FLOAT toolame_get_VBR_q(toolame_options *glopts);


/** Set the adjustment (in dB) applied to the ATH for Psycho models 3 and 4.
 *
 *	Default: 0.0
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param level			adjustment level in db
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_ATH_level(toolame_options *glopts, FLOAT level);


/** Get the adjustment (in dB) applied to the ATH for Psycho models 3 and 4.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			adjustment level in db
 */
FLOAT toolame_get_ATH_level(toolame_options *glopts);


/** Set the upper bitrate for VBR
 *
 *	Default: 0 (off)
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param bitrate			upper bitrate for VBR
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_VBR_max_bitrate_kbps(toolame_options *glopts, int bitrate);

/** Get the upper bitrate for VBR.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the upper bitrate for VBR
 */
int toolame_get_VBR_max_bitrate_kbps(toolame_options *glopts);


/** Enable/Disable the quick mode for psycho model calculation.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param quickmode		the state of quick mode (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_quick_mode(toolame_options *glopts, int quickmode);

/** Get the state of quick mode.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the state of quick mode (TRUE/FALSE)
 */
int toolame_get_quick_mode(toolame_options *glopts);


/** Set how often the psy model is calculated.
 *
 *	Default: 10
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param quickcount		number of frames between calculations
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_quick_count(toolame_options *glopts, int quickcount );

/** Get the how often the psy model is calculated.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			number of frames between calculations
 */
int toolame_get_quick_count(toolame_options *glopts);


/** Enable/Disable the Eureka 147 DAB extensions for MP2.
 *
 *	Default: FALSE
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param dab			 	state of DAB extensions (TRUE/FALSE)
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_DAB(toolame_options *glopts, int dab);

/** Get the state of the DAB extensions
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			the state of DAB (TRUE/FALSE)
 */
int toolame_get_DAB(toolame_options *glopts);


/** Set the number of bytes to reserve for DAB XPAD data.
 *
 *	Default: 0
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param length			number of bytes to reserve
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_DAB_xpad_length(toolame_options *glopts, int length);


/** Get the number of bytes reserved for DAB XPAD data.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			number of XPAD bytes
 */
int toolame_get_DAB_xpad_length(toolame_options *glopts);


/** Set the CRC error protection length for DAB.
 *
 *	Default: 2
 *
 *  \param glopts 			pointer to toolame options pointer
 *  \param length			length of DAB CRC
 *  \return 				0 if successful, 
 *							non-zero on failure
 */
int toolame_set_DAB_crc_length(toolame_options *glopts, int length);


/** Get the CRC error protection length for DAB.
 *
 *  \param glopts 	pointer to toolame options pointer
 *  \return			length of DAB CRC
 */
int toolame_get_DAB_crc_length(toolame_options *glopts);


#endif


