#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "toolame.h"
#include "toolame_global_flags.h"
#include "common.h"
#include "encoder.h"
#include "musicin.h"
#include "bitbuffer.h"
#include "mem.h"
#include "crc.h"
#include "psycho_n1.h"
#include "psycho_0.h"
#include "psycho_1.h"
#include "psycho_2.h"
#include "psycho_3.h"
#include "psycho_4.h"
#include "availbits.h"
#include "subband.h"
#include "encode_new.h"
#include "energy.h"
#include "dab.h"
#include "tables.h"

#include <assert.h>

void optionsToHeader(toolame_options *glopts, frame_header *header) {
  /* use the options to create header info structure */
  header->lay = 2;
  header->error_protection = glopts->error_protection;

  if ((glopts->bitrate_index = toolame_getBitrateIndex(glopts->version, glopts->bitrate)) < 0) {
    fprintf(stdout,"Not a valid bitrate for this version: %i\n",glopts->bitrate);
    exit(99);
  }
  header->bitrate_index = glopts->bitrate_index;
  // Convert the sampling frequency to the required index
  glopts->sampling_frequency_idx = toolame_getSmpFrqIndex ((long) glopts->sampling_frequency, &glopts->version); 
  header->sampling_frequency_idx = glopts->sampling_frequency_idx;

  header->sampling_frequency = glopts->sampling_frequency;
  if (header->sampling_frequency < 31000)
    header->version = glopts->version = 0;
  else
    header->version = glopts->version = 1;

  header->padding = glopts->padding;
  header->private_bit = glopts->private_bit;
  header->mode = glopts->mode;
  header->mode_ext = 0;
  header->copyright = glopts->copyright;
  header->original = glopts->original;
  header->emphasis = glopts->emphasis;  
}

/*
  toolame_init
  Create a set of encoding options and return
  a pointer to this structure
*/
toolame_options *toolame_init(void) {
  toolame_options *newoptions;  
  newoptions = (toolame_options *)calloc(1, sizeof(toolame_options));

  newoptions->version = 1; /* default to 32/44.1/48 kHz input. i.e. mpeg-1 */
  newoptions->bitrate_index = 10; /* 192 kbps */
  newoptions->sampling_frequency_idx = 0; /* 0 is 44.1 or 22.05 kHz depending on 'version' */
  newoptions->sampling_frequency = 44100;

  newoptions->mode = MPG_MD_STEREO;
  newoptions->psymodel = 3;
  newoptions->bitrate = 192;
  newoptions->vbr = FALSE;
  newoptions->vbrlevel = 0;
  newoptions->athlevel = 0;

  newoptions->quickmode = FALSE;
  newoptions->quickcount = 10;
  newoptions->emphasis = 0;
  newoptions->private_bit = 0;
  newoptions->copyright = FALSE;
  newoptions->original = FALSE;
  newoptions->error_protection = FALSE;
  newoptions->padding = FALSE;
  newoptions->doDAB = FALSE;
  newoptions->dabCrcLength = 0;
  newoptions->dabXpadLength = 0;
  newoptions->verbosity = 2;
  newoptions->upperbitrateindex = 0;

  newoptions->doEnergyLevels = FALSE;
  newoptions->numAncillaryBits = 0;

  return(newoptions);
}


/**
 * This function should actually *check* the parameters to see if they
 * make sense. 
 */

int toolame_init_params (toolame_options *glopts) {
  /* Can't do DAB and energylevel extensions at the same time
     Because both of them think they're the only ones inserting
     information into the ancillary section of the frame */
  if (glopts->doDAB && glopts->doEnergyLevels) {
    fprintf(stderr,"Can't do DAB and Energy Levels at the same time\n");
    exit(99);
  }

  /* Check that if we're doing energy levels, that there's enough space 
     to put the information */
  if (glopts->doEnergyLevels) {
    if ((glopts->mode==MPG_MD_MONO)&&(glopts->numAncillaryBits<16)) {
      fprintf(stderr,"Too few ancillary bits: %i<16\n",glopts->numAncillaryBits);
      glopts->numAncillaryBits = 16;
    }    
    if ((glopts->mode!=MPG_MD_MONO)&&(glopts->numAncillaryBits<40)) {
      fprintf(stderr,"Too few ancillary bits: %i<40\n",glopts->numAncillaryBits);
      glopts->numAncillaryBits = 40;
    }
  }


  glopts->frame.header = &glopts->header;
  glopts->frame.tab_num = -1;		/* no table loaded */
  glopts->frame.alloc = NULL;

  optionsToHeader(glopts, &glopts->header);

  /* this will load the alloc tables and do some other stuff */
  toolame_hdr_to_frps (glopts);

  glopts->toolame_init = glopts->psycho1init = 0;
  glopts->psycho2init = glopts->psycho3init = glopts->psycho4init = 0;

  return(0);
}

/*
  glopts
  leftpcm - holds left channel (or mono channel)
  rightpcm - d'uh
  num_samples - the number of samples in each channel
  mp2buffer - a pointer to the place where we want the mpeg data to be written
  mp2buffer_size - how much space the user allocated for this buffer
  mp2fill_size - how much mpeg data the library has put into the mp2buffer 
*/

