/*
 *  mux_out.c
 *
 *  Copyright (C) Gerhard Monzel - October 2003
 *
 *  This file is part of lve, a linux mpeg video editor
 *      
 *  lve tools are free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  lve is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */
#define  _MUX_OUT 1
#include <stdio.h>
#include <math.h>
#include "mux_out.h"

//=====================
//== LOCAL Functions ==
//=====================

//== bit buffering stuff ==
//=========================

#if 1 

#define HW_HB 3  // bigendian sequence for uint32_t
#define HW_LB 2
#define LW_HB 1
#define LW_LB 0

static void put_bits(BitPack *bp, int n, uint32_t value)
{
  uint8_t *pv   = (uint8_t *)&value;
  uint8_t *dst  = &bp->buf[bp->byte_ofs];
  int     bytes, shift;
    
  n--;
  bytes = n/8;
  shift = bp->bits_left - ((n%8)+1);
  
  //-- case 1: bits will be fit exactly into left space --
  //------------------------------------------------------ 
  if (shift == 0)
  {
    bp->byte_ofs += (bytes+1);
    bp->bits_left = 8;
  }
  //-- case 2: not enough new bits to fill left --
  //----------------------------------------------
  else if (shift>0)
  {
    bp->byte_ofs += bytes;
    bp->bits_left = shift;
    value <<= shift;
  }
  //-- case 3: not enough left to hold new bits --
  //----------------------------------------------
  else 
  {
    bytes++; 
    bp->byte_ofs += bytes;
    bp->bits_left = (8+shift);
    
    dst[bytes] = (uint8_t)(value << bp->bits_left);
    bytes--;
    
    value >>= (-shift);
  }

  switch (bytes)
  {
    case 3:
      dst[0] |= pv[HW_HB]; 
      dst[1]  = pv[HW_LB];
      dst[2]  = pv[LW_HB];
      dst[3]  = pv[LW_LB];
      break;
      
    case 2:
      dst[0] |= pv[HW_LB]; 
      dst[1]  = pv[LW_HB];
      dst[2]  = pv[LW_LB];
      break;
    
    case 1:
      dst[0] |= pv[LW_HB]; 
      dst[1]  = pv[LW_LB];
      break;
      
    case 0:
      dst[0] |= pv[LW_LB];
      break;
  }

  /*
  if (put_debug) 
    fprintf(stderr, "*** %d: %02x %02x %02x %02x %02x %02x %02x (%d)\n",
            n, bp->buf[0], bp->buf[1], bp->buf[2], bp->buf[3],
            bp->buf[4], bp->buf[5], bp->buf[6], bp->byte_ofs); 
  */
}

#else

static void put_bits(BitPack *bp, int n, uint32_t value)
{
  uint8_t *dst  = &bp->buf[bp->byte_ofs];
  int     bytes = n/8;
  int     shift = bp->bits_left - (n%8);
  
  //-- one byte more ? -- 
  if (shift < 0)
  {
    bp->bits_left = 8+shift;
    bp->byte_ofs++;
    
    dst[bytes+1] = ((uint8_t)value << (bp->bits_left));
    value >>= (-shift);
  }
  else switch (shift)
  {  
    //-- one full byte --
    case 0:
        bp->bits_left = 8;
        bp->byte_ofs++;
        break;
        
    case 8: 
        //-- count it here --
        bp->bits_left = 8;
        bp->byte_ofs++;
        bytes--;
        break;
          
    default: 
        value <<= shift;
        bp->bits_left = shift;
        break;
  }

  switch (bytes)
  {
    case 3:
      dst[3] |= (uint8_t)value;
      value >>= 8;
      
    case 2:
      dst[2] |= (uint8_t)value;
      value >>= 8;
      
    case 1:
      dst[1] |= (uint8_t)value;
      value >>= 8;
      
    case 0:
      dst[0] |= (uint8_t)value;
      break;
  }
  
  bp->byte_ofs += bytes;
}
#endif

static void put_byte(BitPack *bp, uint8_t value)
{
  if (bp->bits_left<8) fprintf(stderr, "ERR: bp not aligned (%d)\n", bp->bits_left);
  
  bp->buf[bp->byte_ofs++] = value;
}


