#include <stdio.h>
#include <stdlib.h>
#include "toolame.h"
#include "toolame_global_flags.h"

void doEnergyLevels(toolame_options *encodeOptions, unsigned char *mp2buffer, int frameEnd, short int *leftpcm, short int *rightpcm) {
  /* Reference:
     Using the BWF Energy Levels in AudioScience Bitstreams
     http://www.audioscience.com/internet/technical/app0001.pdf

     The absolute peak of the PCM file for the left and right
     channel in this frame are written into the last 5 bytes of the frame.
     
     The last 5 bytes *must* be reserved for this to work correctly (otherwise
     you'll be overwriting mpeg audio data)
  */
     
  int i;
  static int leftMax, rightMax;
  unsigned char rhibyte, rlobyte, lhibyte, llobyte;

  /* find the maximum in the left and right channels */
  leftMax = rightMax = -1;
  for (i=0; i<1152;i++) { 
    if (abs(leftpcm[i])>leftMax)
      leftMax = abs(leftpcm[i]);
    if (abs(rightpcm[i])>rightMax)
      rightMax = abs(rightpcm[i]);
  }

  /* convert max value to hi/lo bytes and write into buffer */
  lhibyte = leftMax/256;
  llobyte = leftMax - 256*lhibyte;
  mp2buffer[frameEnd-1] = llobyte;
  mp2buffer[frameEnd-2] = lhibyte;
  
  if (encodeOptions->mode!=MPG_MD_MONO) {
    // Only write the right channel energy info
    // if we're in stereo mode.
    mp2buffer[frameEnd-3] = 0;
    
    rhibyte = rightMax/256;
    rlobyte = rightMax - 256*rhibyte;
    mp2buffer[frameEnd-4] = rlobyte;
    mp2buffer[frameEnd-5] = rhibyte;
  }
}
