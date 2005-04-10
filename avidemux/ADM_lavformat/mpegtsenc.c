/*
 * MPEG2 transport stream (aka DVB) mux
 * Copyright (c) 2003 Fabrice Bellard.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "avformat.h"

#include "mpegts.h"

/* write DVB SI sections */

#include "ADM_tscrc.h" 
unsigned int mpegts_crc32(const uint8_t *data, int len)
{
    register int i;
    unsigned int crc = 0xffffffff;
    
    for (i=0; i<len; i++)
        crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];
    
    return crc;
}

/*********************************************/
/* mpegts section writer */

typedef struct MpegTSSection {
    int pid;
    int cc;
    void (*write_packet)(struct MpegTSSection *s, const uint8_t *packet);
    void *opaque;
} MpegTSSection;

/* NOTE: 4 bytes must be left at the end for the crc32 */
void mpegts_write_section(MpegTSSection *s, uint8_t *buf, int len)
{
    unsigned int crc;
    unsigned char packet[TS_PACKET_SIZE];
    const unsigned char *buf_ptr;
    unsigned char *q;
    int first, b, len1, left;

    crc = mpegts_crc32(buf, len - 4);
    buf[len - 4] = (crc >> 24) & 0xff;
    buf[len - 3] = (crc >> 16) & 0xff;
    buf[len - 2] = (crc >> 8) & 0xff;
    buf[len - 1] = (crc) & 0xff;
    
    /* send each packet */
    buf_ptr = buf;
    while (len > 0) {
        first = (buf == buf_ptr);
        q = packet;
        *q++ = 0x47;
        b = (s->pid >> 8);
        if (first)
            b |= 0x40;
        *q++ = b;
        *q++ = s->pid;
        s->cc = (s->cc + 1) & 0xf;
        *q++ = 0x10 | s->cc;
        if (first)
            *q++ = 0; /* 0 offset */
        len1 = TS_PACKET_SIZE - (q - packet);
        if (len1 > len) 
            len1 = len;
        memcpy(q, buf_ptr, len1);
        q += len1;
        /* add known padding data */
        left = TS_PACKET_SIZE - (q - packet);
        if (left > 0)
            memset(q, 0xff, left);

        s->write_packet(s, packet);

        buf_ptr += len1;
        len -= len1;
    }
}

static inline void put16(uint8_t **q_ptr, int val)
{
    uint8_t *q;
    q = *q_ptr;
    *q++ = val >> 8;
    *q++ = val;
    *q_ptr = q;
}

int mpegts_write_section1(MpegTSSection *s, int tid, int id, 
                          int version, int sec_num, int last_sec_num,
                          uint8_t *buf, int len)
{
    uint8_t section[1024], *q;
    unsigned int tot_len;
    
    tot_len = 3 + 5 + len + 4;
    /* check if not too big */
    if (tot_len > 1024)
        return -1;

    q = section;
    *q++ = tid;
    put16(&q, 0xb000 | (len + 5 + 4)); /* 5 byte header + 4 byte CRC */
    put16(&q, id);
    *q++ = 0xc1 | (version << 1); /* current_next_indicator = 1 */
    *q++ = sec_num;
    *q++ = last_sec_num;
    memcpy(q, buf, len);
    
    mpegts_write_section(s, section, tot_len);
    return 0;
}

/*********************************************/
/* mpegts writer */

#define DEFAULT_PMT_START_PID   0x1000 // 0x0040  //0x1000 MEANX
#define DEFAULT_START_PID       0x0031 // 0x100 MEANX
#define DEFAULT_AUDIO_START_PID 0x0034 // 0x100 MEANX
#define DEFAULT_PROVIDER_NAME   "FFmpeg"
#define DEFAULT_SERVICE_NAME    "Service01"

/* default network id, transport stream and service identifiers */
#define DEFAULT_ONID            0x0001
#define DEFAULT_TSID            0x0001
#define DEFAULT_SID             0x0001

/* a PES packet header is generated every DEFAULT_PES_HEADER_FREQ packets */
#define DEFAULT_PES_HEADER_FREQ 16
#define DEFAULT_PES_PAYLOAD_SIZE ((DEFAULT_PES_HEADER_FREQ - 1) * 184 + 170)

/* we retransmit the SI info at this rate */
#define SDT_RETRANS_TIME 500
#define PAT_RETRANS_TIME 100

typedef struct MpegTSWriteStream {
    int pid; /* stream associated pid */
    int cc;
    int payload_index;
    int64_t payload_pts;
    uint8_t payload[DEFAULT_PES_PAYLOAD_SIZE];
} MpegTSWriteStream;

