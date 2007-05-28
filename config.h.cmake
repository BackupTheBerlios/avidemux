/* config.h.in.  Generated from configure.in by autoheader.  */

/* MPEG2DEC */
#cmakedefine ACCEL_DETECT

#if ${WIN32_DEF}
#define rindex ADM_rindex
#define index ADM_index
#define ftello ftello_adm
#define fseeko fseeko_adm
#endif
/* Big endian CPU - SPARC or PowerPC */
#cmakedefine ADM_BIG_ENDIAN

/* BSD OS specific ifdef */
#cmakedefine ADM_BSD_FAMILY

/* be verbose */
#cmakedefine ADM_DEBUG

#cmakedefine HAVE_AUDIO

/* Sparc workstations */
#cmakedefine ADM_SPARC

/* "Build for windows 32bits" */
#cmakedefine ADM_WIN32

/* "ALSA is 1.0" */
#cmakedefine ALSA_1_0_SUPPORT

/* "use ALSA as possible audio device" */
#cmakedefine ALSA_SUPPORT

/* AMR_NB */
#cmakedefine AMR_NB

/* "X86_64 AMD64 assembly" */
#cmakedefine ARCH_64_BITS

/* Enable PowerPC optim */
#cmakedefine ARCH_POWERPC

/* AltiVec for libmpeg2 */
#cmakedefine ARCH_PPC

/* post proc */
#cmakedefine ARCH_X86

/* "X86_32 assembly" */
#cmakedefine ARCH_X86_32

/* "X86_64 AMD64 assembly" */
#cmakedefine ARCH_X86_64

/* CONFIG_AC3_ENCODER */
#cmakedefine CONFIG_AC3_ENCODER

/* AMR_NB */
#cmakedefine CONFIG_AMR_NB

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_CYUV_DECODER

/* Inform FFmpeg we are using Darwin Mac OS X */
#cmakedefine CONFIG_DARWIN

/* FFMPEG */
#cmakedefine CONFIG_DECODERS

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_DVVIDEO_DECODER

/* DV encoder */
#cmakedefine CONFIG_DVVIDEO_ENCODER

/* FFMPEG */
#cmakedefine CONFIG_ENCODERS

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_FFV1_DECODER

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_FFV1_ENCODER

/* CONFIG_FFVHUFF_ENCODER */
#cmakedefine CONFIG_FFVHUFF_DECODER

/* CONFIG_FFVHUFF_ENCODER */
#cmakedefine CONFIG_FFVHUFF_ENCODER

/* CONFIG_H263P_ENCODER */
#cmakedefine CONFIG_H263P_DECODER

/* CONFIG_H263P_ENCODER */
#cmakedefine CONFIG_H263P_ENCODER

/* CONFIG_H263_ENCODER */
#cmakedefine CONFIG_H263_DECODER

/* CONFIG_H263_ENCODER */
#cmakedefine CONFIG_H263_ENCODER

/* CONFIG_H263P_ENCODER */
#cmakedefine CONFIG_H264_DECODER

/* CONFIG_HUFFYUV_ENCODER */
#cmakedefine CONFIG_HUFFYUV_DECODER

/* CONFIG_HUFFYUV_ENCODER */
#cmakedefine CONFIG_HUFFYUV_ENCODER

/* CONFIG_MJPEG_ENCODER */
#cmakedefine CONFIG_MJPEG_DECODER

/* CONFIG_MJPEG_ENCODER */
#cmakedefine CONFIG_MJPEG_ENCODER

/* CONFIG_MUXERS */
#cmakedefine CONFIG_MOV_MUXER

/* CONFIG_MP2_ENCODER */
#cmakedefine CONFIG_MP2_ENCODER

/* CONFIG_MUXERS */
#cmakedefine CONFIG_MP4_MUXER

/* CONFIG_MPEG1VIDEO_ENCODER */
#cmakedefine CONFIG_MPEG1VIDEO_DECODER

/* CONFIG_MPEG1VIDEO_ENCODER */
#cmakedefine CONFIG_MPEG1VIDEO_ENCODER

/* CONFIG_MPEG2VIDEO_ENCODER */
#cmakedefine CONFIG_MPEG2VIDEO_DECODER

/* CONFIG_MPEG2VIDEO_ENCODER */
#cmakedefine CONFIG_MPEG2VIDEO_ENCODER

/* CONFIG_MPEG4_ENCODER */
#cmakedefine CONFIG_MPEG4_DECODER

/* CONFIG_MPEG4_ENCODER */
#cmakedefine CONFIG_MPEG4_ENCODER