static void put_word(BitPack *bp, uint16_t value)
{
  if (bp->bits_left<8) fprintf(stderr, "ERR: bp not aligned (%d)\n", bp->bits_left);
  
  bp->buf[bp->byte_ofs++] = (uint8_t)(value>>8); 
  bp->buf[bp->byte_ofs++] = (uint8_t)value;
}


static void put_qword(BitPack *bp, uint32_t value)
{
  if (bp->bits_left<8) fprintf(stderr, "ERR: bp not aligned (%d)\n", bp->bits_left);
  
  bp->buf[bp->byte_ofs+3] = (uint8_t)value;
  value >>= 8;

  bp->buf[bp->byte_ofs+2] = (uint8_t)value;
  value >>= 8;

  bp->buf[bp->byte_ofs+1] = (uint8_t)value;
  value >>= 8;

  bp->buf[bp->byte_ofs] = (uint8_t)value;

  bp->byte_ofs += 4;
}


static void put_mem(BitPack *bp, uint8_t *src, int n)
{
  if (bp->bits_left<8) fprintf(stderr, "ERR: bp not aligned (%d)\n", bp->bits_left);
  
  memcpy((void *)&bp->buf[bp->byte_ofs], src, n);
  bp->byte_ofs += n;
}

static void put_skip(BitPack *bp, int n)
{
  if (bp->bits_left<8) fprintf(stderr, "ERR: bp not aligned (%d)\n", bp->bits_left);
  
  bp->byte_ofs += n;
}

static void put_init_bp(BitPack *bp, uint8_t *buf)
{
  bp->buf       = buf;
  bp->byte_ofs  = 0;
  bp->bits_left = 8;
}

//== BitPack stuff ==
//===================
static int put_bp_header(PackStream *ps)
{
  BitPack *bp       = &ps->bit_pack;
  int byte_ofs_old  = bp->byte_ofs;  
  int i;
    
  put_qword(bp, MX_PCK_START_CODE);
     
  //-- set SCR (System Clock Reference) here to 0,  --
  //-- update will be done while flushing the unit. --
  for (i=0; i<6; i++) put_byte(bp, 0x00);
  
  put_bits(bp, 22, ps->mux_rate);
  put_bits(bp, 1, 1);    /* marker */
  put_bits(bp, 1, 1);    /* marker */
  put_bits(bp, 5, 0x1F); /* reserved */
  put_bits(bp, 3, 0);    /* stuffing length */

  return (bp->byte_ofs - byte_ofs_old);
}

static int put_bp_private_stream2(PackStream *ps, int len)
{
  BitPack *bp = &ps->bit_pack;
  
  put_qword(bp, MX_PRIVATE_STREAM_2); 
  put_word(bp, len);
  put_skip(bp, len);
  
  return len + 6;
}

