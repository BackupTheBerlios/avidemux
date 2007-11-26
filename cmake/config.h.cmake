/* config.h.in.  Generated from configure.in by autoheader.  */

#define ADM_BUILD_CLI 1
#define ADM_BUILD_GTK 2
#define ADM_BUILD_QT4 3

/* Jog Shuttle */
#cmakedefine USE_JOG

/* MPEG2DEC */
#cmakedefine ACCEL_DETECT

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

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* use ALSA as possible audio device */
#cmakedefine ALSA_SUPPORT
#endif

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
#cmakedefine CONFIG_ENCODERS
#cmakedefine CONFIG_DVVIDEO_ENCODER

#cmakedefine CONFIG_DECODERS
#cmakedefine CONFIG_DVVIDEO_DECODER
#cmakedefine CONFIG_H263_DECODER
#cmakedefine CONFIG_MPEG4_DECODER
#cmakedefine CONFIG_MPEGAUDIO_HP
#cmakedefine CONFIG_SNOW_DECODER
#cmakedefine CONFIG_VC1_DECODER
#cmakedefine CONFIG_WMV2_DECODER
#cmakedefine CONFIG_WMV3_DECODER
#cmakedefine CONFIG_ZLIB

#cmakedefine CONFIG_MUXERS
#cmakedefine CONFIG_MOV_MUXER
#cmakedefine CONFIG_MP4_MUXER
#cmakedefine CONFIG_PSP_MUXER
#cmakedefine CONFIG_TG2_MUXER
#cmakedefine CONFIG_TGP_MUXER

#define ENABLE_MMX ${ENABLE_MMX}
#define ENABLE_THREADS ${ENABLE_THREADS}
#cmakedefine HAVE_FAST_UNALIGNED
#cmakedefine HAVE_LRINTF
#cmakedefine HAVE_MMX
#cmakedefine HAVE_THREADS
#cmakedefine RUNTIME_CPUDETECT

#cmakedefine HAVE_FAST_64BIT
#cmakedefine HAVE_SSSE3
#cmakedefine CONFIG_DARWIN

/* Name mangling */
#cmakedefine CYG_MANGLING

/* Mad */
#cmakedefine FPM_DEFAULT
#cmakedefine FPM_INTEL
#cmakedefine FPM_PPC
#cmakedefine FPM_SPARC

/* Using GCC 2.9x.x */
#cmakedefine GCC_2_95_X

/* AltiVec for mpeg2enc */
#cmakedefine HAVE_ALTIVEC

/* Enable AltiVec by default */
#cmakedefine HAVE_ALTIVEC_H

/* Enable AltiVec by default */
#cmakedefine HAVE_BUILTIN_VECTOR

/* FontConfig detected */
#cmakedefine HAVE_FONTCONFIG

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_CLI || ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK
/* Define if the GNU gettext() function is already present or preinstalled. */
#cmakedefine HAVE_GETTEXT
#endif

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the `mp3lame' library (-lmp3lame). */
#cmakedefine HAVE_LIBMP3LAME

/* Use malloc.h */
#cmakedefine HAVE_MALLOC_H

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* stricter prototyping */
#cmakedefine ICONV_NEED_CONST

/* use classing FAAD support */
#cmakedefine OLD_FAAD_PROTO

/* OSS detected */
#cmakedefine OSS_SUPPORT

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

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* aRts detected */
#cmakedefine USE_ARTS
#endif

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* ESD detected */
#cmakedefine USE_ESD
#endif

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

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* SDL detected */
#cmakedefine USE_SDL
#endif

/* Vorbis detected */
#cmakedefine USE_VORBIS

/* use x264 encoder */
#cmakedefine USE_X264

#if (!defined(__APPLE__) && (${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4)) || (defined(__APPLE__) && ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK)
/* XVideo detected */
#cmakedefine USE_XV
#endif

/* use Xvid 1.x API */
#cmakedefine USE_XVID_4

/* use Xvid 0.9 API */
#cmakedefine USE_XX_XVID

/* Version number of package */
#define VERSION "${VERSION}"

/* Big endian CPU - SPARC or PowerPC */
#cmakedefine WORDS_BIGENDIAN

/* use Nvwa memory leak detector */
#cmakedefine FIND_LEAKS

#cmakedefine ADM_OS_DARWIN
#cmakedefine ADM_OS_LINUX
#cmakedefine ADM_OS_WINDOWS

#cmakedefine ADM_CPU_X86
#cmakedefine ADM_CPU_X86_64

#ifdef ADM_OS_WINDOWS
#define rindex strrchr
#define index strchr
#define ftello ftello64
#define fseeko fseeko64
#endif
