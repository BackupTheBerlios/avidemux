
/***************************************************************************
                          ADM_vidDecTelecide  -  description
                             -------------------
    
    email                : fixounet@free.fr

    Port of Donal Graft Telecide which is (c) Donald Graft
    http://www.neuron2.net
    http://puschpull.org/avisynth/decomb_reference_manual.html

 ***************************************************************************/
/*
	Telecide plugin for Avisynth -- recovers original progressive
	frames from telecined streams. The filter operates by matching
	fields and automatically adapts to phase/pattern changes.

	Copyright (C) 2003 Donald A. Graft

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include"ADM_video/ADM_vidField.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"


#if 1 || TEST_DECOMB
#include "ADM_vidDecTelecide.h"
#include "ADM_filter/video_filters.h"
static FILTER_PARAM decomb_template={16,{"order","back","guide",
	 	 	"gthresh","post","chroma","vthresh",
			"bthresh","dthresh","blend",
			"nt","y0","y1","hints",
			"show","debug"}};
BUILD_CREATE(decomb_create,Telecide);
SCRIPT_CREATE(decomb_script,Telecide,decomb_template);
extern uint8_t DIA_getDecombTelecide(TelecideParam *param);

uint8_t Telecide::configure(AVDMGenericVideoStream *in)
{
	_in=in;
	return DIA_getDecombTelecide(_param);
	
}

char *Telecide::printConf( void )
{
 	static char buf[50];

  	ADM_assert(_param); 	
 	sprintf((char *)buf," Decomb Telecide");
        return buf;
}


#define UPLANE(x) (x+_info.width*_info.height)
#define VPLANE(x) (x+((_info.width*_info.height*5)>>2))

#define PROGRESSIVE  0x00000001
#define MAGIC_NUMBER (0xdeadbeef)
#define IN_PATTERN   0x00000002


  uint8_t PutHintingData(unsigned char *video, unsigned int hint);
  uint8_t GetHintingData(unsigned char *video, unsigned int *hint);
  void BitBlt(uint8_t * dstp, int dst_pitch, const uint8_t* srcp,
            int src_pitch, int row_size, int height);
  void DrawString(uint8_t *dst, int x, int y, const char *s);
  void DrawStringYUY2(uint8_t *dst, int x, int y, const char *s); 

Telecide::Telecide(AVDMGenericVideoStream *in,CONFcouple *couples) 
{

		int i;		
		int count;
		char *d, *dsaved;
		unsigned int *p, *x;
		_lastFrame=0xfffffff0;
		
   		
		_in=in;		
   		memcpy(&_info,_in->getInfo(),sizeof(_info));    
  		_info.encoding=1;
		_uncompressed=NULL;
		vidCache=new VideoCache(12,in);
				 	
  		_info.encoding=1;

		
		pitch = _info.width;
		dpitch = _info.width;
		pitchover2 = pitch >> 1;
		pitchtimes4 = pitch << 2;
		w = _info.width;
		h = _info.height;
		wover2 = w/2;
		hover2 = h/2;
		hplus1over2 = (h+1)/2;
		hminus2= h - 2;
		_param=NEW(TelecideParam);
		if(couples)
		{
			GET(order);
			GET(back);
			GET(chroma);
			GET(guide);
			GET(gthresh);
			GET(post);
			GET(vthresh);
			GET(bthresh);
			GET(dthresh);
			GET(blend);
			GET(nt);
			GET(y0);
			GET(y1);
			GET(hints);
			GET(show);
			GET(debug);
		}
		else // Default
  		{
			 	
			 	_param->order = 1; 		// 0 Field ok, 1 field reverted 0 BFF/1 TFF
				_param->back = NO_BACK; // 0 Never, 1 when bad, 2 always tried MUST Have post !=0
				_param->chroma = false;
				_param->guide = GUIDE_32;// 0 / NONE - 1 GUIDE_32/ivtc-2 GUIDE 22/PAL-3 PAL/NTSC
				_param->gthresh = 10.0;
				_param->post = POST_NONE;
				_param->vthresh = 50.0;
				_param->bthresh = 50.0;
				_param->dthresh = 7.0;
				_param->blend = false;
				_param->nt = 10;	// Noise tolerance
				_param->y0 = 0;		// Zone to try (avoid subs)
				_param->y1 = 0;
				_param->hints = true;
				_param->show = false;
				_param->debug = false; 

		}
				 
				
		tff = (_param->order == 0 ? false : true);	

		_param->back_saved = _param->back;

		// Set up pattern guidance.
		cache = (struct CACHE_ENTRY *) malloc(CACHE_SIZE * sizeof(struct CACHE_ENTRY));
		for (i = 0; i < CACHE_SIZE; i++)
		{
			cache[i].frame = 0xffffffff;
			cache[i].chosen = 0xff;
		}

		if (_param->guide == GUIDE_32)
		{
			// 24fps to 30 fps telecine.
			cycle = 5;
		}
		if (_param->guide == GUIDE_22)
		{
			// PAL guidance (expect the current match to be continued).
			cycle = 2;
		}
		else if (_param->guide == GUIDE_32322)
		{
			// 25fps to 30 fps telecine.
			cycle = 6;
		}

		// Get needed dynamic storage.
		vmetric = 0;
		_param->vthresh_saved = _param->vthresh;
		xblocks = (_info.width+BLKSIZE-1) / BLKSIZE;
		yblocks = (_info.height+BLKSIZE-1) / BLKSIZE;
#ifdef WINDOWED_MATCH
		matchp = (unsigned int *) malloc(xblocks * yblocks * sizeof(unsigned int));
		
		matchc = (unsigned int *) malloc(xblocks * yblocks * sizeof(unsigned int));
		
#endif
		sump = (unsigned int *) malloc(xblocks * yblocks * sizeof(unsigned int));
		
		sumc = (unsigned int *) malloc(xblocks * yblocks * sizeof(unsigned int));
		

		
}
//____________________________________________________________________
Telecide::~Telecide()
{
		unsigned int *p;

		if (cache != NULL) free(cache);
#ifdef WINDOWED_MATCH
		if (matchp != NULL) free(matchp);
		if (matchc != NULL) free(matchc);
#endif
		if (sump != NULL) free(sump);
		if (sumc != NULL) free(sumc);

		delete vidCache;
		vidCache=NULL;
		
		
}
//____________________________________________________________________
void Telecide::Show(PVideoFrame &dst, int frame)
{
	char use;
	
	if (chosen == P) use = 'p';
	else if (chosen == C) use = 'c';
	else use = 'n';

	sprintf(buf, "Telecide %s", VERSION);
	DrawString(dst, 0, 0, buf);

	sprintf(buf, "Copyright 2003 Donald A. Graft");
	DrawString(dst, 0, 1, buf);

	sprintf(buf,"frame %d:", frame);
	DrawString(dst, 0, 3, buf);

	sprintf(buf, "matches: %d  %d  %d", p, c, np);
	DrawString(dst, 0, 4, buf);

	if (_param->post != POST_NONE)
	{
		sprintf(buf,"vmetrics: %d  %d  %d [chosen=%d]", pblock, cblock, npblock, vmetric);
		DrawString(dst, 0, 5, buf);
	}

	if (_param->guide != GUIDE_NONE)
	{
		sprintf(buf, "pattern mismatch=%0.2f%%", mismatch); 
		DrawString(dst, 0, 5 + (_param->post != POST_NONE), buf);
	}

	sprintf(buf,"[%s %c]%s %s",
		found == true ? "forcing" : "using", use,
		_param->post != POST_NONE ? (film == true ? " [progressive]" : " [interlaced]") : "",
		_param->guide != GUIDE_NONE ? status : "");
	DrawString(dst, 0, 5 + (_param->post != POST_NONE) + (_param->guide != GUIDE_NONE), buf);
}
//______________________________________________________________
void Telecide::Debug(int frame)
{
	char use;

	if (chosen == P) use = 'p';
	else if (chosen == C) use = 'c';
	else use = 'n';
	sprintf(buf,"Telecide: frame %d: matches: %d %d %d", frame, p, c, np);
	OutputDebugString(buf);
	if (_param->post != POST_NONE)
	{
		sprintf(buf,"Telecide: frame %d: vmetrics: %d %d %d [chosen=%d]", frame, pblock, cblock, npblock, vmetric);
		OutputDebugString(buf);
	}
	sprintf(buf,"Telecide: frame %d: [%s %c]%s %s", frame, found == true ? "forcing" : "using", use,
		_param->post != POST_NONE ? (film == true ? " [progressive]" : " [interlaced]") : "",
		_param->guide != GUIDE_NONE ? status : "");
	OutputDebugString(buf);
}

//______________________________________________________________
uint8_t Telecide::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
				uint8_t *data,uint32_t *flags)
{
uint32_t framep,framen;
uint8_t * fp,* fc, *fn, *dst, *final;
uint8_t * lc,* lp;

unsigned char *fprp, *fcrp, *fcrp_saved, *fnrp;
	unsigned char *fprpU, *fcrpU, *fnrpU;//, *fcrp_savedU
	unsigned char *fprpV, *fcrpV,  *fnrpV;//*fcrp_savedV
	unsigned char *dstp, *finalp;
	unsigned char *dstpU, *dstpV;
	
	unsigned char *crp, *prp;
	unsigned char *crpU, *prpU;
	unsigned char *crpV, *prpV;


	*len=(_info.width*_info.height*5)>>2;
	
	
//	dst = env->NewVideoFrame(vi);
	dst=data;
	// Get the current frame.
	
	if (frame > _info.nb_frames - 1) frame = _info.nb_frames - 1;
	
	framep=framen=frame;
	if(frame) framep=frame-1;
	if(frame<_info.nb_frames-1) framen=frame+1;
	
	
	//GETFRAME(pframe, fp);
	fp=vidCache->getImage(framep);
	ADM_assert(fp);
	fprp = (unsigned char *) fp;
	
	{
		fprpU = (unsigned char *)  UPLANE(fp);
		fprpV = (unsigned char *) VPLANE(fp );
	}
	
	fc=vidCache->getImage(frame);
	ADM_assert(fc);
	fcrp = (unsigned char *) fc;
	
	{
		fcrpU = (unsigned char *) UPLANE(fc);
		fcrpV = (unsigned char *) VPLANE(fc );
	}

		
	//GETFRAME(nframe, fn);
	fn=vidCache->getImage(framen);
	ADM_assert(fn);
	fnrp = (unsigned char *) fn;
	
	{
		fnrpU = (unsigned char *)  UPLANE(fn);
		fnrpV = (unsigned char *) VPLANE(fn );
	}
/*________________________________________________________________+++ */
	