static int put_bp_system_header(PackStream *ps, int is_vobu)
{
  BitPack *bp          = &ps->bit_pack;
  int     byte_ofs_old = bp->byte_ofs;
  int     size;

  put_qword(bp, MX_SYS_START_CODE);
  put_word(bp, 16);  // packet size will be updated later              
  put_bits(bp, 1, 1);
    
  put_bits(bp, 22, ps->mux_rate);
  put_bits(bp, 1, 1); /* marker */
  put_bits(bp, 6, ps->audio_bound);

  put_bits(bp, 1, 0); /* not fixed bitrate */
  put_bits(bp, 1, 1); /* non constrainted bit stream */
    
  put_bits(bp, 1, 0); /* audio locked */
  put_bits(bp, 1, 0); /* video locked */
  put_bits(bp, 1, 1); /* marker */

  put_bits(bp, 5, ps->video_bound);
  put_bits(bp, 8, 0xFF); /* reserved byte */

  //-- special VOBU pack or ... --
  //------------------------------
  if (is_vobu)
  {
    put_bits(bp, 8, 0xB9); 
    put_bits(bp, 2, 3);
    put_bits(bp, 1, 1);
    put_bits(bp, 13, 232);
    
    put_bits(bp, 8, 0xB8); 
    put_bits(bp, 2, 3);
    put_bits(bp, 1, 0);
    put_bits(bp, 13, 32);

    put_bits(bp, 8, 0xBD); 
    put_bits(bp, 2, 3);
    put_bits(bp, 1, 1);
    put_bits(bp, 13, 58);

    put_bits(bp, 8, 0xBF); 
    put_bits(bp, 2, 3);
    put_bits(bp, 1, 1);
    put_bits(bp, 13, 2);
  }  
  //-- ... standard system pack --
  //------------------------------
  else
  {
    //-- VIDEO or ... --
    if (ps->pkt_id == VIDEO_ID )
    {
      put_bits(bp, 8, ps->pkt_id); /* stream ID */
      put_bits(bp, 2, 3);
     
      put_bits(bp, 1, 1);
      put_bits(bp, 13, ps->video_max_buf_size / 1024);
    }
    //-- ... AUDIO --
    else
    {  
      uint8_t id;
      
      if (ps->audio_id < AUDIO_ID_MP2)
        id = 0xBD;
      else
        id = ps->audio_id;
    
      put_bits(bp, 8, id); /* stream ID */
      put_bits(bp, 2, 3);
     
      put_bits(bp, 1, 0);
      put_bits(bp, 13, ps->audio_max_buf_size / 128);
    }
  }

  size = bp->byte_ofs - byte_ofs_old;
  
  /* patch packet size */
  bp->buf[byte_ofs_old+4] = (size - 6) >> 8;
  bp->buf[byte_ofs_old+5] = (size - 6) & 0xff;
    
  return size;
}

//== pack builders ==
//===================
static int mux_put_vobu_pack(PackStream *ps)
{
  int size;

  memset(ps->pack_start, 0, ps->pack_size);
  put_init_bp(&ps->bit_pack, ps->pack_start);
  
  size  = put_bp_header(ps);
  size += put_bp_system_header(ps, 1);
  size += put_bp_private_stream2(ps, 980);
  size += put_bp_private_stream2(ps, 1018);
  
  ps->pack_cnt++;
  ps->pack_start += size;
  ps->buf_level  += size;
  
  //-- TODO --
  if (size != ps->pack_size) 
    fprintf(stderr, "\nERR: vobu pack size (%d) mismatch\n", size);
  
  return size;
}

static int mux_find_a52sw(uint8_t *pbuf, int len)
{
  uint16_t s = pbuf[0];
  int      i;
  
  len--;
  for (i=1; i<len; i++)
  {
    s = (s<<8) | pbuf[i];
    if (s == 0x0B77) return i;
  }  
  return 0;
}

