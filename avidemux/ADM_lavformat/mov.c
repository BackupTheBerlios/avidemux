/*
 * MOV decoder.
 * Copyright (c) 2001 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <limits.h>

//#define DEBUG

#include "avformat.h"
#include "avi.h"
#include "mov.h"

#ifdef CONFIG_ZLIB
#include <zlib.h>
#endif

/*
 * First version by Francois Revol revol@free.fr
 * Seek function by Gael Chardon gael.dev@4now.net
 *
 * Features and limitations:
 * - reads most of the QT files I have (at least the structure),
 *   the exceptions are .mov with zlib compressed headers ('cmov' section). It shouldn't be hard to implement.
 *   FIXED, Francois Revol, 07/17/2002
 * - ffmpeg has nearly none of the usual QuickTime codecs,
 *   although I succesfully dumped raw and mp3 audio tracks off .mov files.
 *   Sample QuickTime files with mp3 audio can be found at: http://www.3ivx.com/showcase.html
 * - .mp4 parsing is still hazardous, although the format really is QuickTime with some minor changes
 *   (to make .mov parser crash maybe ?), despite what they say in the MPEG FAQ at
 *   http://mpeg.telecomitalialab.com/faq.htm
 * - the code is quite ugly... maybe I won't do it recursive next time :-)
 * - seek is not supported with files that contain edit list
 *
 * Funny I didn't know about http://sourceforge.net/projects/qt-ffmpeg/
 * when coding this :) (it's a writer anyway)
 *
 * Reference documents:
 * http://www.geocities.com/xhelmboyx/quicktime/formats/qtm-layout.txt
 * Apple:
 *  http://developer.apple.com/documentation/QuickTime/QTFF/
 *  http://developer.apple.com/documentation/QuickTime/QTFF/qtff.pdf
 * QuickTime is a trademark of Apple (AFAIK :))
 */



#undef NDEBUG
#include <assert.h>

/* Allows seeking */
#define MOV_SEEK

/* Special handling for movies created with Minolta Dimaxe Xi*/
/* this fix should not interfere with other .mov files, but just in case*/
#define MOV_MINOLTA_FIX

/* some streams in QT (and in MP4 mostly) aren't either video nor audio */
/* so we first list them as this, then clean up the list of streams we give back, */
/* getting rid of these */
#define CODEC_TYPE_MOV_OTHER    (enum CodecType) 2

/* http://gpac.sourceforge.net/tutorial/mediatypes.htm */
const CodecTag ff_mov_obj_type[] = {
    { CODEC_ID_MPEG4     ,  32 },
    { CODEC_ID_H264      ,  33 },
    { CODEC_ID_AAC       ,  64 },
    { CODEC_ID_MPEG2VIDEO,  96 }, /* MPEG2 Simple */
    { CODEC_ID_MPEG2VIDEO,  97 }, /* MPEG2 Main */
    { CODEC_ID_MPEG2VIDEO,  98 }, /* MPEG2 SNR */
    { CODEC_ID_MPEG2VIDEO,  99 }, /* MPEG2 Spatial */
    { CODEC_ID_MPEG2VIDEO, 100 }, /* MPEG2 High */
    { CODEC_ID_MPEG2VIDEO, 101 }, /* MPEG2 422 */
    { CODEC_ID_AAC       , 102 }, /* MPEG2 AAC Main */
    { CODEC_ID_AAC       , 103 }, /* MPEG2 AAC Low */
    { CODEC_ID_AAC       , 104 }, /* MPEG2 AAC SSR */
    { CODEC_ID_MP3       , 107 }, /* MEANX Compatibility with xine was 105*/
    { CODEC_ID_MPEG1VIDEO, 106 },
    { CODEC_ID_MP2       , 107 },
    { CODEC_ID_MJPEG     , 108 },
    { CODEC_ID_PCM_S16LE , 224 },
    { CODEC_ID_VORBIS    , 225 },
    { CODEC_ID_AC3       , 226 },
    { CODEC_ID_PCM_ALAW  , 227 },
    { CODEC_ID_PCM_MULAW , 228 },
    { CODEC_ID_PCM_S16BE , 230 },
    { CODEC_ID_H263      , 242 },
    { CODEC_ID_H261      , 243 },
    { 0, 0 },
};