int toolame_encode_buffer(toolame_options *glopts, short int leftpcm[], short int rightpcm[],
			  int num_samples, unsigned char *mp2buffer, int mp2buffer_size, 
			  int *mp2fill_size) {
  int frameNum = 0;
  int nch;
  static unsigned int crc;
  int sb, ch, adb;
  unsigned long frameBits, sentBits = 0;
  int i;


  /* Used to keep the SNR values for the fast/quick psy models */
  static FLOAT smrdef[2][32];
  /* This is the saved audio buffer? */
  short sam[2][1344];		/* was [1056]; 
				   MFC MAY03 - 
				   so why the bloody hell did you change it? 
				   explain yourself son.
				   I think this was changed to 1344 for mp3 encoding
				   I think it can be changed back to 1056?
				*/

  static short int buffer[2][1152];
  static unsigned int bit_alloc[2][SBLIMIT], scfsi[2][SBLIMIT];
  static unsigned int scalar[2][3][SBLIMIT], j_scale[3][SBLIMIT];
  static FLOAT smr[2][SBLIMIT], lgmin[2][SBLIMIT], max_sc[2][SBLIMIT];

  static int psycount = 0;

  static int samples_left=0;

  int flush_encoder = FALSE;

  typedef unsigned int SUB[2][3][SCALE_BLOCK][SBLIMIT];
  typedef FLOAT JSBS[3][SCALE_BLOCK][SBLIMIT];
  typedef FLOAT SBS[2][3][SCALE_BLOCK][SBLIMIT];

  static SUB *subband=NULL;
  static SBS *sb_sample=NULL;
  static JSBS *j_sample=NULL;
  static short **win_buf;

  Bit_stream_struc *mybs;

  mybs = (Bit_stream_struc *)calloc(1, sizeof(Bit_stream_struc));
  mybs->buf = mp2buffer;
  mybs->buf_size = mp2buffer_size;
  /* now would be a great time to validate the size of the buffer.
     samples/1152 * sizeof(frame) < mp2buffer_size */
  mybs->buf_byte_idx = 0;
  mybs->buf_bit_idx = 8;
  mybs->totbit = 0;
  mybs->mode = WRITE_MODE;
  mybs->eob = FALSE;
  mybs->eobs = FALSE;

  /* clear buffers */
  if (!glopts->toolame_init) { // Clear buffers on the first call
    subband = (SUB *) toolame_malloc (sizeof (SUB), "subband");
    sb_sample = (SBS *) toolame_malloc (sizeof (SBS), "sb_sample");
    j_sample = (JSBS *) toolame_malloc (sizeof (JSBS), "j_sample");
    win_buf = (short **) toolame_malloc (sizeof (short *) * 2, "win_buf");

    memset ((char *) buffer, 0, sizeof (buffer));
    memset ((char *) bit_alloc, 0, sizeof (bit_alloc));
    memset ((char *) scalar, 0, sizeof (scalar));
    memset ((char *) j_scale, 0, sizeof (j_scale));
    memset ((char *) scfsi, 0, sizeof (scfsi));
    memset ((char *) smr, 0, sizeof (smr));
    memset ((char *) lgmin, 0, sizeof (lgmin));
    memset ((char *) max_sc, 0, sizeof (max_sc));
    memset ((char *) sam, 0, sizeof (sam));
    glopts->toolame_init++;
  }

  *mp2fill_size = 0; //Set the number of bits of mpeg data to zero 

  nch = glopts->frame.nch;

  if (num_samples == -1) { // Flush the encoder
    if (samples_left == 0) {
      // Then there isn't anything to flush
      *mp2fill_size = 0;
      return(0);
    }
    //fprintf(stdout,"Flushing %i samples\n", samples_left);
    flush_encoder = TRUE;
    num_samples = 1152 - samples_left; // There will only ever be one frame to flush.
  }

  //fprintf(stdout, "Asked for %i samples now. %i from before\n",num_samples, samples_left);
  //  while (get_audio (musicin, buffer, nch, &header, glopts) > 0) {
  while (num_samples+samples_left >= 1152) { // Check that there's enough PCM data for one frame
    // Copy a set of samples into the buffers
    if (flush_encoder) {
      /* Pad out the PCM buffers with 0 and encode the frame */
      for (i=samples_left; i< 1152; i++) {
	buffer[0][i] = buffer[1][i] = 0;
      }
    } else {
      for (i=samples_left;i<1152;i++) {
	buffer[0][i] = *leftpcm++;
	buffer[1][i] = *rightpcm++;
      }
    }
    num_samples = num_samples - 1152 + samples_left;
    samples_left = 0;
    //fprintf(stdout,"doing. %i left\n",num_samples);

    win_buf[0] = &buffer[0][0];
    win_buf[1] = &buffer[1][0];

    adb = available_bits (&glopts->header, glopts);

    /* allow the user to reserve some space at the end of the frame
       This will however leave fewer bits for the audio.
       Need to do a sanity check here to see that there are *some* 
       bits left. */
    if (glopts->numAncillaryBits > 0.6*adb) {
      /* Trying to reserve more than 60% of the frame.
	 0.6 is arbitrary. but since most applications
	 probably only want to reserve a few bytes, this seems fine.
	 Typical frame size is about 800bytes */
      fprintf(stderr,"You're trying to reserve more than 60%% of the mpeg frame for ancillary data\n");
      fprintf(stderr,"This is probably an error. But I'll keep going anyway...\n");
    } 

    adb -= glopts->numAncillaryBits;

    /* MFC 26 July 2003
       Doing DAB became a bit harder in the reorganisation of the code.
       Now there is no guarantee that there is more than one frame in the bitbuffer.
       But DAB requires that the CRC for the *current* frame be written at the end 
       of the *previous* frame.
       Workaround: Users (Nicholas?) wanting to implement DAB will have to do some work
       in the frontend.
            First: Reserve some bits for yourself (options->numAncillaryBits)
	    Second: Put the encoder into "single frame mode" i.e. only read 1152 samples
	            per channel. (frontendoptions->singleFrameMode)
	    Third: When you receive each mp2 frame back from the library, you'll have to insert
	           the options->dabCrc[i] values into the end of the frame yourself. (DAB crc calc is 
		   done below)
       The frontend will have to keep the previous frame in memory. 
       As of 26July all that needs to be done is for the frontend to buffer one frame in memory, such that
       the CRC for the next frame can be written in at the end of it.
    */

    {
      int gr, bl, ch;
      /* New polyphase filter
	 Combines windowing and filtering. Ricardo Feb'03 */
      for( gr = 0; gr < 3; gr++ )
	for ( bl = 0; bl < 12; bl++ )
	  for ( ch = 0; ch < nch; ch++ )
	    WindowFilterSubband( &buffer[ch][gr * 12 * 32 + 32 * bl], ch,
				 &(*sb_sample)[ch][gr][bl][0] );
    }

    scalefactor_calc_new(*sb_sample, scalar, nch, glopts->frame.sblimit);
    find_sf_max (scalar, &glopts->frame, max_sc);
    if (glopts->frame.actual_mode == MPG_MD_JOINT_STEREO) {
      /* this way we calculate more mono than we need */
      /* but it is cheap */
      combine_LR_new (*sb_sample, *j_sample, glopts->frame.sblimit);
      scalefactor_calc_new (j_sample, &j_scale, 1, glopts->frame.sblimit);
    }

    if ((glopts->quickmode == TRUE) && (++psycount % glopts->quickcount != 0)) {
      /* We're using quick mode, so we're only calculating the model every
         'quickcount' frames. Otherwise, just copy the old ones across */
      for (ch = 0; ch < nch; ch++) {
	for (sb = 0; sb < SBLIMIT; sb++)
	  smr[ch][sb] = smrdef[ch][sb];
      }
    } else {
      /* calculate the psymodel */
      switch (glopts->psymodel) {
      case -1:
	psycho_n1 (smr, nch);
	break;
      case 0:	/* Psy Model A */
	psycho_0 (smr, nch, scalar, glopts->header.sampling_frequency);	
	break;
      case 1:
	psycho_1 (glopts, buffer, max_sc, smr, &glopts->frame);
	break;
      case 2:
	psycho_2 (buffer, sam, nch, smr, glopts->header.sampling_frequency , glopts);	
	break;
      case 3:
	/* Modified psy model 1 */
	psycho_3 (buffer, max_sc, smr, &glopts->frame, glopts);
	break;
      case 4:
	psycho_4 (buffer, sam, nch, smr, glopts->header.sampling_frequency , glopts);
	break;	
      default:
	fprintf (stderr, "Invalid psy model specification: %i\n", glopts->psymodel);
	exit (0);
      }

      if (glopts->quickmode == TRUE)
	/* copy the smr values and reuse them later */
	for (ch = 0; ch < nch; ch++) {
	  for (sb = 0; sb < SBLIMIT; sb++)
	    smrdef[ch][sb] = smr[ch][sb];
	}

    }

    sf_transmission_pattern (scalar, scfsi, &glopts->frame);
    main_bit_allocation_new (smr, scfsi, bit_alloc, &adb, &glopts->frame, glopts);

    if (glopts->error_protection)
      CRC_calc (&glopts->frame, bit_alloc, scfsi, &crc);

    write_header (&glopts->frame, mybs);
    if (glopts->error_protection)
      buffer_putbits (mybs, crc, 16);
    write_bit_alloc (bit_alloc, &glopts->frame, mybs);
    write_scalefactors(bit_alloc, scfsi, scalar, &glopts->frame, mybs);
    subband_quantization_new (scalar, *sb_sample, j_scale, *j_sample, bit_alloc,
    			  *subband, &glopts->frame);
    write_samples_new(*subband, bit_alloc, &glopts->frame, mybs);

    /* If not all the bits were used, write out a stack of zeros */
    for (i = 0; i < adb; i++)
      buffer_put1bit (mybs, 0);

    /* MFC July 03 
       Write an extra byte for 16/24/32/48 input when padding is on.
       Something must be going astray with the frame size calcs.
       This fudge works fine for the moment */
    if ((glopts->sampling_frequency_idx != 0) && (glopts->padding==TRUE)) // i.e. not a 44.1 or 22kHz input file
      buffer_putbits (mybs, 0, 8);

    if (glopts->doDAB) {
      // Do the CRC calc for DAB stuff if required.
      // It will be up to the frontend to insert it into the end of the 
      // previous frame.
      for (i=glopts->dabCrcLength-1; i>=0; i--) 
	CRC_calcDAB (&glopts->frame, bit_alloc, scfsi, scalar, &glopts->dabCrc[i], i);
    }


    {
      // Allocate space for the reserved ancillary bits
      for (i=0; i<glopts->numAncillaryBits;i++) 
	buffer_put1bit(mybs, 0);
    }

    frameBits = buffer_sstell (mybs) - sentBits;

    if (frameBits % 8) {	/* a program failure */
      fprintf (stderr, "Sent %ld bits = %ld slots plus %ld\n", frameBits,
	       frameBits / 8, frameBits % 8);
      fprintf (stderr, "If you are reading this, the program is broken\n");
      fprintf (stderr, "email [mfc at NOTplanckenerg.com] without the NOT\n");
      fprintf (stderr, "with the command line arguments and other info\n");
      exit (0);
    }

    //fprintf(stdout,"Frame size: %li\n\n",frameBits>>3);
    *mp2fill_size += (frameBits>>3);

    if (glopts->doEnergyLevels)
      doEnergyLevels(glopts, mp2buffer, *mp2fill_size, buffer[0], buffer[1]);

    sentBits += frameBits;
    frameNum++;
  } // end of one frame

  /* Cope with non-integral numbers of samples */
  if ((num_samples != 0)&&(!flush_encoder)) {
    //fprintf(stdout,"sample overflow %i\n",num_samples);
    // Remember samples left
    samples_left = num_samples;
    // Copy these remaining samples into the buffer.
    for (i=0;i<num_samples;i++) {
      buffer[0][i] = *leftpcm++;
      buffer[1][i] = *rightpcm++;
    }
  }

  return(frameNum);
}
// MEANX--allow interleaved left/right encoding
int toolame_encode_buffer_interleaved(toolame_options *glopts, short int interpcm[],
			  int num_samples, unsigned char *mp2buffer, int mp2buffer_size, 
			  int *mp2fill_size) {
  int frameNum = 0;
  int nch;
  static unsigned int crc;
  int sb, ch, adb;
  unsigned long frameBits, sentBits = 0;
  int i;


  /* Used to keep the SNR values for the fast/quick psy models */
  static FLOAT smrdef[2][32];
  /* This is the saved audio buffer? */
  short sam[2][1344];		/* was [1056]; 
				   MFC MAY03 - 
				   so why the bloody hell did you change it? 
				   explain yourself son.
				   I think this was changed to 1344 for mp3 encoding
				   I think it can be changed back to 1056?
				*/

  static short int buffer[2][1152];
  static unsigned int bit_alloc[2][SBLIMIT], scfsi[2][SBLIMIT];
  static unsigned int scalar[2][3][SBLIMIT], j_scale[3][SBLIMIT];
  static FLOAT smr[2][SBLIMIT], lgmin[2][SBLIMIT], max_sc[2][SBLIMIT];

  static int psycount = 0;

  static int samples_left=0;

  int flush_encoder = FALSE;

  typedef unsigned int SUB[2][3][SCALE_BLOCK][SBLIMIT];
  typedef FLOAT JSBS[3][SCALE_BLOCK][SBLIMIT];
  typedef FLOAT SBS[2][3][SCALE_BLOCK][SBLIMIT];

  static SUB *subband=NULL;
  static SBS *sb_sample=NULL;
  static JSBS *j_sample=NULL;
  static short **win_buf;

  Bit_stream_struc *mybs;

  mybs = (Bit_stream_struc *)calloc(1, sizeof(Bit_stream_struc));
  mybs->buf = mp2buffer;
  mybs->buf_size = mp2buffer_size;
  /* now would be a great time to validate the size of the buffer.
     samples/1152 * sizeof(frame) < mp2buffer_size */
  mybs->buf_byte_idx = 0;
  mybs->buf_bit_idx = 8;
  mybs->totbit = 0;
  mybs->mode = WRITE_MODE;
  mybs->eob = FALSE;
  mybs->eobs = FALSE;

  /* clear buffers */
  if (!glopts->toolame_init) { // Clear buffers on the first call
    subband = (SUB *) toolame_malloc (sizeof (SUB), "subband");
    sb_sample = (SBS *) toolame_malloc (sizeof (SBS), "sb_sample");
    j_sample = (JSBS *) toolame_malloc (sizeof (JSBS), "j_sample");
    win_buf = (short **) toolame_malloc (sizeof (short *) * 2, "win_buf");

    memset ((char *) buffer, 0, sizeof (buffer));
    memset ((char *) bit_alloc, 0, sizeof (bit_alloc));
    memset ((char *) scalar, 0, sizeof (scalar));
    memset ((char *) j_scale, 0, sizeof (j_scale));
    memset ((char *) scfsi, 0, sizeof (scfsi));
    memset ((char *) smr, 0, sizeof (smr));
    memset ((char *) lgmin, 0, sizeof (lgmin));
    memset ((char *) max_sc, 0, sizeof (max_sc));
    memset ((char *) sam, 0, sizeof (sam));
    glopts->toolame_init++;
  }

  *mp2fill_size = 0; //Set the number of bits of mpeg data to zero 

  nch = glopts->frame.nch;

  if (num_samples == -1) { // Flush the encoder
    if (samples_left == 0) {
      // Then there isn't anything to flush
      *mp2fill_size = 0;
      return(0);
    }
    //fprintf(stdout,"Flushing %i samples\n", samples_left);
    flush_encoder = TRUE;
    num_samples = 1152 - samples_left; // There will only ever be one frame to flush.
  }

  //fprintf(stdout, "Asked for %i samples now. %i from before\n",num_samples, samples_left);
  //  while (get_audio (musicin, buffer, nch, &header, glopts) > 0) {
  while (num_samples+samples_left >= 1152) { // Check that there's enough PCM data for one frame
    // Copy a set of samples into the buffers
    if (flush_encoder) {
      /* Pad out the PCM buffers with 0 and encode the frame */
      for (i=samples_left; i< 1152; i++) {
	buffer[0][i] = buffer[1][i] = 0;
      }
    } else {
      for (i=samples_left;i<1152;i++) {
	buffer[0][i] = *interpcm++;
	buffer[1][i] = *interpcm++;
      }
    }
    num_samples = num_samples - 1152 + samples_left;
    samples_left = 0;
    //fprintf(stdout,"doing. %i left\n",num_samples);

    win_buf[0] = &buffer[0][0];
    win_buf[1] = &buffer[1][0];

    adb = available_bits (&glopts->header, glopts);

    /* allow the user to reserve some space at the end of the frame
       This will however leave fewer bits for the audio.
       Need to do a sanity check here to see that there are *some* 
       bits left. */
    if (glopts->numAncillaryBits > 0.6*adb) {
      /* Trying to reserve more than 60% of the frame.
	 0.6 is arbitrary. but since most applications
	 probably only want to reserve a few bytes, this seems fine.
	 Typical frame size is about 800bytes */
      fprintf(stderr,"You're trying to reserve more than 60%% of the mpeg frame for ancillary data\n");
      fprintf(stderr,"This is probably an error. But I'll keep going anyway...\n");
    } 

    adb -= glopts->numAncillaryBits;

    /* MFC 26 July 2003
       Doing DAB became a bit harder in the reorganisation of the code.
       Now there is no guarantee that there is more than one frame in the bitbuffer.
       But DAB requires that the CRC for the *current* frame be written at the end 
       of the *previous* frame.
       Workaround: Users (Nicholas?) wanting to implement DAB will have to do some work
       in the frontend.
            First: Reserve some bits for yourself (options->numAncillaryBits)
	    Second: Put the encoder into "single frame mode" i.e. only read 1152 samples
	            per channel. (frontendoptions->singleFrameMode)
	    Third: When you receive each mp2 frame back from the library, you'll have to insert
	           the options->dabCrc[i] values into the end of the frame yourself. (DAB crc calc is 
		   done below)
       The frontend will have to keep the previous frame in memory. 
       As of 26July all that needs to be done is for the frontend to buffer one frame in memory, such that
       the CRC for the next frame can be written in at the end of it.
    */

    {
      int gr, bl, ch;
      /* New polyphase filter
	 Combines windowing and filtering. Ricardo Feb'03 */
      for( gr = 0; gr < 3; gr++ )
	for ( bl = 0; bl < 12; bl++ )
	  for ( ch = 0; ch < nch; ch++ )
	    WindowFilterSubband( &buffer[ch][gr * 12 * 32 + 32 * bl], ch,
				 &(*sb_sample)[ch][gr][bl][0] );
    }

    scalefactor_calc_new(*sb_sample, scalar, nch, glopts->frame.sblimit);
    find_sf_max (scalar, &glopts->frame, max_sc);
    if (glopts->frame.actual_mode == MPG_MD_JOINT_STEREO) {
      /* this way we calculate more mono than we need */
      /* but it is cheap */
      combine_LR_new (*sb_sample, *j_sample, glopts->frame.sblimit);
      scalefactor_calc_new (j_sample, &j_scale, 1, glopts->frame.sblimit);
    }

    if ((glopts->quickmode == TRUE) && (++psycount % glopts->quickcount != 0)) {
      /* We're using quick mode, so we're only calculating the model every
         'quickcount' frames. Otherwise, just copy the old ones across */
      for (ch = 0; ch < nch; ch++) {
	for (sb = 0; sb < SBLIMIT; sb++)
	  smr[ch][sb] = smrdef[ch][sb];
      }
    } else {
      /* calculate the psymodel */
      switch (glopts->psymodel) {
      case -1:
	psycho_n1 (smr, nch);
	break;
      case 0:	/* Psy Model A */
	psycho_0 (smr, nch, scalar, glopts->header.sampling_frequency);	
	break;
      case 1:
	psycho_1 (glopts, buffer, max_sc, smr, &glopts->frame);
	break;
      case 2:
	psycho_2 (buffer, sam, nch, smr, glopts->header.sampling_frequency , glopts);	
	break;
      case 3:
	/* Modified psy model 1 */
	psycho_3 (buffer, max_sc, smr, &glopts->frame, glopts);
	break;
      case 4:
	psycho_4 (buffer, sam, nch, smr, glopts->header.sampling_frequency , glopts);
	break;	
      default:
	fprintf (stderr, "Invalid psy model specification: %i\n", glopts->psymodel);
	exit (0);
      }

      if (glopts->quickmode == TRUE)
	/* copy the smr values and reuse them later */
	for (ch = 0; ch < nch; ch++) {
	  for (sb = 0; sb < SBLIMIT; sb++)
	    smrdef[ch][sb] = smr[ch][sb];
	}

    }

    sf_transmission_pattern (scalar, scfsi, &glopts->frame);
    main_bit_allocation_new (smr, scfsi, bit_alloc, &adb, &glopts->frame, glopts);

    if (glopts->error_protection)
      CRC_calc (&glopts->frame, bit_alloc, scfsi, &crc);

    write_header (&glopts->frame, mybs);
    if (glopts->error_protection)
      buffer_putbits (mybs, crc, 16);
    write_bit_alloc (bit_alloc, &glopts->frame, mybs);
    write_scalefactors(bit_alloc, scfsi, scalar, &glopts->frame, mybs);
    subband_quantization_new (scalar, *sb_sample, j_scale, *j_sample, bit_alloc,
    			  *subband, &glopts->frame);
    write_samples_new(*subband, bit_alloc, &glopts->frame, mybs);

    /* If not all the bits were used, write out a stack of zeros */
    for (i = 0; i < adb; i++)
      buffer_put1bit (mybs, 0);

    /* MFC July 03 
       Write an extra byte for 16/24/32/48 input when padding is on.
       Something must be going astray with the frame size calcs.
       This fudge works fine for the moment */
    if ((glopts->sampling_frequency_idx != 0) && (glopts->padding==TRUE)) // i.e. not a 44.1 or 22kHz input file
      buffer_putbits (mybs, 0, 8);

    if (glopts->doDAB) {
      // Do the CRC calc for DAB stuff if required.
      // It will be up to the frontend to insert it into the end of the 
      // previous frame.
      for (i=glopts->dabCrcLength-1; i>=0; i--) 
	CRC_calcDAB (&glopts->frame, bit_alloc, scfsi, scalar, &glopts->dabCrc[i], i);
    }


    {
      // Allocate space for the reserved ancillary bits
      for (i=0; i<glopts->numAncillaryBits;i++) 
	buffer_put1bit(mybs, 0);
    }

    frameBits = buffer_sstell (mybs) - sentBits;

    if (frameBits % 8) {	/* a program failure */
      fprintf (stderr, "Sent %ld bits = %ld slots plus %ld\n", frameBits,
	       frameBits / 8, frameBits % 8);
      fprintf (stderr, "If you are reading this, the program is broken\n");
      fprintf (stderr, "email [mfc at NOTplanckenerg.com] without the NOT\n");
      fprintf (stderr, "with the command line arguments and other info\n");
      exit (0);
    }

    //fprintf(stdout,"Frame size: %li\n\n",frameBits>>3);
    *mp2fill_size += (frameBits>>3);

    if (glopts->doEnergyLevels)
      doEnergyLevels(glopts, mp2buffer, *mp2fill_size, buffer[0], buffer[1]);

    sentBits += frameBits;
    frameNum++;
  } // end of one frame

  /* Cope with non-integral numbers of samples */
  if ((num_samples != 0)&&(!flush_encoder)) {
    //fprintf(stdout,"sample overflow %i\n",num_samples);
    // Remember samples left
    samples_left = num_samples;
    // Copy these remaining samples into the buffer.
    for (i=0;i<num_samples;i++) {
      buffer[0][i] = *interpcm++;
      buffer[1][i] = *interpcm++;
    }
  }

  return(frameNum);
}

