#include <stdio.h>
#include <stdlib.h>
#include "toolame.h"
#include "toolame_global_flags.h"
#include "common.h"
#include "bitbuffer.h"



/*write 1 bit from the bit stream */
void buffer_put1bit (Bit_stream_struc * bs, int bit)
{
  bs->totbit++;

  bs->buf[bs->buf_byte_idx] |= (bit & 0x1) << (bs->buf_bit_idx - 1);
  bs->buf_bit_idx--;
  if (!bs->buf_bit_idx) {
    bs->buf_bit_idx = 8;
    bs->buf_byte_idx++;
    if (bs->buf_byte_idx >= bs->buf_size) {
      //empty_buffer (bs, minimum);
      fprintf(stdout,"buffer_put1bit: error. mp2buffer needs to be bigger\n");
      exit(99);
    }
    bs->buf[bs->buf_byte_idx] = 0;
  }
}

/*write N bits into the bit stream */
INLINE void buffer_putbits (Bit_stream_struc * bs, unsigned int val, int N)
{
  static int putmask[9] = { 0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };
  register int j = N;
  register int k, tmp;

  bs->totbit += N;
  while (j > 0) {
    k = MIN (j, bs->buf_bit_idx);
    tmp = val >> (j - k);
    bs->buf[bs->buf_byte_idx] |= (tmp & putmask[k]) << (bs->buf_bit_idx - k);
    bs->buf_bit_idx -= k;
    if (!bs->buf_bit_idx) {
      bs->buf_bit_idx = 8;
      bs->buf_byte_idx++;
      if (bs->buf_byte_idx >= bs->buf_size) {
	//empty_buffer (bs, minimum);
	fprintf(stdout,"buffer_putbits: error. mp2buffer needs to be bigger\n");
	exit(99);	
      }
      bs->buf[bs->buf_byte_idx] = 0;
    }
    j -= k;
  }
}

/*return the current bit stream length (in bits)*/
unsigned long buffer_sstell (Bit_stream_struc * bs)
{
  return (bs->totbit);
}
