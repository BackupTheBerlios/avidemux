/**********************************************************************

  resample.h

  Real-time library interface by Dominic Mazzoni

  Based on resample-1.7:
    http://www-ccrma.stanford.edu/~jos/resample/

  License: LGPL - see the file LICENSE.txt for more information

**********************************************************************/

#ifndef LIBRESAMPLE_INCLUDED
#define LIBRESAMPLE_INCLUDED


void *resample_open(int      highQuality,
                    double   minFactor,
                    double   maxFactor);

int resample_get_filter_width(void *handle);

int resample_process(void   *handle,
                     double  factor,
                     int16_t  *inBuffer,
                     int     inBufferLen,
                     int     lastFlag,
                     int    *inBufferUsed,
                     int16_t  *outBuffer,
                     int     outBufferLen,
		     int     interleave);

void resample_close(void *handle);


#endif /* LIBRESAMPLE_INCLUDED */
