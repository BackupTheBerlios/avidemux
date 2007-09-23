/* config.h.in.  Generated from configure.in by autoheader.  */

/* MPEG2DEC */
#cmakedefine ACCEL_DETECT

#if ${WIN32_DEF}
#define rindex strrchr
#define index strchr
#define ftello ftello64
#define fseeko fseeko64
#endif

/* Big endian CPU - SPARC or PowerPC */
#cmakedefine ADM_BIG_ENDIAN

#define PACKAGE   "avidemux"
#define ADMLOCALE "${ADM_LOCALE}"

/* BSD OS specific ifdef */
#cmakedefine ADM_BSD_FAMILY

#cmakedefine HAVE_AUDIO

/* Sparc workstations */
#cmakedefine ADM_SPARC

/* Build for Windows 32bits */
#cmakedefine ADM_WIN32

/* ALSA is 1.0 */
#cmakedefine ALSA_1_0_SUPPORT

/* use ALSA as possible audio device */
#cmakedefine ALSA_SUPPORT

/* AMR_NB */
#cmakedefine AMR_NB

/* X86_64 AMD64 assembly */
#cmakedefine ARCH_64_BITS

/* Enable PowerPC optim */
#cmakedefine ARCH_POWERPC

/* AltiVec for libmpeg2 */
#cmakedefine ARCH_PPC

/* post proc */
#cmakedefine ARCH_X86

/* X86_32 assembly */
#cmakedefine ARCH_X86_32

/* X86_64 AMD64 assembly */
#cmakedefine ARCH_X86_64

/* FFMPEG */
#cmakedefine CONFIG_AC3_ENCODER
#cmakedefine CONFIG_AMR_NB
#cmakedefine CONFIG_CYUV_DECODER
#cmakedefine CONFIG_DARWIN
#cmakedefine CONFIG_DECODERS
#cmakedefine CONFIG_DVVIDEO_DECODER
#cmakedefine CONFIG_DVVIDEO_ENCODER
#cmakedefine CONFIG_ENCODERS
#cmakedefine CONFIG_FFV1_DECODER
#cmakedefine CONFIG_FFV1_ENCODER
#cmakedefine CONFIG_FFVHUFF_DECODER
#cmakedefine CONFIG_FFVHUFF_ENCODER
#cmakedefine CONFIG_H263P_DECODER
#cmakedefine CONFIG_H263P_ENCODER
#cmakedefine CONFIG_H263_DECODER
#cmakedefine CONFIG_H263_ENCODER
#cmakedefine CONFIG_H264_DECODER
#cmakedefine CONFIG_HUFFYUV_DECODER
#cmakedefine CONFIG_HUFFYUV_ENCODER
#cmakedefine CONFIG_MJPEG_DECODER
#cmakedefine CONFIG_MJPEG_ENCODER
#cmakedefine CONFIG_MOV_MUXER
#cmakedefine CONFIG_MATROSKA_MUXER
#cmakedefine CONFIG_MP2_ENCODER
#cmakedefine CONFIG_MP4_MUXER
#cmakedefine CONFIG_MPEG1VIDEO_DECODER
#cmakedefine CONFIG_MPEG1VIDEO_ENCODER
#cmakedefine CONFIG_MPEG2VIDEO_DECODER
#cmakedefine CONFIG_MPEG2VIDEO_ENCODER
#cmakedefine CONFIG_MPEG4_DECODER
#cmakedefine CONFIG_MPEG4_ENCODER
#cmakedefine CONFIG_MPEGAUDIO_HP
#cmakedefine CONFIG_MSMPEG4V2_DECODER
#cmakedefine CONFIG_MSMPEG4V3_DECODER
#cmakedefine CONFIG_MSMPEG4V3_ENCODER
#cmakedefine CONFIG_MUXERS
#cmakedefine CONFIG_PSP_MUXER
#cmakedefine CONFIG_QDM2_DECODER
#cmakedefine CONFIG_RISKY
#cmakedefine CONFIG_SNOW_DECODER
#cmakedefine CONFIG_SNOW_ENCODER
#cmakedefine CONFIG_SVQ3_DECODER
#cmakedefine CONFIG_TG2_MUXER
#cmakedefine CONFIG_TGP_MUXER
#cmakedefine CONFIG_TSCC_DECODER
#cmakedefine CONFIG_VC1_DECODER
#cmakedefine CONFIG_WMAV2_DECODER
#cmakedefine CONFIG_WMV1_DECODER
#cmakedefine CONFIG_WMV2_DECODER
#cmakedefine CONFIG_WMV3_DECODER
#cmakedefine CONFIG_ZLIB

