/* 
   RTjpeg (C) Justin Schoeman 1998 (justin@suntiger.ee.up.ac.za)
   
   With modifications by:
   (c) 1998, 1999 by Joerg Walter <trouble@moes.pmnet.uni-oldenburg.de>
   and
   (c) 1999 by Wim Taymans <wim.taymans@tvd.be>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
    
*/

#ifndef _I386_TYPES_H
#include "ADM_default.h"
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;
typedef int8_t __s8;
typedef int16_t __s16;
typedef int32_t __s32;
#endif

extern void RTjpeg_init_Q(__u8 Q);
extern void RTjpeg_init_compress(__u32 *buf, int width, int height, __u8 Q);
extern void RTjpeg_init_decompress(__u32 *buf, int width, int height);
extern int RTjpeg_compressYUV420(__s8 *sp, unsigned char *bp);
extern int RTjpeg_compressYUV422(__s8 *sp, unsigned char *bp);
extern void RTjpeg_decompressYUV420(__s8 *sp, __u8 *bp);
extern void RTjpeg_decompressYUV422(__s8 *sp, __u8 *bp);
extern int RTjpeg_compress8(__s8 *sp, unsigned char *bp);
extern void RTjpeg_decompress8(__s8 *sp, __u8 *bp);

extern void RTjpeg_init_mcompress(void);
extern int RTjpeg_mcompressYUV420(__s8 *sp, unsigned char *bp, __u16 lmask, __u16 cmask);
extern int RTjpeg_mcompressYUV422(__s8 *sp, unsigned char *bp, __u16 lmask, __u16 cmask);
extern int RTjpeg_mcompress8(__s8 *sp, unsigned char *bp, __u16 lmask);
extern void RTjpeg_set_test(int i);

extern void RTjpeg_yuv420rgb(__u8 *buf, __u8 *rgb, int stride);
extern void RTjpeg_yuv422rgb(__u8 *buf, __u8 *rgb, int stride);
extern void RTjpeg_yuvrgb8(__u8 *buf, __u8 *rgb, int stride);
extern void RTjpeg_yuvrgb16(__u8 *buf, __u8 *rgb, int stride);
extern void RTjpeg_yuvrgb24(__u8 *buf, __u8 *rgb, int stride);
extern void RTjpeg_yuvrgb32(__u8 *buf, __u8 *rgb, int stride);
		  

