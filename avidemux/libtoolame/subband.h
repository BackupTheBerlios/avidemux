

void  WindowFilterSubband( short *pBuffer, int ch, FLOAT s[SBLIMIT] );
void create_dct_matrix (FLOAT filter[16][32]);

#ifdef REFERENCECODE
void window_subband (short **buffer, FLOAT z[64], int k);
void filter_subband (FLOAT z[HAN_SIZE], FLOAT s[SBLIMIT]);
#endif