//--/MEANX
void toolame_deinit(toolame_options *glopts) {
  // free mem

  if (glopts->psycho3init) psycho_3_deinit(glopts->p3mem);
  if (glopts->psycho4init) psycho_4_deinit(glopts->p4mem);
  if (glopts->psycho2init) psycho_2_deinit(glopts->p2mem);
  if (glopts->psycho1init) psycho_1_deinit(glopts->p1mem);

  toolame_free ( (void **) &glopts);
}

int toolame_encode_flush(toolame_options *glopts, unsigned char *mp2buffer, int mp2buffer_size, int *mp2fill_size) {
  // Call encode_buffer telling it that we have -1 frames to encode.
  // This is a signal that it should pad the rest of the audio data it has with zeros
  // and flush the mp2 audio out.
  return( toolame_encode_buffer(glopts, NULL, NULL, -1, mp2buffer, mp2buffer_size, mp2fill_size) );
}



char *toolame_getModeName(int mode) {
  static char *mode_name[5] = { "stereo", "j-stereo", "dual-ch", "single-ch", "illegal mode"};
  if (mode>=0 && mode<4)
    return (mode_name[mode]);
  else 
    return (mode_name[4]);
}

char *toolame_getVersionName(int version) {
  static char *version_name[3] = { "MPEG-2 LSF", "MPEG-1", "illegal version" };
  if (version==0)
    return(version_name[0]);
  if (version==1)
    return(version_name[1]);
  return(version_name[2]);
}

  /* 1: MPEG-1, 0: MPEG-2 LSF, 1995-07-11 shn */
  static int bitrate[2][15] = {
    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384}
  };