/*________________________________________________________________+++ */
	// Ensure that the metrics for the frames
	// after the current frame are in the cache. They will be used for
	// pattern guidance.
	if (_param->guide != GUIDE_NONE)
	{
		for (y = frame + 1; y <= frame + cycle + 1; y++)
		{
			if (y >_info.nb_frames- 1) break;
			if (CacheQuery(y, &p, &pblock, &c, &cblock) == false)
			{
				
				
				//GETFRAME(y == 0 ? 1 : y - 1, lp);
				lp=vidCache->getImage(y == 0 ? 1 : y - 1);
				ADM_assert(lp);
				prp = (unsigned char *) lp;
				{
					prpU = (unsigned char *)  UPLANE(lp);
					prpV = (unsigned char *) VPLANE(lp );
				}
				//GETFRAME(y, lc);
				lc=vidCache->getImage(y);
				ADM_assert(lc);
				crp = (unsigned char *) lc;
				{
					crpU = (unsigned char *) UPLANE(lc);
					crpV = (unsigned char *) VPLANE(lc);
				}
				CalculateMetrics(y, crp, crpU, crpV, prp, prpU, prpV);
				vidCache->unlock(lp);
				vidCache->unlock(lc);
			}
		}
	}
/* Check for manual overrides of the field matching. */

	found = false;
	film = true;
	override = false;
	inpattern = false;
	_param->vthresh = _param->vthresh_saved;
	_param->back = _param->back_saved;
	

	// Get the metrics for the current-previous (p), current-current (c), and current-next (n) match
	// candidates.
	if (CacheQuery(frame, &p, &pblock, &c, &cblock) == false)
	{
		CalculateMetrics(frame, fcrp, fcrpU, fcrpV, fprp, fprpU, fprpV);
		CacheQuery(frame, &p, &pblock, &c, &cblock);
	}
	if (CacheQuery(framen, &np, &npblock, &nc, &ncblock) == false)
	{
		CalculateMetrics(framen, fnrp, fnrpU, fnrpV, fcrp, fcrpU, fcrpV);
		CacheQuery(framen, &np, &npblock, &nc, &ncblock);
	}

	// Determine the best candidate match.
	if (found != true)
	{
		lowest = c;
		chosen = C;
		if (_param->back == ALWAYS_BACK && p < lowest)
		{
			lowest = p;
			chosen = P;
		}
		if (np < lowest)
		{
			lowest = np;
			chosen = N;
		}
	}
	if ((frame == 0 && chosen == P) || (frame == _info.nb_frames - 1 && chosen == N))
	{
		chosen = C;
		lowest = c;
	}

	// See if we can apply pattern guidance.
	mismatch = 100.0;
	if (_param->guide != GUIDE_NONE)
	{
		hard = false;
		if (frame >= cycle && PredictHardYUY2(frame, &predicted, &predicted_metric) == true)
		{
			inpattern = true;
			mismatch = 0.0;
			hard = true;
			if (chosen != predicted)
			{
				// The chosen frame doesn't match the prediction.
				if (predicted_metric == 0) mismatch = 0.0;
				else mismatch = (100.0*abs(predicted_metric - lowest))/predicted_metric;
				if (mismatch < _param->gthresh)
				{
					// It's close enough, so use the predicted one.
					if (found != true)
					{
						chosen = predicted;
						override = true;
					}
				}
				else
				{
					hard = false;
					inpattern = false;
				}
			}
		}

		if (hard == false && _param->guide != GUIDE_22)
		{
			int i;
			struct PREDICTION *pred = PredictSoftYUY2(frame);

			if ((frame <= _info.nb_frames - 1 - cycle) &&	(pred[0].metric != 0xffffffff))
			{
				// Apply pattern guidance.
				// If the predicted match metric is within defined percentage of the
				// best calculated one, then override the calculated match with the
				// predicted match.
				i = 0;
				while (pred[i].metric != 0xffffffff)
				{
					predicted = pred[i].predicted;
					predicted_metric = pred[i].predicted_metric;
#ifdef DEBUG_PATTERN_GUIDANCE
					sprintf(buf, "%d: predicted = %d\n", frame, predicted);
					OutputDebugString(buf);
#endif
					if (chosen != predicted)
					{
						// The chosen frame doesn't match the prediction.
						if (predicted_metric == 0) mismatch = 0.0;
						else mismatch = (100.0*abs(predicted_metric - lowest))/predicted_metric;
						if ((int) mismatch <= _param->gthresh)
						{
							// It's close enough, so use the predicted one.
							if (found != true)
							{
								chosen = predicted;
								override = true;
							}
							inpattern = true;
							break;
						}
						else
						{
							// Looks like we're not in a predictable pattern.
							inpattern = false;
						}
					}
					else
					{
						inpattern = true;
						mismatch = 0.0;
						break;
					}
					i++;
				}
			}
		}
	}

	// Check for overrides of vthresh.
	
	// Check the match for progressive versus interlaced.
	if (_param->post != POST_NONE)
	{
		if (chosen == P) vmetric = pblock;
		else if (chosen == C) vmetric = cblock;
		else if (chosen == N) vmetric = npblock;

		if (found == false && _param->back == BACK_ON_COMBED && vmetric > _param->bthresh && p < lowest)
		{
			// Backward match.
			vmetric = pblock;
			chosen = P;
			inpattern = false;
			mismatch = 100;
		}
		if (vmetric > _param->vthresh)
		{
			// After field matching and pattern guidance the frame is still combed.
			film = false;
			if (found == false && (_param->post == POST_FULL_NOMATCH || _param->post == POST_FULL_NOMATCH_MAP))
			{
				chosen = C;
				vmetric = cblock;
				inpattern = false;
				mismatch = 100;
			}
		}
	}
	_param->vthresh = _param->vthresh_saved;

	// Setup strings for debug info.
	if (inpattern == true && override == false) strcpy(status, "[in-pattern]");
	else if (inpattern == true && override == true) strcpy(status, "[in-pattern*]");
	else strcpy(status, "[out-of-pattern]");

	// Assemble and output the reconstructed frame according to the final match.
	dstp = dst;
    
	{
		dstpU =  UPLANE(dst);
		dstpV = VPLANE(dst );
	}
	if (chosen == N)
	{
		// The best match was with the next frame.
		if (tff == true)
		{
			BitBlt(dstp, 2 * dpitch, fnrp, 2 * pitch, w, hover2);
			BitBlt(dstp + dpitch, 2 * dpitch, fcrp + pitch, 2 * pitch,	w, hover2);
			
			{
				BitBlt(dstpU, dpitch, fnrpU, pitch, w/2, h/4);
				BitBlt(dstpV, dpitch, fnrpV, pitch, w/2, h/4);
				BitBlt(dstpU + dpitch/2, dpitch, fcrpU + pitch/2, pitch, w/2, h/4);
				BitBlt(dstpV + dpitch/2, dpitch, fcrpV + pitch/2, pitch, w/2, h/4);
			}
		}
		else
		{
			BitBlt(dstp, 2 * dpitch, fcrp, 2 * pitch, w, hplus1over2);
			BitBlt(dstp + dpitch, 2 * dpitch, fnrp + pitch, 2 * pitch,	w, hover2);
			
			{
				BitBlt(dstpU, dpitch, fcrpU, pitch, w/2, h/4);
				BitBlt(dstpV, dpitch, fcrpV, pitch, w/2, h/4);
				BitBlt(dstpU + dpitch/2, dpitch, fnrpU + pitch/2, pitch, w/2, h/4);
				BitBlt(dstpV + dpitch/2, dpitch, fnrpV + pitch/2, pitch, w/2, h/4);
			}
		}
	}
	else if (chosen == C)
	{
		// The best match was with the current frame.
		BitBlt(dstp, 2 * dpitch, fcrp, 2 * pitch, w, hplus1over2);
		BitBlt(dstp + dpitch, 2 * dpitch, fcrp + pitch, 2 * pitch,	w, hover2);
		
		{
			BitBlt(dstpU, dpitch, fcrpU, pitch, w/2, h/4);
			BitBlt(dstpV, dpitch, fcrpV, pitch, w/2, h/4);
			BitBlt(dstpU + dpitch/2, dpitch, fcrpU + pitch/2, pitch, w/2, h/4);
			BitBlt(dstpV + dpitch/2, dpitch, fcrpV + pitch/2, pitch, w/2, h/4);
		}
	}
	else if (tff == false)
	{
		// The best match was with the previous frame.
		BitBlt(dstp, 2 * dpitch, fprp, 2 * pitch, w, hplus1over2);
		BitBlt(dstp + dpitch, 2 * dpitch, fcrp + pitch, 2 * pitch,	w, hover2);
		
		{
			BitBlt(dstpU, dpitch, fprpU, pitch, w/2, h/4);
			BitBlt(dstpV, dpitch, fprpV, pitch, w/2, h/4);
			BitBlt(dstpU + dpitch/2, dpitch, fcrpU + pitch/2, pitch, w/2, h/4);
			BitBlt(dstpV + dpitch/2, dpitch, fcrpV + pitch/2, pitch, w/2, h/4);
		}
	}
	else
	{
		// The best match was with the previous frame.
		BitBlt(dstp, 2 * dpitch, fcrp, 2 * pitch, w, hplus1over2);
		BitBlt(dstp + dpitch, 2 * dpitch, fprp + pitch, 2 * pitch,	w, hover2);
		
		{
			BitBlt(dstpU, dpitch, fcrpU, pitch, w/2, h/4);
			BitBlt(dstpV, dpitch, fcrpV, pitch, w/2, h/4);
			BitBlt(dstpU + dpitch/2, dpitch, fprpU + pitch/2, pitch, w/2, h/4);
			BitBlt(dstpV + dpitch/2, dpitch, fprpV + pitch/2, pitch, w/2, h/4);
		}
	}
	if (_param->guide != GUIDE_NONE) PutChosen(frame, chosen);

	/* Check for manual overrides of the deinterlacing. */
	
	force = 0;
	

	// Do postprocessing if enabled and required for this frame.
	if (_param->post == POST_NONE || _param->post == POST_METRICS)
	{
		if (force == '+') film = false;
		else if (force == '-') film = true;
	}
	else if ((force == '+') ||
		((_param->post == POST_FULL || _param->post == POST_FULL_MAP || _param->post == POST_FULL_NOMATCH || _param->post == POST_FULL_NOMATCH_MAP)
		         && (film == false && force != '-')))
	{
		unsigned char *dstpp, *dstpn;
		int v1, v2, z;

		if (_param->blend == true)
		{
			// Blend mode.
			final = data; //env->NewVideoFrame(vi);
			// Do first and last lines.
			finalp = final;
			dstp = dst;
			dstpn = dstp + dpitch;
			for (x = 0; x < w; x++)
			{
				finalp[x] = (((int)dstp[x] + (int)dstpn[x]) >> 1);
			}
			finalp = final + (h-1)*dpitch;
			dstp = dst + (h-1)*dpitch;
			dstpp = dstp - dpitch;
			for (x = 0; x < w; x++)
			{
				finalp[x] = (((int)dstp[x] + (int)dstpp[x]) >> 1);
			}
			// Now do the rest.
			dstp = dst + dpitch;
			dstpp = dstp - dpitch;
			dstpn = dstp + dpitch;
			finalp = final + dpitch;
			for (y = 1; y < h - 1; y++)
			{
				for (x = 0; x < w; x++)
				{
					v1 = (int)( dstp[x] - _param->dthresh);
					if (v1 < 0) v1 = 0; 
					v2 = (int)( dstp[x] + _param->dthresh);
					if (v2 > 235) v2 = 235; 
					if ((v1 > dstpp[x] && v1 > dstpn[x]) || (v2 < dstpp[x] && v2 < dstpn[x]))
					{
						if (_param->post == POST_FULL_MAP || _param->post == POST_FULL_NOMATCH_MAP)
						{
							
							{
								finalp[x] = 235;
							}
						}
						else
							finalp[x] = ((int)dstpp[x] + (int)dstpn[x] + (int)dstp[x] + (int)dstp[x]) >> 2;
					}
					else finalp[x] = dstp[x];
				}
				finalp += dpitch;
				dstp += dpitch;
				dstpp += dpitch;
				dstpn += dpitch;
			}

			
			{
				// Chroma planes.
				for (z = 0; z < 2; z++)
				{
					if (z == 0)
					{
						// Do first and last lines.
						finalp =  UPLANE(final);
						dstp =  UPLANE(dst);
						dstpn = dstp + dpitch/2;
						for (x = 0; x < wover2; x++)
						{
							finalp[x] = (((int)dstp[x] + (int)dstpn[x]) >> 1);
						}
						finalp =  UPLANE(final) + (hover2-1)*dpitch/2;
						dstp =  UPLANE(dst) + (hover2-1)*dpitch/2;
						dstpp = dstp - dpitch/2;
						for (x = 0; x < wover2; x++)
						{
							finalp[x] = (((int)dstp[x] + (int)dstpp[x]) >> 1);
						}
						// Now do the rest.
						finalp =  UPLANE(final) + dpitch/2;
						dstp =  UPLANE(dst)+ dpitch/2;
					}
					else
					{
						// Do first and last lines.
						finalp = VPLANE(final );
						dstp = VPLANE(dst );
						dstpn = dstp + dpitch/2;
						for (x = 0; x < wover2; x++)
						{
							finalp[x] = (((int)dstp[x] + (int)dstpn[x]) >> 1);
						}
						finalp =VPLANE( final ) + (hover2-1)*dpitch/2;
						dstp = VPLANE(dst ) + (hover2-1)*dpitch/2;
						dstpp = dstp - dpitch/2;
						for (x = 0; x < wover2; x++)
						{
							finalp[x] = (((int)dstp[x] + (int)dstpp[x]) >> 1);
						}
						// Now do the rest.
						finalp = VPLANE(final ) + dpitch/2;
						dstp = VPLANE(dst ) + dpitch/2;
					}
					dstpp = dstp - dpitch/2;
					dstpn = dstp + dpitch/2;
					for (y = 1; y < hover2 - 1; y++)
					{
						for (x = 0; x < wover2; x++)
						{
							v1 = (int)( dstp[x] - _param->dthresh);
							if (v1 < 0) v1 = 0; 
							v2 = (int)( dstp[x] + _param->dthresh);
							if (v2 > 235) v2 = 235; 
							if ((v1 > dstpp[x] && v1 > dstpn[x]) || (v2 < dstpp[x] && v2 < dstpn[x]))
							{
								if (_param->post == POST_FULL_MAP || _param->post == POST_FULL_NOMATCH_MAP)
								{
									finalp[x] = 128;
								}
								else
									finalp[x] = ((int)dstpp[x] + (int)dstpn[x] + (int)dstp[x] + (int)dstp[x]) >> 2;
							}
							else finalp[x] = dstp[x];
						}
						finalp += dpitch/2;
						dstp += dpitch/2;
						dstpp += dpitch/2;
						dstpn += dpitch/2;
					}
				}
			}
			if (_param->show == true) Show(final, frame);
			if (_param->debug == true) Debug(frame);
			if (_param->hints == true) WriteHints(final, film, inpattern);
			
			vidCache->unlockAll();
			return 1;
		}

		// Interpolate mode.
		// Luma plane.
		dstp = dst + dpitch;
		dstpp = dstp - dpitch;
		dstpn = dstp + dpitch;
		for (y = 1; y < h - 1; y+=2)
		{
			for (x = 0; x < w; x++)
			{
				v1 = (int)( dstp[x] - _param->dthresh);
				if (v1 < 0) v1 = 0; 
				v2 = (int)( dstp[x] + _param->dthresh);
				if (v2 > 235) v2 = 235; 
				if ((v1 > dstpp[x] && v1 > dstpn[x]) || (v2 < dstpp[x] && v2 < dstpn[x]))
				{
					if (_param->post == POST_FULL_MAP || _param->post == POST_FULL_NOMATCH_MAP)
					{
						
						{
							dstp[x] = 235;
						}
					}
					else
						dstp[x] = (dstpp[x] + dstpn[x]) >> 1;
				}
			}
			dstp += 2*dpitch;
			dstpp += 2*dpitch;
			dstpn += 2*dpitch;
		}

		
		{
			// Chroma planes.
			for (z = 0; z < 2; z++)
			{
				if (z == 0) dstp =  UPLANE(dst) + dpitch/2;
				else dstp = VPLANE(dst ) + dpitch/2;
				dstpp = dstp - dpitch/2;
				dstpn = dstp + dpitch/2;
				for (y = 1; y < hover2 - 1; y+=2)
				{
					for (x = 0; x < wover2; x++)
					{
						v1 = (int)( dstp[x] - _param->dthresh);
						if (v1 < 0) v1 = 0; 
						v2 = (int)( dstp[x] + _param->dthresh);
						if (v2 > 235) v2 = 235; 
						if ((v1 > dstpp[x] && v1 > dstpn[x]) || (v2 < dstpp[x] && v2 < dstpn[x]))
						{
							if (_param->post == POST_FULL_MAP || _param->post == POST_FULL_NOMATCH_MAP)
							{
								dstp[x] = 128;
							}
							else
								dstp[x] = (dstpp[x] + dstpn[x]) >> 1;
						}
					}
					dstp += dpitch;
					dstpp += dpitch;
					dstpn += dpitch;
				}
			}
		}
	}

	if (_param->show == true) Show(dst, frame);
	if (_param->debug == true) Debug(frame);
	if (_param->hints == true) WriteHints(dst, film, inpattern);
	vidCache->unlockAll();
	return 1;
}