typedef struct MpegTSService {
    MpegTSSection pmt; /* MPEG2 pmt table context */
    int pcr_pid;
    int sid;           /* service ID */
    char *name;
    char *provider_name;
} MpegTSService;

typedef struct MpegTSWrite {
    MpegTSSection pat; /* MPEG2 pat table */
    MpegTSSection sdt; /* MPEG2 sdt table context */
    MpegTSService **services;
    int sdt_packet_count;
    int sdt_packet_freq;
    int pat_packet_count;
    int pat_packet_freq;
    int nb_services;
    int onid;
    int tsid;
} MpegTSWrite;

static void mpegts_write_pat(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    MpegTSService *service;
    uint8_t data[1012], *q;
    int i;
    
    q = data;
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        put16(&q, service->sid);
        put16(&q, 0xe000 | service->pmt.pid);
    }
    mpegts_write_section1(&ts->pat, PAT_TID, ts->tsid, 0, 0, 0,
                          data, q - data);
}

static void mpegts_write_pmt(AVFormatContext *s, MpegTSService *service)
{
    //    MpegTSWrite *ts = s->priv_data;
    uint8_t data[1012], *q, *desc_length_ptr, *program_info_length_ptr;
    int val, stream_type, i;

    q = data;
    put16(&q, 0xe000 | service->pcr_pid);

    program_info_length_ptr = q;
    q += 2; /* patched after */

    /* put program info here */

    val = 0xf000 | (q - program_info_length_ptr - 2);
    program_info_length_ptr[0] = val >> 8;
    program_info_length_ptr[1] = val;
    
    for(i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        MpegTSWriteStream *ts_st = st->priv_data;
        switch(st->codec.codec_id) {
        case CODEC_ID_MPEG1VIDEO:
        case CODEC_ID_MPEG2VIDEO:
            stream_type = STREAM_TYPE_VIDEO_MPEG2;
            break;
        case CODEC_ID_MPEG4:
            stream_type = STREAM_TYPE_VIDEO_MPEG4;
            break;
        case CODEC_ID_H264:
            stream_type = STREAM_TYPE_VIDEO_H264;
            break;
        case CODEC_ID_MP2:
        case CODEC_ID_MP3:
            stream_type = STREAM_TYPE_AUDIO_MPEG1;
            break;
        case CODEC_ID_AAC:
            stream_type = STREAM_TYPE_AUDIO_AAC;
            break;
        case CODEC_ID_AC3:
            stream_type = STREAM_TYPE_AUDIO_AC3;
            break;
        default:
            stream_type = STREAM_TYPE_PRIVATE_DATA;
            break;
        }
        *q++ = stream_type;
        put16(&q, 0xe000 | ts_st->pid);
        desc_length_ptr = q;
        q += 2; /* patched after */

        /* write optional descriptors here */

        val = 0xf000 | (q - desc_length_ptr - 2);
        desc_length_ptr[0] = val >> 8;
        desc_length_ptr[1] = val;
    }
    mpegts_write_section1(&service->pmt, PMT_TID, service->sid, 0, 0, 0,
                          data, q - data);
}   

/* NOTE: str == NULL is accepted for an empty string */
static void putstr8(uint8_t **q_ptr, const char *str)
{
    uint8_t *q;
    int len;

    q = *q_ptr;
    if (!str)
        len = 0;
    else
        len = strlen(str);
    *q++ = len;
    memcpy(q, str, len);
    q += len;
    *q_ptr = q;
}

static void mpegts_write_sdt(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    MpegTSService *service;
    uint8_t data[1012], *q, *desc_list_len_ptr, *desc_len_ptr;
    int i, running_status, free_ca_mode, val;
    
    q = data;
    put16(&q, ts->onid);
    *q++ = 0xff;
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        put16(&q, service->sid);
        *q++ = 0xfc | 0x00; /* currently no EIT info */
        desc_list_len_ptr = q;
        q += 2;
        running_status = 4; /* running */
        free_ca_mode = 0;

        /* write only one descriptor for the service name and provider */
        *q++ = 0x48;
        desc_len_ptr = q;
        q++;
        *q++ = 0x01; /* digital television service */
        putstr8(&q, service->provider_name);
        putstr8(&q, service->name);
        desc_len_ptr[0] = q - desc_len_ptr - 1;

        /* fill descriptor length */
        val = (running_status << 13) | (free_ca_mode << 12) | 
            (q - desc_list_len_ptr - 2);
        desc_list_len_ptr[0] = val >> 8;
        desc_list_len_ptr[1] = val;
    }
    mpegts_write_section1(&ts->sdt, SDT_TID, ts->tsid, 0, 0, 0,
                          data, q - data);
}