/* FFMPEG */
#cmakedefine CONFIG_MPEGAUDIO_HP

/* CONFIG_MSMPEG4V3_ENCODER */
#cmakedefine CONFIG_MSMPEG4V2_DECODER

/* CONFIG_MSMPEG4V3_ENCODER */
#cmakedefine CONFIG_MSMPEG4V3_DECODER

/* CONFIG_MSMPEG4V3_ENCODER */
#cmakedefine CONFIG_MSMPEG4V3_ENCODER

/* CONFIG_MUXERS */
#cmakedefine CONFIG_MUXERS

/* CONFIG_MUXERS */
#cmakedefine CONFIG_PSP_MUXER

/* FFMPEG */
#cmakedefine CONFIG_QDM2_DECODER

/* FFMPEG */
#cmakedefine CONFIG_RISKY

/* CONFIG_SNOW_DECODER */
#cmakedefine CONFIG_SNOW_DECODER

/* CONFIG_SNOW_ENCODER */
#cmakedefine CONFIG_SNOW_ENCODER

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_SVQ3_DECODER

/* CONFIG_MUXERS */
#cmakedefine CONFIG_TG2_MUXER

/* CONFIG_MUXERS */
#cmakedefine CONFIG_TGP_MUXER

/* CONFIG_MPEG1VIDEO_ENCODER */
#cmakedefine CONFIG_TSCC_DECODER

/* FFMPEG */
#cmakedefine CONFIG_VC1_DECODER

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_WMAV2_DECODER

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_WMV1_DECODER

/* CONFIG_FFV1_ENCODER */
#cmakedefine CONFIG_WMV2_DECODER

/* FFMPEG */
#cmakedefine CONFIG_WMV3_DECODER

/* CONFIG_MPEG1VIDEO_ENCODER */
#cmakedefine CONFIG_ZLIB

/* "Name mangling" */
#cmakedefine CYG_MANGLING

/* roundup function */
#cmakedefine EMULATE_FAST_INT

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#cmakedefine ENABLE_NLS

/* Mad */
#cmakedefine FPM_DEFAULT

/* Mad */
#cmakedefine FPM_INTEL

/* Mad */
#cmakedefine FPM_PPC

/* Mad */
#cmakedefine FPM_SPARC

/* Using GCC 2.9x.x */
#cmakedefine GCC_2_95_X

/* gettext package name */
#cmakedefine GETTEXT_PACKAGE

/* Define to 1 if you have the <alsa/asoundlib.h> header file. */
#cmakedefine HAVE_ALSA_ASOUNDLIB_H

/* AltiVec for mpeg2enc */
#cmakedefine HAVE_ALTIVEC

/* Enable AltiVec by default */
#cmakedefine HAVE_ALTIVEC_H

/* Define to 1 if you have the <artsc.h> header file. */
#cmakedefine HAVE_ARTSC_H

/* Enable AltiVec by default */
#cmakedefine HAVE_BUILTIN_VECTOR

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
#cmakedefine HAVE_CFLOCALECOPYCURRENT

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
#cmakedefine HAVE_CFPREFERENCESCOPYAPPVALUE

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#cmakedefine HAVE_DCGETTEXT

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H

/* Define to 1 if you have the <dts.h> header file. */
#cmakedefine HAVE_DTS_H

/* Define to 1 if you have the <dts_internal.h> header file. */
#cmakedefine HAVE_DTS_INTERNAL_H

/* Define to 1 if you have the <faac.h> header file. */
#cmakedefine HAVE_FAAC_H

/* Define to 1 if you have the <faad.h> header file. */
#cmakedefine HAVE_FAAD_H

/* "FontConfig detected" */
#cmakedefine HAVE_FONTCONFIG

/* Define if the GNU gettext() function is already present or preinstalled. */
#cmakedefine HAVE_GETTEXT

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY

/* Define if you have the iconv() function. */
#cmakedefine HAVE_ICONV

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the <lame/lame.h> header file. */
#cmakedefine HAVE_LAME_LAME_H

/* Define to 1 if you have the `aften' library (-laften). */
#cmakedefine HAVE_LIBAFTEN

/* Define to 1 if you have the `amrnb' library (-lamrnb). */
#cmakedefine HAVE_LIBAMRNB

/* Define to 1 if you have the `asound' library (-lasound). */
#cmakedefine HAVE_LIBASOUND

/* Define to 1 if you have the `dts' library (-ldts). */
#cmakedefine HAVE_LIBDTS

