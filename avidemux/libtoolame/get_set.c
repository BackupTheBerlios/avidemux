/*
 *  tooLAME: an optimized mpeg 1/2 layer 2 audio encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "common.h"
#include "toolame.h"
#include "toolame_global_flags.h"
#include "mem.h"




/*********************************************/

int toolame_set_mode (toolame_options *glopts, MPEG_mode mode)
{
  if (mode<0 || mode>3)
{
    fprintf(stderr,"invalid mode %i\n",mode);
    return(-1);
  }
  glopts->mode = mode;
  return(0);
}

MPEG_mode toolame_get_mode (toolame_options *glopts)
{
	return(glopts->mode);
}

const char *toolame_get_mode_name(toolame_options *glopts)
{
  static const char *mode_name[5] = { "Stereo", "J-Stereo", "Dual-Channel", "Mono", "Illegal Mode"};
  int mode = glopts->mode;
  if (mode>=0 && mode<4)
    return (mode_name[mode]);
  else 
    return (mode_name[4]);
}


int toolame_set_psymodel (toolame_options *glopts, int psymodel)
{
  if (psymodel<-1 || psymodel>4)
    return(-1);
  glopts->psymodel = psymodel;
  return(0);
}

int toolame_get_psymodel (toolame_options *glopts)
{
  return(glopts->psymodel);
}


/* number of channels on the input stream */
int toolame_set_num_channels(toolame_options* glopts, int num_channels)
{
    glopts->num_channels = num_channels;
    return 0;
}

int toolame_get_num_channels(toolame_options* glopts)
{
	return(glopts->num_channels);
}


int toolame_set_in_samplerate (toolame_options *glopts, int samplerate)
{  
  glopts->samplerate_in = samplerate;
  return(0);
}

int toolame_get_in_samplerate (toolame_options *glopts)
{
  return(glopts->samplerate_in);
}

int toolame_set_out_samplerate (toolame_options *glopts, int samplerate)
{  
  glopts->samplerate_out = samplerate;
  return(0);
}

int toolame_get_out_samplerate (toolame_options *glopts)
{
  return(glopts->samplerate_out);
}

int toolame_set_brate (toolame_options *glopts, int bitrate)
{
  glopts->bitrate = bitrate;
  return(0);
}

int toolame_get_brate (toolame_options *glopts)
{
  return(glopts->bitrate);
}


/* Because the LAME API isn't always the best way ;) */
int toolame_set_bitrate (toolame_options *glopts, int bitrate)
{
  glopts->bitrate = bitrate;
  return(0);
}

int toolame_get_bitrate (toolame_options *glopts)
{
  return(glopts->bitrate);
}


int toolame_set_emphasis (toolame_options *glopts, Emphasis_type emphasis)
{
  //MFC Fix: add #defines for the emphasis modes
  if (emphasis!=0 && emphasis!=1 && emphasis!=3) 
    return(-1);
  glopts->emphasis = emphasis;
  return(0);
}

Emphasis_type toolame_get_emphasis (toolame_options *glopts)
{
  return(glopts->emphasis);
}

int toolame_set_error_protection (toolame_options *glopts, int error_protection)
{
  if (error_protection)
    glopts->error_protection = TRUE;
  else
    error_protection = FALSE;
  return(0);
}

int toolame_get_error_protection (toolame_options *glopts)
{
  return(glopts->error_protection);
}

int toolame_set_copyright (toolame_options *glopts, int copyright)
{
  if (copyright)
    glopts->copyright = TRUE;
  else 
    glopts->copyright = FALSE;
  return(0);
}
int toolame_get_copyright (toolame_options *glopts)
{
  return(glopts->copyright);
}

int toolame_set_original (toolame_options *glopts, int original)
{
  if (original)
    glopts->original = TRUE;
  else
    glopts->original = FALSE;
  return(0);
}
int toolame_get_original (toolame_options *glopts)
{
  return(glopts->original);
}

int toolame_set_padding (toolame_options *glopts, Padding_type padding)
{
  if (padding)
    glopts->padding = TRUE;
  else
    glopts->padding = FALSE;
      
  return(0);
}

Padding_type toolame_get_padding (toolame_options *glopts)
{
  return(glopts->padding);
}