static MpegTSService *mpegts_add_service(MpegTSWrite *ts, 
                                         int sid, 
                                         const char *provider_name, 
                                         const char *name)
{
    MpegTSService *service;

    service = av_mallocz(sizeof(MpegTSService));
    if (!service)
        return NULL;
    service->pmt.pid = DEFAULT_PMT_START_PID + ts->nb_services - 1;
    service->sid = sid;
    service->provider_name = av_strdup(provider_name);
    service->name = av_strdup(name);
    service->pcr_pid = 0x1fff;
    dynarray_add(&ts->services, &ts->nb_services, service);
    return service;
}

static void section_write_packet(MpegTSSection *s, const uint8_t *packet)
{
    AVFormatContext *ctx = s->opaque;
    put_buffer(&ctx->pb, packet, TS_PACKET_SIZE);
}

static int mpegts_write_header(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    MpegTSWriteStream *ts_st;
    MpegTSService *service;
    AVStream *st;
    int i, total_bit_rate;

    int vidPid=0,audPid=0;      //MEANX

    ts->tsid = DEFAULT_TSID;
    ts->onid = DEFAULT_ONID;
    /* allocate a single DVB service */
    service = mpegts_add_service(ts, DEFAULT_SID, 
                                 DEFAULT_PROVIDER_NAME, DEFAULT_SERVICE_NAME);
    service->pmt.write_packet = section_write_packet;
    service->pmt.opaque = s;

    ts->pat.pid = PAT_PID;
    ts->pat.cc = 0;
    ts->pat.write_packet = section_write_packet;
    ts->pat.opaque = s;

    ts->sdt.pid = SDT_PID;
    ts->sdt.cc = 0;
    ts->sdt.write_packet = section_write_packet;
    ts->sdt.opaque = s;

    /* assign pids to each stream */
    total_bit_rate = 0;
    for(i = 0;i < s->nb_streams; i++) {
        st = s->streams[i];
        ts_st = av_mallocz(sizeof(MpegTSWriteStream));
        if (!ts_st)
            goto fail;
        st->priv_data = ts_st;
        // MEANX
        //ts_st->pid = DEFAULT_START_PID + i;
        if(st->codec.codec_type==CODEC_TYPE_AUDIO) ts_st->pid=DEFAULT_AUDIO_START_PID+audPid++;
                                else ts_st->pid=DEFAULT_START_PID+vidPid++;
        // /MEANX
        ts_st->payload_pts = AV_NOPTS_VALUE;
        /* update PCR pid if needed */
        if (st->codec.codec_type == CODEC_TYPE_AUDIO && //MEANX lock PCR to audio
            service->pcr_pid == 0x1fff)
            service->pcr_pid = ts_st->pid;
        total_bit_rate += st->codec.bit_rate;
    }
    if (total_bit_rate <= 8 * 1024)
        total_bit_rate = 8 * 1024;
    ts->sdt_packet_freq = (total_bit_rate * SDT_RETRANS_TIME) / 
        (TS_PACKET_SIZE * 8 * 1000);
    ts->pat_packet_freq = (total_bit_rate * PAT_RETRANS_TIME) / 
        (TS_PACKET_SIZE * 8 * 1000);
#if 0
    printf("%d %d %d\n", 
           total_bit_rate, ts->sdt_packet_freq, ts->pat_packet_freq);
#endif

    /* write info at the start of the file, so that it will be fast to
       find them */
    mpegts_write_sdt(s);
    mpegts_write_pat(s);
    for(i = 0; i < ts->nb_services; i++) {
        mpegts_write_pmt(s, ts->services[i]);
    }
    put_flush_packet(&s->pb);

    return 0;

 fail:
    for(i = 0;i < s->nb_streams; i++) {
        st = s->streams[i];
        av_free(st->priv_data);
    }
    return -1;
}

/* send SDT, PAT and PMT tables regulary */
static void retransmit_si_info(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    int i;

    if (++ts->sdt_packet_count == ts->sdt_packet_freq) {
        ts->sdt_packet_count = 0;
        mpegts_write_sdt(s);
    }
    if (++ts->pat_packet_count == ts->pat_packet_freq) {
        ts->pat_packet_count = 0;
        mpegts_write_pat(s);
        for(i = 0; i < ts->nb_services; i++) {
            mpegts_write_pmt(s, ts->services[i]);
        }
    }
}