/* int toolame_getBitrate(toolame_options *glopt) { */
/*   int version = glopt->version; */
/*   int bitrateindex = glopt->bitrate_index; */
/*   if (version>=0 && version<2 && bitrateindex>=0 && bitrateindex<15)  */
/*     return(bitrate[version][bitrateindex]); */
/*   return(-1); */
/* } */



int toolame_getBitrateIndex (int version, int bRate) {
/* legal bitrates rates from 32 to 448 
   version is MPEG-1 or MPEG-2 LSF */ 
/* convert bitrate in kbps to index */
  int index = 0;
  int found = 0;

  // MFC sanity check.
  if (version!=0 && version!=1) {
    fprintf(stderr,"getbitrateindex error %i\n",version);
    exit(99);
  }

  while (!found && index < 15) {
    if (bitrate[version][index] == bRate)
      found = 1;
    else
      ++index;
  }
  if (found)
    return (index);
  else {
    fprintf (stderr,
	     "BitrateIndex: %d is not a legal bitrate for version %i\n",
	     bRate, version);
    exit (-1);			/* Error! */
  }
}

int toolame_getSmpFrqIndex (long sRate, int *version)
/* convert samp frq in Hz to index */
/* legal rates 16000, 22050, 24000, 32000, 44100, 48000 */
{
  /* 1: MPEG-1, 0: MPEG-2 LSF, 1995-07-11 shn */
  //static FLOAT s_freq[2][4] = { {22.05, 24, 16, 0}, {44.1, 48, 32, 0} };


  if (sRate == 44100L) {
    *version = MPEG_AUDIO_ID;
    return (0);
  } else if (sRate == 48000L) {
    *version = MPEG_AUDIO_ID;
    return (1);
  } else if (sRate == 32000L) {
    *version = MPEG_AUDIO_ID;
    return (2);
  } else if (sRate == 24000L) {
    *version = MPEG_PHASE2_LSF;
    return (1);
  } else if (sRate == 22050L) {
    *version = MPEG_PHASE2_LSF;
    return (0);
  } else if (sRate == 16000L) {
    *version = MPEG_PHASE2_LSF;
    return (2);
  } else {
    fprintf (stderr, "SmpFrqIndex: %ld is not a legal sample rate\n", sRate);
    exit(99);
  }
}