static const CodecTag mov_video_tags[] = {
/*  { CODEC_ID_, MKTAG('c', 'v', 'i', 'd') }, *//* Cinepak */
/*  { CODEC_ID_H263, MKTAG('r', 'a', 'w', ' ') }, *//* Uncompressed RGB */
/*  { CODEC_ID_H263, MKTAG('Y', 'u', 'v', '2') }, *//* Uncompressed YUV422 */
/*    { CODEC_ID_RAWVIDEO, MKTAG('A', 'V', 'U', 'I') }, *//* YUV with alpha-channel (AVID Uncompressed) */
/* Graphics */
/* Animation */
/* Apple video */
/* Kodak Photo CD */
    { CODEC_ID_MJPEG, MKTAG('j', 'p', 'e', 'g') }, /* PhotoJPEG */
    { CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'e', 'g') }, /* MPEG */
    { CODEC_ID_MJPEG, MKTAG('m', 'j', 'p', 'a') }, /* Motion-JPEG (format A) */
    { CODEC_ID_MJPEG, MKTAG('m', 'j', 'p', 'b') }, /* Motion-JPEG (format B) */
    { CODEC_ID_MJPEG, MKTAG('A', 'V', 'D', 'J') }, /* MJPEG with alpha-channel (AVID JFIF meridien compressed) */
/*    { CODEC_ID_MJPEG, MKTAG('A', 'V', 'R', 'n') }, *//* MJPEG with alpha-channel (AVID ABVB/Truevision NuVista) */
/*    { CODEC_ID_GIF, MKTAG('g', 'i', 'f', ' ') }, *//* embedded gif files as frames (usually one "click to play movie" frame) */
/* Sorenson video */
    { CODEC_ID_SVQ1, MKTAG('S', 'V', 'Q', '1') }, /* Sorenson Video v1 */
    { CODEC_ID_SVQ1, MKTAG('s', 'v', 'q', '1') }, /* Sorenson Video v1 */
    { CODEC_ID_SVQ1, MKTAG('s', 'v', 'q', 'i') }, /* Sorenson Video v1 (from QT specs)*/
    { CODEC_ID_SVQ3, MKTAG('S', 'V', 'Q', '3') }, /* Sorenson Video v3 */
    { CODEC_ID_MPEG4, MKTAG('m', 'p', '4', 'v') },
    { CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', 'X') }, /* OpenDiVX *//* sample files at http://heroinewarrior.com/xmovie.php3 use this tag */
    { CODEC_ID_MPEG4, MKTAG('X', 'V', 'I', 'D') },
    { CODEC_ID_MPEG4, MKTAG('3', 'I', 'V', '2') }, /* experimental: 3IVX files before ivx D4 4.5.1 */
/*    { CODEC_ID_, MKTAG('I', 'V', '5', '0') }, *//* Indeo 5.0 */
    { CODEC_ID_H263, MKTAG('h', '2', '6', '3') }, /* H263 */
    { CODEC_ID_H263, MKTAG('s', '2', '6', '3') }, /* H263 ?? works */
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'c', ' ') }, /* DV NTSC */
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'c', 'p') }, /* DV PAL */
/*    { CODEC_ID_DVVIDEO, MKTAG('A', 'V', 'd', 'v') }, *//* AVID dv */
    { CODEC_ID_VP3, MKTAG('V', 'P', '3', '1') }, /* On2 VP3 */
    { CODEC_ID_RPZA, MKTAG('r', 'p', 'z', 'a') }, /* Apple Video (RPZA) */
    { CODEC_ID_CINEPAK, MKTAG('c', 'v', 'i', 'd') }, /* Cinepak */
    { CODEC_ID_8BPS, MKTAG('8', 'B', 'P', 'S') }, /* Planar RGB (8BPS) */
    { CODEC_ID_SMC, MKTAG('s', 'm', 'c', ' ') }, /* Apple Graphics (SMC) */
    { CODEC_ID_QTRLE, MKTAG('r', 'l', 'e', ' ') }, /* Apple Animation (RLE) */
    { CODEC_ID_QDRAW, MKTAG('q', 'd', 'r', 'w') }, /* QuickDraw */
    { CODEC_ID_H264, MKTAG('a', 'v', 'c', '1') }, /* AVC-1/H.264 */
    { CODEC_ID_MPEG2VIDEO, MKTAG('h', 'd', 'v', '2') }, /* MPEG2 produced by Sony HD camera */
    { CODEC_ID_MPEG2VIDEO, MKTAG('h', 'd', 'v', '3') }, /* HDV produced by FCP */
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'x', '5', 'n') }, /* MPEG2 IMX NTSC 525/60 50mb/s produced by FCP */
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'x', '5', 'p') }, /* MPEG2 IMX PAL 625/50 50mb/s produced by FCP */
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'x', '3', 'n') }, /* MPEG2 IMX NTSC 525/60 30mb/s produced by FCP */
    { CODEC_ID_MPEG2VIDEO, MKTAG('m', 'x', '3', 'p') }, /* MPEG2 IMX PAL 625/50 30mb/s produced by FCP */
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'p', 'p') }, /* DVCPRO PAL produced by FCP */
    //{ CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'h', '5') }, /* DVCPRO HD 50i produced by FCP */
    //{ CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'h', '6') }, /* DVCPRO HD 60i produced by FCP */
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', '5', 'p') }, /* DVCPRO50 PAL produced by FCP */
    { CODEC_ID_DVVIDEO, MKTAG('d', 'v', '5', 'n') }, /* DVCPRO50 NTSC produced by FCP */
    //{ CODEC_ID_JPEG2000, MKTAG('m', 'j', 'p', '2') }, /* JPEG 2000 produced by FCP */
    { CODEC_ID_NONE, 0 },
};