void Telecide::CalculateMetrics(int frame, unsigned char *fcrp, unsigned char *fcrpU, unsigned char *fcrpV,
									unsigned char *fprp, unsigned char *fprpU, unsigned char *fprpV)
{
	int x, y, p, c, tmp1, tmp2, skip;
	bool vc;
    unsigned char *currbot0, *currbot2, *prevbot0, *prevbot2;
	unsigned char *prevtop0, *prevtop2, *prevtop4, *currtop0, *currtop2, *currtop4;
	unsigned char *a0, *a2, *b0, *b2, *b4;
	unsigned int diff, index;
#define T 4

	/* Clear the block sums. */
 	for (y = 0; y < yblocks; y++)
	{
 		for (x = 0; x < xblocks; x++)
		{
#ifdef WINDOWED_MATCH
			matchp[y*xblocks+x] = 0;
			matchc[y*xblocks+x] = 0;
#endif
			sump[y*xblocks+x] = 0;
			sumc[y*xblocks+x] = 0;
		}
	}

	/* Find the best field match. Subsample the frames for speed. */
	currbot0  = fcrp + pitch;
	currbot2  = fcrp + 3 * pitch;
	currtop0 = fcrp;
	currtop2 = fcrp + 2 * pitch;
	currtop4 = fcrp + 4 * pitch;
	prevbot0  = fprp + pitch;
	prevbot2  = fprp + 3 * pitch;
	prevtop0 = fprp;
	prevtop2 = fprp + 2 * pitch;
	prevtop4 = fprp + 4 * pitch;
	if (tff == true)
	{
		a0 = prevbot0;
		a2 = prevbot2;
		b0 = currtop0;
		b2 = currtop2;
		b4 = currtop4;
	}
	else
	{
		a0 = currbot0;
		a2 = currbot2;
		b0 = prevtop0;
		b2 = prevtop2;
		b4 = prevtop4;
	}
	p = c = 0;

	// Calculate the field match and film/video metrics.
	 skip = 1;
	
	for (y = 0, index = 0; y < h - 4; y+=4)
	{
		/* Exclusion band. Good for ignoring subtitles. */
		if (_param->y0 == _param->y1 || y < _param->y0 || y > _param->y1)
		{
			for (x = 0; x < w;)
			{
				
					index = (y/BLKSIZE)*xblocks + x/BLKSIZE;
				
				// Test combination with current frame.
				tmp1 = ((long)currbot0[x] + (long)currbot2[x]);
//				diff = abs((long)currtop0[x] - (tmp1 >> 1));
				diff = abs((((long)currtop0[x] + (long)currtop2[x] + (long)currtop4[x])) - (tmp1 >> 1) - tmp1);
				if (diff > _param->nt)
				{
					c += diff;
#ifdef WINDOWED_MATCH
					matchc[index] += diff;
#endif
				}

				tmp1 = currbot0[x] + T;
				tmp2 = currbot0[x] - T;
				vc = (tmp1 < currtop0[x] && tmp1 < currtop2[x]) ||
					 (tmp2 > currtop0[x] && tmp2 > currtop2[x]);
				if (vc)
				{
					sumc[index]++;
				}

				// Test combination with previous frame.
				tmp1 = ((long)a0[x] + (long)a2[x]);
				diff = abs((((long)b0[x] + (long)b2[x] + (long)b4[x])) - (tmp1 >> 1) - tmp1);
				if (diff > _param->nt)
				{
					p += diff;
#ifdef WINDOWED_MATCH
					matchp[index] += diff;
#endif
				}

				tmp1 = a0[x] + T;
				tmp2 = a0[x] - T;
				vc = (tmp1 < b0[x] && tmp1 < b2[x]) ||
					 (tmp2 > b0[x] && tmp2 > b2[x]);
				if (vc)
				{
					sump[index]++;
				}

				x += skip;
				if (!(x&3)) x += 4;
			}
		}
		currbot0 += pitchtimes4;
		currbot2 += pitchtimes4;
		currtop0 += pitchtimes4;
		currtop2 += pitchtimes4;
		currtop4 += pitchtimes4;
		a0		 += pitchtimes4;
		a2		 += pitchtimes4;
		b0		 += pitchtimes4;
		b2		 += pitchtimes4;
		b4		 += pitchtimes4;
	}

	if ( _param->chroma == true)
	{
		int z;

		for (z = 0; z < 2; z++)
		{
			// Do the same for the U plane.
			if (z == 0)
			{
				currbot0  = fcrpU + pitchover2;
				currbot2  = fcrpU + 3 * pitchover2;
				currtop0 = fcrpU;
				currtop2 = fcrpU + 2 * pitchover2;
				currtop4 = fcrpU + 4 * pitchover2;
				prevbot0  = fprpU + pitchover2;
				prevbot2  = fprpU + 3 * pitchover2;
				prevtop0 = fprpU;
				prevtop2 = fprpU + 2 * pitchover2;
				prevtop4 = fprpU + 4 * pitchover2;
			}
			else
			{
				currbot0  = fcrpV + pitchover2;
				currbot2  = fcrpV + 3 * pitchover2;
				currtop0 = fcrpV;
				currtop2 = fcrpV + 2 * pitchover2;
				currtop4 = fcrpV + 4 * pitchover2;
				prevbot0  = fprpV + pitchover2;
				prevbot2  = fprpV + 3 * pitchover2;
				prevtop0 = fprpV;
				prevtop2 = fprpV + 2 * pitchover2;
				prevtop4 = fprpV + 4 * pitchover2;
			}
			if (tff == true)
			{
				a0 = prevbot0;
				a2 = prevbot2;
				b0 = currtop0;
				b2 = currtop2;
				b4 = currtop4;
			}
			else
			{
				a0 = currbot0;
				a2 = currbot2;
				b0 = prevtop0;
				b2 = prevtop2;
				b4 = prevtop4;
			}

			for (y = 0, index = 0; y < hover2 - 4; y+=4)
			{
				/* Exclusion band. Good for ignoring subtitles. */
				if (_param->y0 == _param->y1 || y < _param->y0/2 || y > _param->y1/2)
				{
					for (x = 0; x < wover2;)
					{
						
							index = (y/BLKSIZE)*xblocks + x/BLKSIZE;
						
						// Test combination with current frame.
						tmp1 = ((long)currbot0[x] + (long)currbot2[x]);
						diff = abs((((long)currtop0[x] + (long)currtop2[x] + (long)currtop4[x])) - (tmp1 >> 1) - tmp1);
						if (diff > _param->nt)
						{
							c += diff;
#ifdef WINDOWED_MATCH
							matchc[index] += diff;
#endif
						}

						tmp1 = currbot0[x] + T;
						tmp2 = currbot0[x] - T;
						vc = (tmp1 < currtop0[x] && tmp1 < currtop2[x]) ||
							 (tmp2 > currtop0[x] && tmp2 > currtop2[x]);
						if (vc)
						{
							sumc[index]++;
						}

						// Test combination with previous frame.
						tmp1 = ((long)a0[x] + (long)a2[x]);
						diff = abs((((long)b0[x] + (long)b2[x] + (long)b4[x])) - (tmp1 >> 1) - tmp1);
						if (diff > _param->nt)
						{
							p += diff;
#ifdef WINDOWED_MATCH
							matchp[index] += diff;
#endif
						}

						tmp1 = a0[x] + T;
						tmp2 = a0[x] - T;
						vc = (tmp1 < b0[x] && tmp1 < b2[x]) ||
							 (tmp2 > b0[x] && tmp2 > b2[x]);
						if (vc)
						{
							sump[index]++;
						}

						x ++;
						if (!(x&3)) x += 4;
					}
				}
				currbot0 += 4*pitchover2;
				currbot2 += 4*pitchover2;
				currtop0 += 4*pitchover2;
				currtop2 += 4*pitchover2;
				currtop4 += 4*pitchover2;
				a0		 += 4*pitchover2;
				a2		 += 4*pitchover2;
				b0		 += 4*pitchover2;
				b2		 += 4*pitchover2;
				b4		 += 4*pitchover2;
			}
		}
	}

	// Now find the blocks that have the greatest differences.
#ifdef WINDOWED_MATCH
	highest_matchp = 0;
	for (y = 0; y < yblocks; y++)
	{
		for (x = 0; x < xblocks; x++)
		{
if (frame == 45 && matchp[y * xblocks + x] > 2500)
{
	sprintf(buf, "%d/%d = %d\n", x, y, matchp[y * xblocks + x]);
	OutputDebugString(buf);
}
			if (matchp[y * xblocks + x] > highest_matchp)
			{
				highest_matchp = matchp[y * xblocks + x];
			}
		}
	}
	highest_matchc = 0;
	for (y = 0; y < yblocks; y++)
	{
		for (x = 0; x < xblocks; x++)
		{
if (frame == 44 && matchc[y * xblocks + x] > 2500)
{
	sprintf(buf, "%d/%d = %d\n", x, y, matchc[y * xblocks + x]);
	OutputDebugString(buf);
}
			if (matchc[y * xblocks + x] > highest_matchc)
			{
				highest_matchc = matchc[y * xblocks + x];
			}
		}
	}
#endif
	if (_param->post != POST_NONE)
	{
		highest_sump = 0;
		for (y = 0; y < yblocks; y++)
		{
			for (x = 0; x < xblocks; x++)
			{
				if (sump[y * xblocks + x] > highest_sump)
				{
					highest_sump = sump[y * xblocks + x];
				}
			}
		}
		highest_sumc = 0;
		for (y = 0; y < yblocks; y++)
		{
			for (x = 0; x < xblocks; x++)
			{
				if (sumc[y * xblocks + x] > highest_sumc)
				{
					highest_sumc = sumc[y * xblocks + x];
				}
			}
		}
	}
#ifdef WINDOWED_MATCH
	CacheInsert(frame, highest_matchp, highest_sump, highest_matchc, highest_sumc);
#else
	CacheInsert(frame, p, highest_sump, c, highest_sumc);
#endif
}