/********************************************************************
new_ext()
Puts a new extension name on a file name <filename>.
Removes the last extension name, if any.
1992-08-19, 1995-06-12 shn
***********************************************************************/
void toolame_newExtension (char *filename, char *extname, char *newname)
{
  int found, dotpos;

  /* First, strip the extension */
  dotpos = strlen (filename);
  found = 0;
  do {
    switch (filename[dotpos]) {
    case '.':
      found = 1;
      break;
    case '\\':
    case '/':
    case ':':
      found = -1;
      break;
    default:
      dotpos--;
      if (dotpos < 0)
	found = -1;
      break;
    }
  }
  while (found == 0);
  if (found == -1)
    strcpy (newname, filename);
  if (found == 1) {
    strncpy (newname, filename, dotpos);
    newname[dotpos] = '\0';
  }
  strcat (newname, extname);
}


int toolame_js_bound (int m_ext)
{
  /* layer 2 only */
  static int jsb_table[4] = { 4, 8, 12, 16 };

  if (m_ext < 0 || m_ext > 3) {
    fprintf (stderr, "js_bound bad modext (%d)\n", m_ext);
    exit (1);
  }
  return (jsb_table[m_ext]);
}

void toolame_hdr_to_frps (toolame_options *glopts)
/* interpret data in hdr str to fields in frame */
{
  frame_info *frame = &glopts->frame;
  frame_header *hdr = frame->header;	/* (or pass in as arg?) */

  frame->actual_mode = hdr->mode;
  frame->nch = (hdr->mode == MPG_MD_MONO) ? 1 : 2;

  frame->sblimit = pick_table (glopts, frame);
  /* MFC FIX this up */
  encode_init(glopts, frame);

  if (hdr->mode == MPG_MD_JOINT_STEREO)
    frame->jsbound = toolame_js_bound (hdr->mode_ext);
  else
    frame->jsbound = frame->sblimit;
  /* alloc, tab_num set in pick_table */
}


