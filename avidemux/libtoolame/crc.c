#include <stdio.h>
#include <string.h>
#include "toolame.h"
#include "toolame_global_flags.h"
#include "common.h"
#include "crc.h"

/*****************************************************************************
*
*  CRC error protection package
*
*****************************************************************************/

void CRC_calc (frame_info * frame, unsigned int bit_alloc[2][SBLIMIT],
	       unsigned int scfsi[2][SBLIMIT], unsigned int *crc)
{
  int i, k;
  frame_header *header = frame->header;
  int nch = frame->nch;
  int sblimit = frame->sblimit;
  int jsbound = frame->jsbound;
  al_table *alloc = frame->alloc;

  *crc = 0xffff;		/* changed from '0' 92-08-11 shn */
  update_CRC (header->bitrate_index, 4, crc);
  update_CRC (header->sampling_frequency_idx, 2, crc);
  update_CRC (header->padding, 1, crc);
  update_CRC (header->private_bit, 1, crc);
  update_CRC (header->mode, 2, crc);
  update_CRC (header->mode_ext, 2, crc);
  update_CRC (header->copyright, 1, crc);
  update_CRC (header->original, 1, crc);
  update_CRC (header->emphasis, 2, crc);

  for (i = 0; i < sblimit; i++)
    for (k = 0; k < ((i < jsbound) ? nch : 1); k++)
      update_CRC (bit_alloc[k][i], (*alloc)[i][0].bits, crc);

  for (i = 0; i < sblimit; i++)
    for (k = 0; k < nch; k++)
      if (bit_alloc[k][i])
	update_CRC (scfsi[k][i], 2, crc);
}

void update_CRC (unsigned int data, unsigned int length, unsigned int *crc)
{
  unsigned int masking, carry;

  masking = 1 << length;

  while ((masking >>= 1)) {
    carry = *crc & 0x8000;
    *crc <<= 1;
    if (!carry ^ !(data & masking))
      *crc ^= CRC16_POLYNOMIAL;
  }
  *crc &= 0xffff;
}