int toolame_set_VBR (toolame_options *glopts, int vbr)
{
  if (vbr)
    glopts->vbr = TRUE;
  else
    glopts->vbr = FALSE;
  return(0);
}

int toolame_get_VBR (toolame_options *glopts)
{
  return(glopts->vbr);
}

/* VBR quality level.  - *** change to int: 0=highest  9=lowest  */
int toolame_set_VBR_q (toolame_options *glopts, FLOAT level)
{
  if (fabs(level)>50.0)
    return(-1);
  else
    glopts->vbrlevel = level;
  return(0);
}

FLOAT toolame_get_VBR_q (toolame_options *glopts)
{
  return(glopts->vbrlevel);
}

int toolame_set_ATH_level (toolame_options *glopts, FLOAT level)
{
  glopts->athlevel = level;
  return(0);
}

FLOAT toolame_get_ATH_level (toolame_options *glopts)
{
  return(glopts->athlevel);
}

int toolame_set_quick_mode (toolame_options *glopts, int quickmode)
{
  if (quickmode)
    glopts->quickmode = TRUE;
  else
    glopts->quickmode = FALSE;
  return(0);
}
int toolame_get_quick_mode (toolame_options *glopts)
{
  return(glopts->quickmode);
}

int toolame_set_quick_count (toolame_options *glopts, int quickcount )
{
  glopts->quickcount = quickcount;
  return(0);
}
int toolame_get_quick_count (toolame_options *glopts)
{
  return(glopts->quickcount);
}


int toolame_set_verbosity (toolame_options *glopts, int verbosity)
{
  glopts->verbosity = verbosity;
  return(0);
}
int toolame_get_verbosity (toolame_options *glopts)
{
  return(glopts->verbosity);
}

int toolame_set_VBR_max_bitrate_kbps (toolame_options *glopts, int bitrate)
{
	glopts->vbr_max_bitrate = bitrate;
	return(0);
}

int toolame_get_VBR_max_bitrate_kbps (toolame_options *glopts)
{
  return(glopts->vbr_max_bitrate);
}

int toolame_set_num_ancillary_bits (toolame_options *glopts, int num)
{
  if (num<0)
    return(-1);
  glopts->num_ancillary_bits = num;
  return(0);
}
int toolame_get_num_ancillary_bits (toolame_options *glopts)
{
  return(glopts->num_ancillary_bits);
}

int toolame_set_energy_levels (toolame_options *glopts, int energylevels )
{
  if (energylevels)
    glopts->do_energy_levels = TRUE;    
  else
    glopts->do_energy_levels = FALSE;
  return(0);
}
int toolame_get_energy_levels (toolame_options *glopts)
{
  return(glopts->do_energy_levels);
}

int toolame_set_version (toolame_options *glopts, MPEG_version version)
{
  if (version!=0 && version!=1)
    return(-1);
  glopts->version = version;
  return(0);
}

MPEG_version toolame_get_version (toolame_options *glopts)
{
  return(glopts->version);
}

const char *toolame_get_version_name(toolame_options *glopts)
{
  static const char *version_name[3] = { "MPEG-2 LSF", "MPEG-1", "Illegal Version" };
  int version = glopts->version;
  if (version==0)
    return(version_name[0]);
  if (version==1)
    return(version_name[1]);
  return(version_name[2]);
}


int toolame_set_DAB (toolame_options *glopts, int dab)
{
  if (dab)
    glopts->do_dab = TRUE;
  else
    glopts->do_dab = FALSE;
  return(0);
}

int toolame_get_DAB (toolame_options *glopts)
{
  return(glopts->do_dab);
}

int toolame_set_DAB_xpad_length (toolame_options *glopts, int length)
{
  if (length<0)
    return(-1);
  glopts->dab_xpad_len = length;
  return(0);
}

int toolame_get_DAB_xpad_length (toolame_options *glopts)
{
  return(glopts->dab_xpad_len);
}

int toolame_set_DAB_crc_length (toolame_options *glopts, int length)
{
  if (length<0)
    return(-1);
  else
    glopts->dab_crc_len = length;
  return(0);
}

int toolame_get_DAB_crc_length (toolame_options *glopts)
{
  return(glopts->dab_crc_len);
}

/************ generic

int toolame_set (toolame_options *glopts, )
{

  return(0);
}
int toolame_get (toolame_options *glopts)
{
  return(glopts->);
}



*************/