/*********************************************/

int toolame_setMode (toolame_options *glopts, int mode) {
  if (mode<0 || mode>3) {
    fprintf(stderr,"invalid mode %i\n",mode);
    return(-1);
  }
  glopts->mode = mode;
  return(0);
}

int toolame_getMode (toolame_options *glopts) {
  return(glopts->mode);
}

int toolame_setPsymodel (toolame_options *glopts, int psymodel) {
  if (psymodel<-1 || psymodel>4)
    return(-1);
  glopts->psymodel = psymodel;
  return(0);
}

int toolame_getPsymodel (toolame_options *glopts) {
  return(glopts->psymodel);
}

int toolame_setSampleFreq (toolame_options *glopts, int sampling_frequency) {  
  glopts->sampling_frequency_idx = toolame_getSmpFrqIndex (sampling_frequency, &glopts->version);
  glopts->sampling_frequency = sampling_frequency;
  /* if the sampling frequency is >=32000, set to MPEG1
     else set to MPEG2 */
  if (sampling_frequency < 32000) 
    glopts->version = MPEG2_LSF;
  else
    glopts->version = MPEG1;
  
  return(0);
}

int toolame_getSampleFreq (toolame_options *glopts) {
  return(glopts->sampling_frequency);
}

int toolame_setBitrate (toolame_options *glopts, int bitrate) {
  glopts->bitrate_index = toolame_getBitrateIndex(glopts->version, bitrate);
  glopts->bitrate = bitrate;
  return(0);
}

