#include <stdio.h>
#include <string.h>
#include "toolame.h"
#include "toolame_global_flags.h"
#include "common.h"
#include "dab.h"

void
CRC_calcDAB (frame_info * frame,
	     unsigned int bit_alloc[2][SBLIMIT],
	     unsigned int scfsi[2][SBLIMIT],
	     unsigned int scalar[2][3][SBLIMIT], unsigned int *crc,
	     int packed)
{
  int i, j, k;
  int nch = frame->nch;
  int nb_scalar;
  int f[5] = { 0, 4, 8, 16, 30 };
  int first, last;

  first = f[packed];
  last = f[packed + 1];
  if (last > frame->sblimit)
    last = frame->sblimit;

  nb_scalar = 0;
  *crc = 0x0;
  for (i = first; i < last; i++)
    for (k = 0; k < nch; k++)
      if (bit_alloc[k][i])	/* above jsbound, bit_alloc[0][i] == ba[1][i] */
	switch (scfsi[k][i]) {
	case 0:
	  for (j = 0; j < 3; j++) {
	    nb_scalar++;
	    update_CRCDAB (scalar[k][j][i] >> 3, 3, crc);
	  }
	  break;
	case 1:
	case 3:
	  nb_scalar += 2;
	  update_CRCDAB (scalar[k][0][i] >> 3, 3, crc);
	  update_CRCDAB (scalar[k][2][i] >> 3, 3, crc);
	  break;
	case 2:
	  nb_scalar++;
	  update_CRCDAB (scalar[k][0][i] >> 3, 3, crc);
	}
}

void update_CRCDAB (unsigned int data, unsigned int length, unsigned int *crc)
{
  unsigned int masking, carry;

  masking = 1 << length;

  while ((masking >>= 1)) {
    carry = *crc & 0x80;
    *crc <<= 1;
    if (!carry ^ !(data & masking))
      *crc ^= CRC8_POLYNOMIAL;
  }
  *crc &= 0xff;
}


#ifdef OLD_BROKEN_DAB_STUFF
// Leaving these here while DAB is reimplemented properly


/*
  19 Jul 03
  DAB stuff got broken in the update to 02m
  Not sure who uses this. I would like to get it working again eventually.
*/

/*
  Just after the available bits calculation in toolame, 
  allocated some bits for the DAB stuff 
*/

    if (header.dab_extension) {
      /* in 24 kHz we always have 4 bytes */
      if (header.sampling_frequency == 1)
	header.dab_extension = 4;
/* You must have one frame in memory if you are in DAB mode                 */
/* in conformity of the norme ETS 300 401 http://www.etsi.org               */
      /* see bitstream.c            */
      if (frameNum == 1)
	minimum = lg_frame + MINIMUM;
      adb -= header.dab_extension * 8 + header.dab_length * 8 + 16;
    }


/* 
   Just after we finishing padding up to the number of ADB
   Put the extension stuff in (ancillary data, right at the end)
*/
    if (header.dab_extension) {
      /* Reserve some bytes for X-PAD in DAB mode */
      putbits (mybs, 0, header.dab_length * 8);
      
      for (i = header.dab_extension - 1; i >= 0; i--) {
	CRC_calcDAB (&frame, bit_alloc, scfsi, scalar, &crc, i);
	/* this crc is for the previous frame in DAB mode  */
	if (mybs->buf_byte_idx + lg_frame < mybs->buf_size)
	  mybs->buf[mybs->buf_byte_idx + lg_frame] = crc;
	/* reserved 2 bytes for F-PAD in DAB mode  */
	putbits (mybs, crc, 8);
      }
      putbits (mybs, 0, 16);
    }

#endif