static const CodecTag mov_audio_tags[] = {
    { CODEC_ID_PCM_S32BE, MKTAG('i', 'n', '3', '2') },
    { CODEC_ID_PCM_S24BE, MKTAG('i', 'n', '2', '4') },
/*    { CODEC_ID_PCM_S16BE, MKTAG('N', 'O', 'N', 'E') }, *//* uncompressed */
    { CODEC_ID_PCM_S16BE, MKTAG('t', 'w', 'o', 's') }, /* 16 bits */
    /* { CODEC_ID_PCM_S8, MKTAG('t', 'w', 'o', 's') },*/ /* 8 bits */
    { CODEC_ID_PCM_U8, MKTAG('r', 'a', 'w', ' ') }, /* 8 bits unsigned */
    { CODEC_ID_PCM_S16LE, MKTAG('s', 'o', 'w', 't') }, /*  */
    { CODEC_ID_PCM_MULAW, MKTAG('u', 'l', 'a', 'w') }, /*  */
    { CODEC_ID_PCM_ALAW, MKTAG('a', 'l', 'a', 'w') }, /*  */
    { CODEC_ID_ADPCM_IMA_QT, MKTAG('i', 'm', 'a', '4') }, /* IMA-4 ADPCM */
    { CODEC_ID_MACE3, MKTAG('M', 'A', 'C', '3') }, /* Macintosh Audio Compression and Expansion 3:1 */
    { CODEC_ID_MACE6, MKTAG('M', 'A', 'C', '6') }, /* Macintosh Audio Compression and Expansion 6:1 */

    { CODEC_ID_MP2, MKTAG('.', 'm', 'p', '3') }, /* MPEG layer 3 */ /* sample files at http://www.3ivx.com/showcase.html use this tag */
    { CODEC_ID_MP2, 0x6D730055 }, /* MPEG layer 3 */
    { CODEC_ID_MP2, 0x5500736D }, /* MPEG layer 3 *//* XXX: check endianness */
/*    { CODEC_ID_OGG_VORBIS, MKTAG('O', 'g', 'g', 'S') }, *//* sample files at http://heroinewarrior.com/xmovie.php3 use this tag */
/* MP4 tags */
    { CODEC_ID_AAC, MKTAG('m', 'p', '4', 'a') }, /* MPEG-4 AAC */
    /* The standard for mpeg4 audio is still not normalised AFAIK anyway */
    { CODEC_ID_AMR_NB, MKTAG('s', 'a', 'm', 'r') }, /* AMR-NB 3gp */
    { CODEC_ID_AMR_WB, MKTAG('s', 'a', 'w', 'b') }, /* AMR-WB 3gp */
    { CODEC_ID_AC3, MKTAG('m', 's', 0x20, 0x00) }, /* Dolby AC-3 */
    { CODEC_ID_ALAC,MKTAG('a', 'l', 'a', 'c') }, /* Apple Lossless */
    { CODEC_ID_QDM2,MKTAG('Q', 'D', 'M', '2') }, /* QDM2 */
    { CODEC_ID_NONE, 0 },
};