int toolame_getBitrate (toolame_options *glopts) {
  return(glopts->bitrate);
}

int toolame_setEmphasis (toolame_options *glopts, int emphasis) {
  //MFC Fix: add #defines for the emphasis modes
  if (emphasis!=0 && emphasis!=1 && emphasis!=3) 
    return(-1);
  glopts->emphasis = emphasis;
  return(0);
}

int toolame_getEmphasis (toolame_options *glopts) {
  return(glopts->emphasis);
}

int toolame_setErrorProtection (toolame_options *glopts, int error_protection) {
  if (error_protection)
    glopts->error_protection = TRUE;
  else
    error_protection = FALSE;
  return(0);
}

int toolame_getErrorProtection (toolame_options *glopts) {
  return(glopts->error_protection);
}

int toolame_setCopyright (toolame_options *glopts, int copyright) {
  if (copyright)
    glopts->copyright = TRUE;
  else 
    glopts->copyright = FALSE;
  return(0);
}
int toolame_getCopyright (toolame_options *glopts) {
  return(glopts->copyright);
}

int toolame_setOriginal (toolame_options *glopts, int original) {
  if (original)
    glopts->original = TRUE;
  else
    glopts->original = FALSE;
  return(0);
}
int toolame_getOriginal (toolame_options *glopts) {
  return(glopts->original);
}