/* NOTE: pes_data contains all the PES packet */
static void mpegts_write_pes(AVFormatContext *s, AVStream *st,
                             const uint8_t *payload, int payload_size,
                             int64_t pts)
{
    MpegTSWriteStream *ts_st = st->priv_data;
    uint8_t buf[TS_PACKET_SIZE];
    uint8_t *q;
    int val, is_start, len, ts_len, header_len;

    is_start = 1;
    while (payload_size > 0) {
        retransmit_si_info(s);

        /* prepare packet header */
        q = buf;
        *q++ = 0x47;
        val = (ts_st->pid >> 8);
        if (is_start)
            val |= 0x40;
        *q++ = val;
        *q++ = ts_st->pid;
        *q++ = 0x10 | ts_st->cc;
        ts_st->cc = (ts_st->cc + 1) & 0xf;
        if (is_start) {
            /* write PES header */
            *q++ = 0x00;
            *q++ = 0x00;
            *q++ = 0x01;
            if (st->codec.codec_type == CODEC_TYPE_VIDEO)
                *q++ = 0xe0;
            else
                *q++ = 0xc0;
            if (pts != AV_NOPTS_VALUE)
                header_len = 8;
            else
                header_len = 3;
            len = payload_size + header_len;
            *q++ = len >> 8;
            *q++ = len;
            *q++ = 0x80;
            if (pts != AV_NOPTS_VALUE) {
                *q++ = 0x80; /* PTS only */
                *q++ = 0x05; /* header len */
                val = (0x02 << 4) | 
                    (((pts >> 30) & 0x07) << 1) | 1;
                *q++ = val;
                val = (((pts >> 15) & 0x7fff) << 1) | 1;
                *q++ = val >> 8;
                *q++ = val;
                val = (((pts) & 0x7fff) << 1) | 1;
                *q++ = val >> 8;
                *q++ = val;
            } else {
                *q++ = 0x00;
                *q++ = 0x00;
            }
            is_start = 0;
        }
        /* write header */
        ts_len = q - buf;
        put_buffer(&s->pb, buf, ts_len);
        /* write data */
        len = TS_PACKET_SIZE - ts_len;
        if (len > payload_size)
            len = payload_size;
        put_buffer(&s->pb, payload, len);
        payload += len;
        payload_size -= len;
        ts_len += len;
        /* stuffing */
        len = TS_PACKET_SIZE - ts_len;
        if (len > 0) {
            memset(buf, 0xff, len);
            put_buffer(&s->pb, buf, len);
        }
    }
    put_flush_packet(&s->pb);
}

static int mpegts_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVStream *st = s->streams[pkt->stream_index];
    int size= pkt->size;
    uint8_t *buf= pkt->data;
    MpegTSWriteStream *ts_st = st->priv_data;
    int len;

    while (size > 0) {
        len = DEFAULT_PES_PAYLOAD_SIZE - ts_st->payload_index;
        if (len > size)
            len = size;
        memcpy(ts_st->payload + ts_st->payload_index, buf, len);
        buf += len;
        size -= len;
        ts_st->payload_index += len;
        if (ts_st->payload_pts == AV_NOPTS_VALUE)
            ts_st->payload_pts = pkt->pts;
        if (ts_st->payload_index >= DEFAULT_PES_PAYLOAD_SIZE) {
            mpegts_write_pes(s, st, ts_st->payload, ts_st->payload_index,
                             ts_st->payload_pts);
            ts_st->payload_pts = AV_NOPTS_VALUE;
            ts_st->payload_index = 0;
        }
    }
    return 0;
}

static int mpegts_write_end(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    MpegTSWriteStream *ts_st;
    MpegTSService *service;
    AVStream *st;
    int i;

    /* flush current packets */
    for(i = 0; i < s->nb_streams; i++) {
        st = s->streams[i];
        ts_st = st->priv_data;
        if (ts_st->payload_index > 0) {
            mpegts_write_pes(s, st, ts_st->payload, ts_st->payload_index,
                             ts_st->payload_pts);
        }
    }
    put_flush_packet(&s->pb);
        
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        av_freep(&service->provider_name);
        av_freep(&service->name);
        av_free(service);
    }
    av_free(ts->services);

    return 0;
}

AVOutputFormat mpegts_mux = {
    "mpegts",
    "MPEG2 transport stream format",
    "video/x-mpegts",
    "ts",
    sizeof(MpegTSWrite),
    CODEC_ID_MP2,
    CODEC_ID_MPEG2VIDEO,
    mpegts_write_header,
    mpegts_write_packet,
    mpegts_write_end,
};
