/* config.h.in.  Generated from configure.in by autoheader.  */

#define ADM_BUILD_CLI 1
#define ADM_BUILD_GTK 2
#define ADM_BUILD_QT4 3

// GCC - CPU
#cmakedefine ADM_BIG_ENDIAN
#cmakedefine ADM_CPU_64BIT
#cmakedefine ADM_CPU_ALTIVEC
#cmakedefine ADM_CPU_PPC
#cmakedefine ADM_CPU_SSSE3
#cmakedefine ADM_CPU_X86
#cmakedefine ADM_CPU_X86_32
#cmakedefine ADM_CPU_X86_64

// GCC - Operating System
#cmakedefine ADM_BSD_FAMILY

/* Jog Shuttle */
#cmakedefine USE_JOG

#define PACKAGE   "avidemux"
#define ADMLOCALE "${ADM_LOCALE}"

#cmakedefine HAVE_AUDIO

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* use ALSA as possible audio device */
#cmakedefine ALSA_SUPPORT
#endif

/* AMR_NB */
#cmakedefine AMR_NB

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
#cmakedefine USE_AFTEN_07	// 0.07
#cmakedefine USE_AFTEN_08	// 0.0.8

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
#cmakedefine USE_LIBDCA_002
#cmakedefine USE_LIBDCA_005

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

#if ${CONFIG_HEADER_TYPE} == ADM_BUILD_GTK || ${CONFIG_HEADER_TYPE} == ADM_BUILD_QT4
/* XVideo detected */
#cmakedefine USE_XV
#endif

/* use Xvid 1.x API */
#cmakedefine USE_XVID_4

/* use Xvid 0.9 API */
#cmakedefine USE_XX_XVID

/* Version number of package */
#define VERSION "${VERSION}"

/* use Nvwa memory leak detector */
#cmakedefine FIND_LEAKS

#ifdef __MINGW32__
#define rindex strrchr
#define index strchr
#define ftello ftello64
#define fseeko fseeko64
#endif