int toolame_setPadding (toolame_options *glopts, int padding) {
  if (padding)
    glopts->padding = TRUE;
  else
    glopts->padding = FALSE;
      
  return(0);
}
int toolame_getPadding (toolame_options *glopts) {
  return(glopts->padding);
}

int toolame_setVBR (toolame_options *glopts, int vbr) {
  if (vbr)
    glopts->vbr = TRUE;
  else
    glopts->vbr = FALSE;
  return(0);
}

int toolame_getVBR (toolame_options *glopts) {
  return(glopts->vbr);
}

int toolame_setVBRLevel (toolame_options *glopts, FLOAT level) {
  if (fabs(level)>50.0)
    return(-1);
  else
    glopts->vbrlevel = level;
  return(0);
}
FLOAT toolame_getVBRLevel (toolame_options *glopts) {
  return(glopts->vbrlevel);
}

int toolame_setATHLevel (toolame_options *glopts, FLOAT level) {
  glopts->athlevel = level;
  return(0);
}
FLOAT toolame_getATHLevel (toolame_options *glopts) {
  return(glopts->athlevel);
}

int toolame_setQuickMode (toolame_options *glopts, int quickmode) {
  if (quickmode)
    glopts->quickmode = TRUE;
  else
    glopts->quickmode = FALSE;
  return(0);
}
int toolame_getQuickMode (toolame_options *glopts) {
  return(glopts->quickmode);
}

int toolame_setQuickCount (toolame_options *glopts, int quickcount ) {
  glopts->quickcount = quickcount;
  return(0);
}
int toolame_getQuickCount (toolame_options *glopts) {
  return(glopts->quickcount);
}


int toolame_setVerbosity (toolame_options *glopts, int verbosity) {
  glopts->verbosity = verbosity;
  return(0);
}
int toolame_getVerbosity (toolame_options *glopts) {
  return(glopts->verbosity);
}

int toolame_setVBRUpperBitrate (toolame_options *glopts, int bitrate) {
  glopts->upperbitrateindex = toolame_getBitrateIndex(glopts->version, bitrate);
  return(0);
}

int toolame_getVBRUpperBitrate (toolame_options *glopts) {
  return(bitrate[glopts->version][glopts->upperbitrateindex]);
}

int toolame_setNumAncillaryBits (toolame_options *glopts, int num) {
  if (num<0)
    return(-1);
  glopts->numAncillaryBits = num;
  return(0);
}
int toolame_getNumAncillaryBits (toolame_options *glopts) {
  return(glopts->numAncillaryBits);
}

int toolame_setEnergyLevels (toolame_options *glopts, int energylevels ) {
  if (energylevels)
    glopts->doEnergyLevels = TRUE;    
  else
    glopts->doEnergyLevels = FALSE;
  return(0);
}
int toolame_getEnergyLevels (toolame_options *glopts) {
  return(glopts->doEnergyLevels);
}

int toolame_setVersion (toolame_options *glopts, int version) {
  if (version!=0 && version!=1)
    return(-1);
  glopts->version = version;
  return(0);
}
int toolame_getVersion (toolame_options *glopts) {
  return(glopts->version);
}

int toolame_setDAB (toolame_options *glopts, int dab) {
  if (dab)
    glopts->doDAB = TRUE;
  else
    glopts->doDAB = FALSE;
  return(0);
}
int toolame_getDAB (toolame_options *glopts) {
  return(glopts->doDAB);
}

int toolame_setDABXPADLength (toolame_options *glopts, int length) {
  if (length<0)
    return(-1);
  glopts->dabXpadLength = length;
  return(0);
}
int toolame_getDABXPADLength (toolame_options *glopts) {
  return(glopts->dabXpadLength);
}

int toolame_setDABCRCLength (toolame_options *glopts, int length) {
  if (length<0)
    return(-1);
  else
    glopts->dabCrcLength = length;
  return(0);
}
int toolame_getDABCRCLength (toolame_options *glopts) {
  return(glopts->dabCrcLength);
}

/************ generic

int toolame_set (toolame_options *glopts, ) {

  return(0);
}
int toolame_get (toolame_options *glopts) {
  return(glopts->);
}



*************/