/* map numeric codes from mdhd atom to ISO 639 */
/* cf. QTFileFormat.pdf p253, qtff.pdf p205 */
/* http://developer.apple.com/documentation/mac/Text/Text-368.html */
/* deprecated by putting the code as 3*5bit ascii */
static const char *mov_mdhd_language_map[] = {
/* 0-9 */
"eng", "fra", "ger", "ita", "dut", "sve", "spa", "dan", "por", "nor",
"heb", "jpn", "ara", "fin", "gre", "ice", "mlt", "tur", "hr "/*scr*/, "chi"/*ace?*/,
"urd", "hin", "tha", "kor", "lit", "pol", "hun", "est", "lav",  NULL,
"fo ",  NULL, "rus", "chi",  NULL, "iri", "alb", "ron", "ces", "slk",
"slv", "yid", "sr ", "mac", "bul", "ukr", "bel", "uzb", "kaz", "aze",
/*?*/
"aze", "arm", "geo", "mol", "kir", "tgk", "tuk", "mon",  NULL, "pus",
"kur", "kas", "snd", "tib", "nep", "san", "mar", "ben", "asm", "guj",
"pa ", "ori", "mal", "kan", "tam", "tel",  NULL, "bur", "khm", "lao",
/*                   roman? arabic? */
"vie", "ind", "tgl", "may", "may", "amh", "tir", "orm", "som", "swa",
/*==rundi?*/
 NULL, "run",  NULL, "mlg", "epo",  NULL,  NULL,  NULL,  NULL,  NULL,
/* 100 */
 NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
 NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
 NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, "wel", "baq",
"cat", "lat", "que", "grn", "aym", "tat", "uig", "dzo", "jav"
};

/* the QuickTime file format is quite convoluted...
 * it has lots of index tables, each indexing something in another one...
 * Here we just use what is needed to read the chunks
 */

typedef struct MOV_sample_to_chunk_tbl {
    long first;
    long count;
    long id;
} MOV_sample_to_chunk_tbl;

typedef struct {
    uint32_t type;
    int64_t offset;
    int64_t size; /* total size (excluding the size and type fields) */
} MOV_atom_t;

typedef struct {
    int seed;
    int flags;
    int size;
    void* clrs;
} MOV_ctab_t;

typedef struct {
    uint8_t  version;
    uint32_t flags; // 24bit

    /* 0x03 ESDescrTag */
    uint16_t es_id;
#define MP4ODescrTag                    0x01
#define MP4IODescrTag                   0x02
#define MP4ESDescrTag                   0x03
#define MP4DecConfigDescrTag            0x04
#define MP4DecSpecificDescrTag          0x05
#define MP4SLConfigDescrTag             0x06
#define MP4ContentIdDescrTag            0x07
#define MP4SupplContentIdDescrTag       0x08
#define MP4IPIPtrDescrTag               0x09
#define MP4IPMPPtrDescrTag              0x0A
#define MP4IPMPDescrTag                 0x0B
#define MP4RegistrationDescrTag         0x0D
#define MP4ESIDIncDescrTag              0x0E
#define MP4ESIDRefDescrTag              0x0F
#define MP4FileIODescrTag               0x10
#define MP4FileODescrTag                0x11
#define MP4ExtProfileLevelDescrTag      0x13
#define MP4ExtDescrTagsStart            0x80
#define MP4ExtDescrTagsEnd              0xFE
    uint8_t  stream_priority;

    /* 0x04 DecConfigDescrTag */
    uint8_t  object_type_id;
    uint8_t  stream_type;
    /* XXX: really streamType is
     * only 6bit, followed by:
     * 1bit  upStream
     * 1bit  reserved
     */
    uint32_t buffer_size_db; // 24
    uint32_t max_bitrate;
    uint32_t avg_bitrate;

    /* 0x05 DecSpecificDescrTag */
    uint8_t  decoder_cfg_len;
    uint8_t *decoder_cfg;

    /* 0x06 SLConfigDescrTag */
    uint8_t  sl_config_len;
    uint8_t *sl_config;
} MOV_esds_t;

struct MOVParseTableEntry;