struct PREDICTION *Telecide::PredictSoftYUY2(int frame)
	{
		// Use heuristics to look forward for a match.
		int i, j, y, c, n, phase;
		unsigned int metric;

		pred[0].metric = 0xffffffff;
		if (frame < 0 || frame > _info.nb_frames - 1 - cycle) return pred;

		// Look at the next cycle of frames.
		for (y = frame + 1; y <= frame + cycle; y++)
		{
			// Look for a frame where the current and next match values are
			// very close. Those are candidates to predict the phase, because
			// that condition should occur only once per cycle. Store the candidate
			// phases and predictions in a list sorted by goodness. The list will
			// be used by the caller to try the phases in order.
			c = cache[y%CACHE_SIZE].metrics[C]; 
			n = cache[y%CACHE_SIZE].metrics[N];
			if (c == 0) c = 1;
			metric = (100 * abs (c - n)) / c;
			phase = y % cycle;
			if (metric < 5)
			{
				// Place the new candidate phase in sorted order in the list.
				// Find the insertion point.
				i = 0;
				while (metric > pred[i].metric) i++;
				// Find the end-of-list marker.
				j = 0;
				while (pred[j].metric != 0xffffffff) j++;
				// Shift all items below the insertion point down by one to make
				// room for the insertion.
				j++;
				for (; j > i; j--)
				{
					pred[j].metric = pred[j-1].metric;
					pred[j].phase = pred[j-1].phase;
					pred[j].predicted = pred[j-1].predicted;
					pred[j].predicted_metric = pred[j-1].predicted_metric;
				}
				// Insert the new candidate data.
				pred[j].metric = metric;
				pred[j].phase = phase;
				if (_param->guide == GUIDE_32)
				{
					switch ((frame % cycle) - phase)
					{
					case -4: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case -3: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case -2: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case -1: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case  0: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case +1: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case +2: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case +3: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case +4: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					}
				}
				else if (_param->guide == GUIDE_32322)
				{
					switch ((frame % cycle) - phase)
					{
					case -5: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case -4: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case -3: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case -2: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case -1: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case  0: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case +1: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case +2: pred[j].predicted = N; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[N]; break; 
					case +3: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case +4: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					case +5: pred[j].predicted = C; pred[j].predicted_metric = cache[frame%CACHE_SIZE].metrics[C]; break; 
					}
				}
			}
#ifdef DEBUG_PATTERN_GUIDANCE
			sprintf(buf,"%d: metric = %d phase = %d\n", frame, metric, phase);
			OutputDebugString(buf);
#endif
		}
		return pred;
	}
	
