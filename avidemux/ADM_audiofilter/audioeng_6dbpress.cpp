/***************************************************************************
                          audioeng_6dbpress.cpp  -  description
                             -------------------
	Derived from audactity compressor
	see http://audacity.sf.net
 ***************************************************************************/
/**********************************************************************

  Audacity: A Digital Audio Editor

  Compressor.cpp

  Dominic Mazzoni

  Steve Jolly made it inherit from EffectSimpleMono.
  GUI added and implementation improved by Dominic Mazzoni, 5/11/2003.

**********************************************************************/
 
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
//#include "../toolkit.hxx"
// Ctor
//__________


AVDMProcessAudio_Compress::AVDMProcessAudio_Compress(AVDMGenericAudioStream * instream,DRCparam *p):AVDMBufferedAudioStream
    (instream)
{
#define AMP 4
    // nothing special here...
    double ramp, coef, off;

    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    _instream->goToTime(0);
    memcpy(&_param,p,sizeof(_param));
//
// The base is 100 ms
//

    strcpy(_name, "PROC:PRESS");
    mCircleSize=DRC_WINDOW;
    drc_cleanup();
   


};
void AVDMProcessAudio_Compress::drc_cleanup(void)
{
    double mCurRate=(double)_wavheader->frequency;
    
    
   for(int j=0; j<mCircleSize; j++) {
      mCircle[j] = 0.0;
      mLevelCircle[j] = _param.mFloor;
   }
   mCirclePos = 0;
   mRMSSum = 0.0;

   mGainDB = ((_param.mThresholdDB*-0.7) * (1 - 1/_param.mRatio));
   if (mGainDB < 0)
      mGainDB = 0;

   mThreshold = pow(10.0, _param.mThresholdDB/10); // factor of 10 because it's power

   if (_param.mUseGain)
      mGain = pow(10.0, mGainDB/20); // factor of 20 because it's amplitude
   else
      mGain = 1.0;

   mAttackFactor = exp(-log(_param.mFloor) / (mCurRate * _param.mAttackTime + 0.5));
   mDecayFactor = exp(log(_param.mFloor) / (mCurRate * _param.mDecayTime + 0.5));

   mLastLevel = 0.0;
   
   memset(mCircle,0,sizeof(mCircle));
   memset(follow,0,sizeof(follow));
   memset(mLevelCircle,0,sizeof(mLevelCircle));
    
}
//
AVDMProcessAudio_Compress::~AVDMProcessAudio_Compress()
{
 	delete _wavheader;
}


//_____________________________________________
uint32_t AVDMProcessAudio_Compress::grab(uint8_t * _outbuff)
{


    int len = 0,i;
    uint8_t *incoming=(uint8_t *)_buffer;

    len = _instream->read(ONE_CHUNK, (uint8_t *) (incoming));
    //printf("\n grabbing :%lu-->%lu\n",ms100,rdall);               
    // Block not filled
    if (len != ONE_CHUNK)
      {
	  printf("\n not enough...\n");
	  if (len == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
	  for(int i=len;i<ONE_CHUNK;i++)
	        incoming[i]=0;
      }
      // Convert to float between 1&-1
      for(i=0;i<ONE_CHUNK/2;i++)
      {
        value[i]=(double)_buffer[i]/32767.;   
      }
    /*******/
   
    int16_t *ofer=(int16_t *)_outbuff;     // Out
    len=ONE_CHUNK/2;// 16 bits / sample
    
    
    if (mLastLevel == 0.0) {
      int preSeed = mCircleSize;
      if (preSeed > len)
         preSeed = len;
      for(i=0; i<preSeed; i++)
         AvgCircle(value[i]);
   }

   for (i = 0; i < len; i++) {
      Follow(value[i], &follow[i], i);
   }

   for (i = 0; i < len; i++) {
      ofer[i] =(int16_t) (32767.*DoCompression(value[i], follow[i]));
   }  
   
    return len*2;

}
double AVDMProcessAudio_Compress::AvgCircle(double value)
{
   double level;

   // Calculate current level from root-mean-squared of
   // circular buffer ("RMS")
   mRMSSum -= mCircle[mCirclePos];
   mCircle[mCirclePos] = value*value;
   mRMSSum += mCircle[mCirclePos];
   level = sqrt(mRMSSum/mCircleSize);
   mLevelCircle[mCirclePos] = level;
   mCirclePos = (mCirclePos+1)%mCircleSize;  
   return level;
}

void AVDMProcessAudio_Compress::Follow(double x, double *outEnv, int maxBack)
{
   /*

   "Follow"ing algorithm by Roger B. Dannenberg, taken from
   Nyquist.  His description follows.  -DMM

   Description: this is a sophisticated envelope follower.
    The input is an envelope, e.g. something produced with
    the AVG function. The purpose of this function is to
    generate a smooth envelope that is generally not less
    than the input signal. In other words, we want to "ride"
    the peaks of the signal with a smooth function. The 
    algorithm is as follows: keep a current output value
    (called the "value"). The value is allowed to increase
    by at most rise_factor and decrease by at most fall_factor.
    Therefore, the next value should be between
    value * rise_factor and value * fall_factor. If the input
    is in this range, then the next value is simply the input.
    If the input is less than value * fall_factor, then the
    next value is just value * fall_factor, which will be greater
    than the input signal. If the input is greater than value *
    rise_factor, then we compute a rising envelope that meets
    the input value by working bacwards in time, changing the
    previous values to input / rise_factor, input / rise_factor^2,
    input / rise_factor^3, etc. until this new envelope intersects
    the previously computed values. There is only a limited buffer
    in which we can work backwards, so if the new envelope does not
    intersect the old one, then make yet another pass, this time
    from the oldest buffered value forward, increasing on each 
    sample by rise_factor to produce a maximal envelope. This will 
    still be less than the input.
    
    The value has a lower limit of floor to make sure value has a 
    reasonable positive value from which to begin an attack.
   */

   float level = AvgCircle(x);
   float high = mLastLevel * mAttackFactor;
   float low = mLastLevel * mDecayFactor;

   if (low < _param.mFloor)
      low = _param.mFloor;

   if (level < low)
      *outEnv = low;
   else if (level < high)
      *outEnv = level;
   else {
      // Backtrack
      double attackInverse = 1.0 / mAttackFactor;
      double temp = level * attackInverse;

      int backtrack = 50;
      if (backtrack > maxBack)
         backtrack = maxBack;

      double *ptr = &outEnv[-1];
      int i;
      bool ok = false;
      for(i=0; i<backtrack-2; i++) {
         if (*ptr < temp) {
            *ptr-- = temp;
            temp *= attackInverse;
         }
         else {
            ok = true;
            break;
         }   
      }

      if (!ok && backtrack>1 && (*ptr < temp)) {
         temp = *ptr;
         for (i = 0; i < backtrack-1; i++) {
            ptr++;
            temp *= mAttackFactor;
            *ptr = temp;
         }
      }
      else
         *outEnv = level;
   }

   mLastLevel = *outEnv;
}

float AVDMProcessAudio_Compress::DoCompression(float value, double env)
{
   float mult;
   float out;

   if (env > mThreshold)
      mult = mGain * pow(mThreshold/env, 1.0/_param.mRatio);
   else
      mult = mGain;

   out = value * mult;

   if (out > 1.0)
      out = 1.0;

   if (out < -1.0)
      out = -1.0;

   return out;
}