typedef struct MOVStreamContext {
    int ffindex; /* the ffmpeg stream id */
    int is_ff_stream; /* Is this stream presented to ffmpeg ? i.e. is this an audio or video stream ? */
    long next_chunk;
    long chunk_count;
    int64_t *chunk_offsets;
    int stts_count;
    Time2Sample *stts_data;
    int ctts_count;
    Time2Sample *ctts_data;
    int edit_count;             /* number of 'edit' (elst atom) */
    long sample_to_chunk_sz;
    MOV_sample_to_chunk_tbl *sample_to_chunk;
    long sample_to_chunk_index;
    int sample_to_time_index;
    long sample_to_time_sample;
    uint64_t sample_to_time_time;
    int sample_to_ctime_index;
    int sample_to_ctime_sample;
    long sample_size;
    long sample_count;
    long *sample_sizes;
    long keyframe_count;
    long *keyframes;
    int time_scale;
    int time_rate;
    long current_sample;
    long left_in_chunk; /* how many samples before next chunk */
    MOV_esds_t esds;
} MOVStreamContext;

typedef struct MOVContext {
    int mp4; /* set to 1 as soon as we are sure that the file is an .mp4 file (even some header parsing depends on this) */
    AVFormatContext *fc;
    int time_scale;
    int duration; /* duration of the longest track */
    int found_moov; /* when both 'moov' and 'mdat' sections has been found */
    int found_mdat; /* we suppose we have enough data to read the file */
    int64_t mdat_size;
    int64_t mdat_offset;
    int ni;                                         ///< non interleaved mode
    int total_streams;
    /* some streams listed here aren't presented to the ffmpeg API, since they aren't either video nor audio
     * but we need the info to be able to skip data from those streams in the 'mdat' section
     */
    MOVStreamContext *streams[MAX_STREAMS];

    int64_t next_chunk_offset;
    MOVStreamContext *partial; /* != 0 : there is still to read in the current chunk */
    int ctab_size;
    MOV_ctab_t **ctab;           /* color tables */
    const struct MOVParseTableEntry *parse_table; /* could be eventually used to change the table */
    /* NOTE: for recursion save to/ restore from local variable! */

    AVPaletteControl palette_control;
} MOVContext;


/* XXX: it's the first time I make a recursive parser I think... sorry if it's ugly :P */

/* those functions parse an atom */
/* return code:
 1: found what I wanted, exit
 0: continue to parse next atom
 -1: error occured, exit
 */
typedef int (*mov_parse_function)(MOVContext *ctx, ByteIOContext *pb, MOV_atom_t atom);

/* links atom IDs to parse functions */
typedef struct MOVParseTableEntry {
    uint32_t type;
    mov_parse_function func;
} MOVParseTableEntry;

static int ff_mov_lang_to_iso639(int code, char *to)
{
    int i;
    /* is it the mangled iso code? */
    /* see http://www.geocities.com/xhelmboyx/quicktime/formats/mp4-layout.txt */
    if (code > 138) {
        for (i = 2; i >= 0; i--) {
            to[i] = 0x60 + (code & 0x1f);
            code >>= 5;
        }
        return 1;
    }
    /* old fashion apple lang code */
    if (code >= (sizeof(mov_mdhd_language_map)/sizeof(char *)))
        return 0;
    if (!mov_mdhd_language_map[code])
        return 0;
    strncpy(to, mov_mdhd_language_map[code], 4);
    return 1;
}

extern int ff_mov_iso639_to_lang(const char *lang, int mp4); /* for movenc.c */
int ff_mov_iso639_to_lang(const char *lang, int mp4)
{
    int i, code = 0;

    /* old way, only for QT? */
    for (i = 0; !mp4 && (i < (sizeof(mov_mdhd_language_map)/sizeof(char *))); i++) {
        if (mov_mdhd_language_map[i] && !strcmp(lang, mov_mdhd_language_map[i]))
            return i;
    }
    /* XXX:can we do that in mov too? */
    if (!mp4)
        return 0;
    /* handle undefined as such */
    if (lang[0] == '\0')
        lang = "und";
    /* 5bit ascii */
    for (i = 0; i < 3; i++) {
        unsigned char c = (unsigned char)lang[i];
        if (c < 0x60)
            return 0;
        if (c > 0x60 + 0x1f)
            return 0;
        code <<= 5;
        code |= (c - 0x60);
    }
    return code;
}

static int mov_read_leaf(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)
{
    if (atom.size>1)
        url_fskip(pb, atom.size);
/*        url_seek(pb, atom_offset+atom.size, SEEK_SET); */
    return 0;
}