/* Define to 1 if you have the `esd' library (-lesd). */
#cmakedefine HAVE_LIBESD

/* Define to 1 if you have the `faac' library (-lfaac). */
#cmakedefine HAVE_LIBFAAC

/* Define to 1 if you have the `faad' library (-lfaad). */
#cmakedefine HAVE_LIBFAAD

/* Define to 1 if you have the `fontconfig' library (-lfontconfig). */
#cmakedefine HAVE_LIBFONTCONFIG

/* Define to 1 if you have the `mp3lame' library (-lmp3lame). */
#cmakedefine HAVE_LIBMP3LAME

/* Define to 1 if you have the `png' library (-lpng). */
#cmakedefine HAVE_LIBPNG

/* Define to 1 if you have the `vorbis' library (-lvorbis). */
#cmakedefine HAVE_LIBVORBIS

/* Define to 1 if you have the `vorbisenc' library (-lvorbisenc). */
#cmakedefine HAVE_LIBVORBISENC

/* Define to 1 if you have the `x264' library (-lx264). */
#cmakedefine HAVE_LIBX264

/* Define to 1 if you have the <libxml/parser.h> header file. */
#cmakedefine HAVE_LIBXML_PARSER_H

/* Define to 1 if you have the `Xv' library (-lXv). */
#cmakedefine HAVE_LIBXV

/* Define to 1 if you have the `xvidcore' library (-lxvidcore). */
#cmakedefine HAVE_LIBXVIDCORE

/* roundup function */
#cmakedefine HAVE_LRINTF

/* Use malloc.h */
#cmakedefine HAVE_MALLOC_H

/* Define to 1 if you have the <malloc/malloc.h> header file. */
#cmakedefine HAVE_MALLOC_MALLOC_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H

/* post proc */
#cmakedefine HAVE_MMX

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the <sys/shm.h> header file. */
#cmakedefine HAVE_SYS_SHM_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* HAVE_THREADS */
#cmakedefine HAVE_THREADS

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* Define to 1 if you have the <X11/extensions/XShm.h> header file. */
#cmakedefine HAVE_X11_EXTENSIONS_XSHM_H

/* Define to 1 if you have the <X11/extensions/Xvlib.h> header file. */
#cmakedefine HAVE_X11_EXTENSIONS_XVLIB_H

/* stricter prototyping */
#cmakedefine ICONV_NEED_CONST

/* "SSE on Cywin" */
#cmakedefine MEMALIGN_HACK

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

/* "Let FFmpeg decide which is faster" */
#cmakedefine RUNTIME_CPUDETECT

/* Define to 1 if you have the ANSI C header files. */
#cmakedefine STDC_HEADERS

/* use liba52 */
#cmakedefine USE_AC3

/* use Aften AC3 encoder */
#cmakedefine USE_AFTEN

/* Use Aften 0.05 or earlier */
#cmakedefine USE_AFTEN_05

/* Use Aften 0.06 */
#cmakedefine USE_AFTEN_06

/* AltiVec for mpeg2enc */
#cmakedefine USE_ALTIVEC

/* Tell avidemux to use libamrnb */
#cmakedefine USE_AMR_NB

/* "aRts detected" */
#cmakedefine USE_ARTS

/* ESD detected */
#cmakedefine USE_ESD

/* "Use faac audio enccoder" */
#cmakedefine USE_FAAC

/* FAAD2 detected */
#cmakedefine USE_FAAD

/* FFmpeg */
#cmakedefine USE_FFMPEG

/* "FontConfig detected" */
#cmakedefine USE_FONTCONFIG

/* "FreeType2 detected" */
#cmakedefine USE_FREETYPE

/* "use late binding of selected libraries" */
#cmakedefine USE_LATE_BINDING

/* "libdca detected" */
#cmakedefine USE_LIBDCA

/* Libxml2 is available */
#cmakedefine USE_LIBXML2

/* MJPEG */
#cmakedefine USE_MJPEG

/* use libmad */
#cmakedefine USE_MP3

/* libpng is available */
#cmakedefine USE_PNG

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
#cmakedefine VERSION

/* Big endian CPU - SPARC or PowerPC */
#cmakedefine WORDS_BIGENDIAN

/* Define to 1 if the X Window System is missing or not being used. */
#cmakedefine X_DISPLAY_MISSING

/* "" */
#cmakedefine fseeko

/* "" */
#cmakedefine ftello

/* "rindex does not exist on Cygwin" */
#cmakedefine index

/* roundup function */
#cmakedefine restrict

/* "rindex does not exist on Cygwin" */
#cmakedefine rindex
