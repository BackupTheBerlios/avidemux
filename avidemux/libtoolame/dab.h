
void CRC_calcDAB (frame_info * frame,
			 unsigned int bit_alloc[2][SBLIMIT],
			 unsigned int scfsi[2][SBLIMIT],
			 unsigned int scalar[2][3][SBLIMIT],
			 unsigned int *crc, int i);

void update_CRCDAB (unsigned int, unsigned int, unsigned int *);