/* Name mangling */
#cmakedefine CYG_MANGLING

/* Mad */
#cmakedefine FPM_DEFAULT
#cmakedefine FPM_INTEL
#cmakedefine FPM_PPC
#cmakedefine FPM_SPARC

/* Using GCC 2.9x.x */
#cmakedefine GCC_2_95_X

/* gettext package name */
#cmakedefine GETTEXT_PACKAGE

/* AltiVec for mpeg2enc */
#cmakedefine HAVE_ALTIVEC

/* Enable AltiVec by default */
#cmakedefine HAVE_ALTIVEC_H

/* Enable AltiVec by default */
#cmakedefine HAVE_BUILTIN_VECTOR

/* FFMPEG */
#cmakedefine HAVE_FAST_UNALIGNED
#cmakedefine HAVE_FAST_64BIT
#cmakedefine HAVE_SSSE3

/* FontConfig detected */
#cmakedefine HAVE_FONTCONFIG

/* Define if the GNU gettext() function is already present or preinstalled. */
#cmakedefine HAVE_GETTEXT

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the `mp3lame' library (-lmp3lame). */
#cmakedefine HAVE_LIBMP3LAME

/* lrintf exists? */
#cmakedefine HAVE_LRINTF

/* Use malloc.h */
#cmakedefine HAVE_MALLOC_H

/* post proc */
#cmakedefine HAVE_MMX
#cmakedefine ENABLE_MMX ${ENABLE_MMX}
#cmakedefine ENABLE_THREADS ${ENABLE_THREADS}

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* HAVE_THREADS */
#cmakedefine HAVE_THREADS

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* stricter prototyping */
#cmakedefine ICONV_NEED_CONST

/* use classing FAAD support */
#cmakedefine OLD_FAAD_PROTO

/* OSS detected */
#cmakedefine OSS_SUPPORT

/* Name of package */
#cmakedefine PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME

/* Define to the full name and version of this package. */
#cmakedefine PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#cmakedefine PACKAGE_TARNAME

/* Define to the version of this package. */
#cmakedefine PACKAGE_VERSION

/* Let FFmpeg decide which is faster */
#cmakedefine RUNTIME_CPUDETECT

/* use liba52 */
#cmakedefine USE_AC3

/* use Aften AC3 encoder */
#cmakedefine USE_AFTEN

/* Use Aften 0.07 */
#cmakedefine USE_AFTEN_07

/* Use Aften 0.08 */
#cmakedefine USE_AFTEN_08

/* AltiVec for mpeg2enc */
#cmakedefine USE_ALTIVEC

/* Tell avidemux to use libamrnb */
#cmakedefine USE_AMR_NB

/* aRts detected */
#cmakedefine USE_ARTS

/* ESD detected */
#cmakedefine USE_ESD

/* Jack detected */
#cmakedefine USE_JACK

/* Use faac audio enccoder */
#cmakedefine USE_FAAC

/* FAAD2 detected */
#cmakedefine USE_FAAD

/* FFmpeg */
#cmakedefine USE_FFMPEG

/* FontConfig detected */
#cmakedefine USE_FONTCONFIG

/* FreeType2 detected */
#cmakedefine USE_FREETYPE

/* use late binding of selected libraries */
#cmakedefine USE_LATE_BINDING

/* libdca detected */
#cmakedefine USE_LIBDCA

/* Libxml2 is available */
#cmakedefine USE_LIBXML2

/* MJPEG */
#cmakedefine USE_MJPEG

/* use libmad */
#cmakedefine USE_MP3

/* libpng is available */
#cmakedefine USE_PNG

/* use libsamplerate */
#cmakedefine USE_SRC

/* SDL detected */
#cmakedefine USE_SDL

/* Vorbis detected */
#cmakedefine USE_VORBIS

/* use x264 encoder */
#cmakedefine USE_X264

/* XVideo detected */
#cmakedefine USE_XV

/* use Xvid 1.x API */
#cmakedefine USE_XVID_4

/* use Xvid 0.9 API */
#cmakedefine USE_XX_XVID

/* Version number of package */
#define  VERSION "${VERSION}"

/* Big endian CPU - SPARC or PowerPC */
#cmakedefine WORDS_BIGENDIAN

/* use Nvwa memory leak detector */
#cmakedefine FIND_LEAKS