static int mux_put_av_pack(PackStream *ps)
{
  BitPack *bp = &ps->bit_pack;
   
  uint32_t startcode;
  int      i, header_len, header_data_len, sub_hdr_len;
  int      size, padding_size, available_size, payload_size;

  uint8_t  pts_dts_flag = 0x00;
  uint8_t  pts_intro    = 0x00;
  double   dts          = -1.0;

  memset(ps->pack_start, 0, ps->pack_size);
  put_init_bp(bp, ps->pack_start); 

  //-- put pack header --
  size = put_bp_header(ps);
    
  header_len = 3;  // mpeg2 id (1), flags (1), header data len (1)
  
  //-- header data may contains at maximum:   --
  //-- PTS (5) + DTS (5) + PTS-ext (3) = (13) --
  //-- if timestamp(s) requested --
  if ( ps->pts != -1.0 )
  {
    //-- video data (PTS + DTS) --
    if ( ps->pkt_id == VIDEO_ID )
    {
      header_data_len = 10;
      pts_dts_flag    = 0xC0;
      pts_intro       = 0x03;
      dts             = ps->pts - ps->pict_duration;   
    }
    //-- audio data (PTS only) --
    else
    {
      header_data_len = 5;
      pts_intro    = 0x02;
      pts_dts_flag = 0x80;
    }  
  }
  else
  {
    header_data_len = 0;
  }
  
  //-- calculate uncorrected payload size ... --
  payload_size = ps->pack_size - (size + 6 + header_len + header_data_len);
    
  //-- ... but possibly some corrections --   
  if (ps->pkt_id < AUDIO_ID_MP2)
  {
    startcode     = MX_PRIVATE_STREAM_1;
    sub_hdr_len   = 4;
    payload_size -= sub_hdr_len;
  }
  else 
  {
    startcode   = 0x100 + ps->pkt_id;
    sub_hdr_len = 0;
  }  
  
  if (ps->pkt_len < payload_size)
  {
    available_size = ps->pkt_len;
    padding_size   = payload_size - available_size;
      
    if ( (padding_size < 7) && (padding_size > 0) ) 
    {
      header_data_len += padding_size;
      padding_size     = 0;
    }
    else
      padding_size -= 6;
  }
  else
  {
    available_size = payload_size;  
    padding_size   = 0;
  }  
      
  //-- PES startcode (4) and PES packet len (2) --
  put_qword(bp, startcode); 
  put_word(bp, available_size + header_len + header_data_len + sub_hdr_len);
    
  //-- mpeg2 needs 3 byte header -- 
  put_byte(bp, 0x80);            /* mpeg2 id */	
  put_byte(bp, pts_dts_flag);    /* flags    */   
  put_byte(bp, header_data_len); /* header data len */
    
  //-- PTS timestamp on (caller) demand --
  if (ps->pts != -1.0)
  {
    int64_t timestamp = (int64_t)(ps->pts * 90);
      
    //-- packet header data --
    put_byte(bp, 
             (pts_intro << 4) | 
             (((timestamp >> 30) & 0x07) << 1) | 
             1);
    put_word(bp, (uint16_t)((((timestamp >> 15) & 0x7fff) << 1) | 1));
    put_word(bp, (uint16_t)((((timestamp) & 0x7fff) << 1) | 1));
      
    header_data_len -= 5;
  }
  
  //-- DTS timestamp on (caller) demand --
  if (dts != -1.0)
  {
    int64_t timestamp = (int64_t)(dts * 90);
      
      
    //-- packet header data --
    put_byte(bp, 
             (0x01 << 4) | 
             (((timestamp >> 30) & 0x07) << 1) | 
             1);
    put_word(bp, (uint16_t)((((timestamp >> 15) & 0x7fff) << 1) | 1));
    put_word(bp, (uint16_t)((((timestamp) & 0x7fff) << 1) | 1));
      
    header_data_len -= 5;
  }
  
  ps->pts = -1.0;  // caller must set pts again to activate timestamping
    
  //-- stuffing rest of header data len --
  for(i=0;i<header_data_len;i++) put_byte(bp, 0xff);

  if (startcode == MX_PRIVATE_STREAM_1)
  {
    uint16_t ofs_sw = mux_find_a52sw(ps->pkt_buf, available_size);
    uint8_t  nb_sw  = (uint8_t)((available_size - ofs_sw)/ps->audio_encoded_fs); 
    
    if ( ((available_size-ofs_sw) % ps->audio_encoded_fs) > 0 ) nb_sw += 1;
  
    //-- a AC3 specific header contains:                --
    //-- audio ID                                   (8) --
    //-- number of sync words contained in payload  (8) --
    //-- offset to the first sync word in payload  (16) --
    put_byte(bp, ps->pkt_id);
    put_byte(bp, nb_sw); 
    put_word(bp, ofs_sw);
  }

  //-- put available data as payload --
  put_mem(bp, ps->pkt_buf, available_size);

  //-- padding rest of payload if needed -- 
  if ( padding_size > 0 )
  {    
    put_qword(bp, MX_PADDING_STREAM); 			
    put_word(bp, padding_size);
    for(i=0; i<padding_size;i++) put_byte(bp, 0xff);  
  }

  size = bp->byte_ofs;
  
  //-- TODO --
  if (size != ps->pack_size) 
    fprintf(stderr, "\nERR: pack size (%d) mismatch\n", size);
  
  ps->pack_cnt++;
  ps->pack_start += size;
  ps->buf_level  += size;
  
  return available_size; // return really used payload
}