void Telecide::WriteHints(unsigned char *dst, bool film, bool inpattern)
	{
		unsigned int hint;

		if (GetHintingData(dst, &hint) == true) hint = 0;
		if (film == true) hint |= PROGRESSIVE;
		else hint &= ~PROGRESSIVE;
		if (inpattern == true) hint |= IN_PATTERN;
		else hint &= ~IN_PATTERN;
		PutHintingData(dst, hint);
	}
bool Telecide::PredictHardYUY2(int frame, unsigned int *predicted, unsigned int *predicted_metric)
{
		// Look for pattern in the actual delivered matches of the previous cycle of frames.
		// If a pattern is found, use that to predict the current match.
		if (_param->guide == GUIDE_22)
		{
			if (cache[(frame-cycle)%CACHE_SIZE  ].chosen == 0xff ||
				cache[(frame-cycle+1)%CACHE_SIZE].chosen == 0xff)
				return false;
			switch ((cache[(frame-cycle)%CACHE_SIZE  ].chosen << 4) +
					(cache[(frame-cycle+1)%CACHE_SIZE].chosen))
			{
			case 0x11:
				*predicted = C;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[C];
				break;
			case 0x22:
				*predicted = N;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[N];
				break;
			default: return false;
			}
		}
		else if (_param->guide == GUIDE_32)
		{
			if (cache[(frame-cycle)%CACHE_SIZE  ].chosen == 0xff ||
				cache[(frame-cycle+1)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+2)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+3)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+4)%CACHE_SIZE].chosen == 0xff)
				return false;

			switch ((cache[(frame-cycle)%CACHE_SIZE  ].chosen << 16) +
					(cache[(frame-cycle+1)%CACHE_SIZE].chosen << 12) +
					(cache[(frame-cycle+2)%CACHE_SIZE].chosen <<  8) +
					(cache[(frame-cycle+3)%CACHE_SIZE].chosen <<  4) +
					(cache[(frame-cycle+4)%CACHE_SIZE].chosen))
			{
			case 0x11122:
			case 0x11221:
			case 0x12211:
			case 0x12221: 
			case 0x21122: 
			case 0x11222: 
				*predicted = C;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[C];
				break;
			case 0x22111:
			case 0x21112:
			case 0x22112: 
			case 0x22211: 
				*predicted = N;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[N];
				break;
			default: return false;
			}
		}
		else if (_param->guide == GUIDE_32322)
		{
			if (cache[(frame-cycle)%CACHE_SIZE  ].chosen == 0xff ||
				cache[(frame-cycle+1)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+2)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+3)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+4)%CACHE_SIZE].chosen == 0xff ||
				cache[(frame-cycle+5)%CACHE_SIZE].chosen == 0xff)
				return false;

			switch ((cache[(frame-cycle)%CACHE_SIZE  ].chosen << 20) +
					(cache[(frame-cycle+1)%CACHE_SIZE].chosen << 16) +
					(cache[(frame-cycle+2)%CACHE_SIZE].chosen << 12) +
					(cache[(frame-cycle+3)%CACHE_SIZE].chosen <<  8) +
					(cache[(frame-cycle+4)%CACHE_SIZE].chosen <<  4) +
					(cache[(frame-cycle+5)%CACHE_SIZE].chosen))
			{
			case 0x111122:
			case 0x111221:
			case 0x112211:
			case 0x122111:
			case 0x111222: 
			case 0x112221:
			case 0x122211:
			case 0x222111: 
				*predicted = C;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[C];
				break;
			case 0x221111:
			case 0x211112:

			case 0x221112: 
			case 0x211122: 
				*predicted = N;
				*predicted_metric = cache[frame%CACHE_SIZE].metrics[N];
				break;
			default: return false;
			}
		}
