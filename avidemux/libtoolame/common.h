#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define         NULL_CHAR               '\0'

#define         MAX_U_32_NUM            0xFFFFFFFF
#ifndef PI
#define         PI                      3.14159265358979
#endif
#define         PI2                     PI/2
#define         PI4                     PI/4
#define         PI64                    PI/64
#define         LN_TO_LOG10             0.2302585093

#define         VOL_REF_NUM             0
#define         MPEG_AUDIO_ID           1
#define		MPEG_PHASE2_LSF		0	/* 1995-07-11 SHN */
#define         MAC_WINDOW_SIZE         24

#define         MONO                    1
#define         STEREO                  2
#define         BITS_IN_A_BYTE          8
#define         WORD                    16
#define         MAX_NAME_SIZE           255
#define         SBLIMIT                 32
#define         SSLIMIT                 18
#define         FFT_SIZE                1024
#define         HAN_SIZE                512
#define         SCALE_BLOCK             12
#define         SCALE_RANGE             64
#define         SCALE                   32768
#define         CRC16_POLYNOMIAL        0x8005
#define         CRC8_POLYNOMIAL         0x1D




/* "bit_stream.h" Definitions */

#define         MINIMUM         4	/* Minimum size of the buffer in bytes */
#define         MAX_LENGTH      32	/* Maximum length of word written or
					   read from bit stream */
#define         READ_MODE       0
#define         WRITE_MODE      1
#define         ALIGNING        8
#define         BINARY          0
#define         ASCII           1

#define         BUFFER_SIZE     4096

#define         MIN(A, B)       ((A) < (B) ? (A) : (B))
#define         MAX(A, B)       ((A) > (B) ? (A) : (B))
/***********************************************************************
*
*  Global Type Definitions
*
***********************************************************************/

typedef struct bit_stream_struc {
  FILE *pt;			/* pointer to bit stream device */
  unsigned char *buf;		/* bit stream buffer */
  int buf_size;			/* size of buffer (in number of bytes) */
  long totbit;			/* bit counter of bit stream */
  int buf_byte_idx;		/* pointer to top byte in buffer */
  int buf_bit_idx;		/* pointer to top bit of top byte in buffer */
  int mode;			/* bit stream open in read or write mode */
  int eob;			/* end of buffer index */
  int eobs;			/* end of bit stream flag */
  char format;

  /* format of file in rd mode (BINARY/ASCII) */
} Bit_stream_struc;


#endif