//== flush unit: update SCRs of packs and write unit to file  ==
//==============================================================
static int mux_flush_packs(PackStream *ps)
{
  BitPack *bp      = &ps->bit_pack;
  uint8_t *start   = ps->stream_buf + 4;
  uint8_t *end     = ps->stream_buf + ps->buf_level;
  double  scr_step = ps->pack_size * ps->pict_duration / ps->buf_level;
  int64_t timestamp;
  int n;
  
  while (start < end)
  {
    //-- transfer SCR to timestamp -- 
    timestamp = (int64_t)(ps->scr * 27000 / 300.0 + 0.5);
     
    //-- update SCR (System Clock Reference) = total 48bits = 6 bytes --
    put_init_bp(bp, start);
 
    put_bits(bp,  2, 0x1);
    put_bits(bp,  3, (uint32_t)((timestamp >> 30) & 0x07));
    put_bits(bp,  1, 1);
    put_bits(bp, 15, (uint32_t)((timestamp >> 15) & 0x7fff));
    put_bits(bp,  1, 1);
    put_bits(bp, 15, (uint32_t)((timestamp) & 0x7fff));
    put_bits(bp,  1, 1);
    put_bits(bp,  9, 0);    // set remainder of SCR (because of simplicity to 0) 
    put_bits(bp,  1, 1);    // marker 

    //-- next SCR in next pack --
    ps->scr += scr_step;
    start   += ps->pack_size;
  }
  
  n = fwrite(ps->stream_buf, 1, ps->buf_level, ps->fp);

  //-- start in front of buffer again --  
  ps->pack_start = ps->stream_buf;
  ps->buf_level  = 0;

  ps->frame_no++;
  
  return 0;
}

//======================
//== PUBLIC Functions ==
//======================
PackStream *mux_open(char *fn, 
                     int v_bit_rate, double frame_rate, 
                     int a_bit_rate, int sample_rate, uint8_t audio_id)
{
  static PackStream _ps = {VOB_PACK_LEN, 40.0, V_PTS_DFLT, A_PTS_DFLT, NULL};
  
  uint32_t muxrate = v_bit_rate + a_bit_rate + 2000;   
  PackStream   *ps = &_ps;
  
  if (!v_bit_rate) muxrate = 10080000; // fixed defined for DVD
  
  if (!strcmp(fn,"-"))
  {
    ps->fp = fdopen(1, "wb");
    ps->is_stdout = 1;
  }
  else
  {
    ps->fp = fopen(fn, "w+b");
    ps->is_stdout = 0;
  }  
    
  if ( ps->fp )
  {
    ps->pack_start    = ps->stream_buf;
    ps->buf_level     = 0;
    ps->pack_cnt      = 0;
    
    ps->frame_no      = 0;
    ps->gop_cnt       = 0;
    ps->pict_duration = 1000.0 / frame_rate;

    ps->mux_rate = (muxrate + (8*50)-1) /(8*50);
    ps->pts      = -1.0;
    ps->scr      =  0.0;   
    
    ps->video_id    = VIDEO_ID;
    ps->video_bound = 1;
    ps->video_max_buf_size = 46 * 1024; 
    ps->video_pts   = 0; 
    
    ps->audio_id    = audio_id;
    ps->audio_bound = 1;
    ps->audio_delay = 0; //(audio_id == 0x80)? 200:0;
    ps->audio_max_buf_size = 4 * 1024;
    ps->audio_pts   = 0;
    // MEANX
    // We set clean default value...
    if(audio_id==AUDIO_ID_AC3)
    	ps->audio_encoded_fs=(int)ceil((AC3_FRAME_SIZE *a_bit_rate)/(8*sample_rate));
    else
    	ps->audio_encoded_fs=(int)ceil(( MP2_FRAME_SIZE*a_bit_rate)/(8*sample_rate));
	
	printf("Audio encoded fs: %d\n",ps->audio_encoded_fs);	
     ps->a_pts_ofs = A_PTS_MIN;
     ps->v_pts_ofs = A_PTS_MIN;
	
  
#if 0    
    
      ap.size_pf = (int)( (ap.samples_pf * ap.br) / (8.0 * ap.sr) + 0.5);
    fprintf(stderr, "\naudio stream type (%s) detected \n", (ap.id==AUDIO_ID_MP2)? "MP2":"AC3");
    fprintf(stderr, "- sample rate (%d Hz)\n- bitrate (%d bps)\n- frame size (%d bytes)\n",
            ap.sr, ap.br, ap.size_pf);
#endif	    
	// MEANX
    put_init_bp(&ps->bit_pack, ps->stream_buf); // equal to pack_start now
    
    return ps;   
  }
 
  return NULL; 
}