#ifdef DEBUG_PATTERN_GUIDANCE
		sprintf(buf, "%d: HARD: predicted = %d\n", frame, *predicted);
		OutputDebugString(buf);
#endif
		return true;
	}
void Telecide::PutChosen(int frame, unsigned int chosen)
	{
		int f;

		f = frame % CACHE_SIZE;
		if (frame < 0 || frame > _info.nb_frames - 1 || cache[f].frame != frame)
			return;
		cache[f].chosen = chosen;
	}

	void Telecide::CacheInsert(int frame, unsigned int p, unsigned int pblock,
									unsigned int c, unsigned int cblock)
	{
		int f;

		f = frame % CACHE_SIZE;
		if (frame < 0 || frame > _info.nb_frames - 1)
			ADM_assert(0);
		cache[f].frame = frame;
		cache[f].metrics[P] = p;
		if (f) cache[f-1].metrics[N] = p;
		cache[f].metrics[C] = c;
		cache[f].metrics[PBLOCK] = pblock;
		cache[f].metrics[CBLOCK] = cblock;
		cache[f].chosen = 0xff;
	}

	bool Telecide::CacheQuery(int frame, unsigned int *p, unsigned int *pblock,
									unsigned int *c, unsigned int *cblock)
	{
		int f;

		f = frame % CACHE_SIZE;
		if (frame < 0 || frame > _info.nb_frames - 1)
		{
			printf("Frame %d is out! (%d)\n",frame,_info.nb_frames-1);
			ADM_assert(0);
		}
		if (cache[f].frame != frame)
		{
			return false;
		}
		*p = cache[f].metrics[P];
		*c = cache[f].metrics[C];
		*pblock = cache[f].metrics[PBLOCK];
		*cblock = cache[f].metrics[CBLOCK];
		return true;
	}