void mux_close(PackStream *ps)
{
  if (ps->fp) 
  {
    if (!ps->is_stdout) fclose(ps->fp);
  }
}


int mux_write_packet(PackStream *ps, 
                     uint8_t pkt_id, uint8_t *pkt_buf, int pkt_len) 
{

    static uint8_t seq_start_code [] = {0x00, 0x00, 0x01, MX_SEQ_CODE};
    static uint8_t gop_start_code [] = {0x00, 0x00, 0x01, MX_GOP_CODE};
    
    int payload_size; 
     
    ps->pkt_id  = pkt_id;    // 0xC0=MP2-Audio, 0x80=AC3 Audio, 0xE0=Video 
    ps->pkt_buf = pkt_buf;
    ps->pkt_len = pkt_len; 

    //-- different handling for video and audio data -- 
    //-------------------------------------------------
    if (pkt_id == VIDEO_ID) 
    {
      if (pkt_len >= 4)
      {
        if ( !memcmp(pkt_buf, seq_start_code, 4) || !memcmp(pkt_buf, gop_start_code, 4) )
        {
          //-- every 2nd. sequence/GOP insert a VOBU packet --
          if (ps->gop_cnt==0)
          {
            mux_put_vobu_pack(ps);
            ps->gop_cnt = 1;
          }
          else
            ps->gop_cnt--;
        
          //-- every sequence/GOP packet will by PTS timestamped --
          ps->pts = ps->frame_no * ps->pict_duration + ps->v_pts_ofs; // with videoffset
          //ps->audio_pts = ps->pts;
        }
      }
    }
    else if (pkt_len)
    {
      double pts_step;
          
      ps->audio_blocks = pkt_len/(double)ps->audio_encoded_fs;
      
      //fprintf(stderr, "*** (%1.4f/%d) ***\n", ps->audio_blocks, ps->audio_encoded_fs);

      if (ps->audio_id == AUDIO_ID_MP2) 
        pts_step = ps->audio_blocks * 24;
      else
        pts_step = ps->audio_blocks * 32;

      //-- every packet containing 1st. correspondend audio data -- 
      //-- of the last frame (fields) will be PTS timestamped.   --
      ps->pts = ps->audio_pts + ps->a_pts_ofs + ps->audio_delay;
      ps->audio_pts += pts_step; 
      
    }
    else
    {
      ps->audio_pts += ps->pict_duration;
    }
    
    while (ps->pkt_len > 0)
    {
      //-- flush stream data as packets (with padding last --
      //-- packet to full packet size, if necessarily.)    --
      payload_size = mux_put_av_pack(ps);
      
      ps->pkt_buf += payload_size;
      ps->pkt_len -= payload_size;
    }  

    //-- after audio data -> update all pack SCRs and write unit -- 
    if (ps->pkt_id < VIDEO_ID) return mux_flush_packs(ps);
    
    //printf("A-V :%d\n",ps->audio_pts-ps->pts);

    return 0;
}

#if 0
#define TEST_VBUF_LEN (27*1204)
#define TEST_ABUF_LEN (TEST_VBUF_LEN/4)

int main()
{
  uint8_t    buffer[TEST_VBUF_LEN];
  PackStream *ps;
  int frame_no = 0;
   
  memset(buffer, 0, TEST_VBUF_LEN); 
   
  if ( (ps = mux_open("test.vob", 10000000, 25.0, 224000, 48000, 0xC0)) != NULL )
  {
    for (frame_no =0; frame_no<1; frame_no++)
    {
      mux_write_packet(ps, 0xE0, buffer, TEST_VBUF_LEN);
      mux_write_packet(ps, 0xC0, buffer, TEST_ABUF_LEN);
    }  
    mux_close(ps);
  }
  
  return 0;
}
#endif 

//eof