uint8_t PutHintingData(unsigned char *video, unsigned int hint)
{
	unsigned char *p;
	unsigned int i, magic_number = MAGIC_NUMBER;
	bool error = false;

	p = video;
	for (i = 0; i < 32; i++)
	{
		*p &= ~1; 
		*p++ |= ((magic_number & (1 << i)) >> i);
	}
	for (i = 0; i < 32; i++)
	{
		*p &= ~1;
		*p++ |= ((hint & (1 << i)) >> i);
	}
	return error;
}

uint8_t GetHintingData(unsigned char *video, unsigned int *hint)
{
	unsigned char *p;
	unsigned int i, magic_number = 0;
	bool error = false;

	p = video;
	for (i = 0; i < 32; i++)
	{
		magic_number |= ((*p++ & 1) << i);
	}
	if (magic_number != MAGIC_NUMBER)
	{
		error = true;
	}
	else
	{
		*hint = 0;
		for (i = 0; i < 32; i++)
		{
			*hint |= ((*p++ & 1) << i);
		}
	}
	return error;
}
void BitBlt(uint8_t* dstp, int dst_pitch, const uint8_t* srcp,
            int src_pitch, int row_size, int height)
{
	for(uint32_t y=0;y<height;y++)
	{
		memcpy(dstp,srcp,row_size);
		dstp+=dst_pitch;
		srcp+=src_pitch;
	}


}	    	
void DrawString(uint8_t *dst, int x, int y, const char *s)
{
	printf("Decomb:%s\n",s);
}

uint8_t	Telecide::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(16);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(order);
	CSET(back);
	CSET(chroma);
	CSET(guide);
	CSET(gthresh);
	CSET(post);
	CSET(vthresh);
	CSET(bthresh);
	CSET(dthresh);
	CSET(blend);
	CSET(nt);
	CSET(y0);
	CSET(y1);
	CSET(hints);
	CSET(show);
	CSET(debug);
	
	return 1;

}

#endif
