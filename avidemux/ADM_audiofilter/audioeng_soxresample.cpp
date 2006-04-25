/***************************************************************************
                          audioeng_soxresample.cpp  - 
			   description
                             -------------------
	Port of sox resampling code	
	Changes:
		- suit to avidemux way of handling streams
		- switch from int32 to 	int16
			     
    begin                : Sun Novemeber 23 2003
    copyright            : (C) 2002/2003 by mean
    email                : fixounet@free.fr
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx" 


AVDMProcessAudio_SoxResample::~AVDMProcessAudio_SoxResample() 
{ 
	if(!_pass)
	{
            for(int i=0;i<_wavheader->channels;i++)
                sox_stop(&(_resamples[i]));
	}
        delete [] _resamples;
  	delete _wavheader;
};   

AVDMProcessAudio_SoxResample::AVDMProcessAudio_SoxResample(AVDMGenericAudioStream *instream,uint32_t  frequency)
	:AVDMBufferedAudioStream(instream)
{
	_pass=0;
	_frequency=frequency;
	if(instream->getInfo()->frequency==frequency)
	{
		printf("Same in/out frequency, bypassing filter\n");
		_pass=1;
	}
    	_wavheader = new WAVHeader;
    	memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));    
    	_wavheader->frequency = frequency;
    	_wavheader->byterate =_wavheader->channels * _wavheader->frequency * 2;
    	strcpy(_name, "PROC:RSOX");    	    	
    
    	double l;
    	l=_instream->getLength() ;
    	l=l*frequency;
    	l=l/_instream->getInfo()->frequency;
    	_length = (uint32_t)floor(l);
	_length=_length & 0xfffffffc;
    	printf("resampling to %lu, %lu -> %lu bytes.\n",(unsigned long int)frequency,
			(unsigned long int)instream->getLength(),
			(unsigned long int)_length);
	if(!_pass)
	{
            int org=_instream->getInfo()->frequency;
            _resamples=new ResampleStruct[_wavheader->channels];
            for(int i=0;i<_wavheader->channels;i++)
            {
                printf("[Sox Resample] from %d to %d for track %i\n",org,frequency,i);
                ADM_assert( sox_init(org,frequency,&(_resamples[i]))) ;
            }
	}
        _head=_tail=0;
}
// we need to purge the buffers when going async


uint8_t AVDMProcessAudio_SoxResample::goToTime(uint32_t newoffset) 
{
	ADM_assert(!newoffset);
        printf("[Sox Resample] : Going back to beginning\n");
	_instream->goToTime(0);
	_headBuff=_tailBuff=0;
	if(!_pass)
	{
            for(int i=0;i<_wavheader->channels;i++)
            {
               sox_stop(&(_resamples[i]));
               ADM_assert( sox_init(_instream->getInfo()->frequency, _frequency,
                           &(_resamples[i]))) ;
            }
	}
	_head=_tail=0;
	return 1;
}
uint32_t 	AVDMProcessAudio_SoxResample::grab(uint8_t *obuffer)
{

    uint32_t rd = 0, rdall = 0, startrd=0;
    uint8_t *in;
    uint32_t nbout, asked,nb_in;
    uint32_t onechunk;   
    uint8_t *myBuffer;
    uint32_t snbout=0;
    static int32_t total=0;
    // the buffer is 750 kbytes
#define BLK_SIZE 8192 // Block size in sample
    onechunk=BLK_SIZE*2*_wavheader->channels;
    myBuffer=(uint8_t *)_buffer;
        
    // passthrought mode
    if(_pass)
    {
    	rd= _instream->read(onechunk, (uint8_t  *)obuffer);
	if(!rd) return MINUS_ONE;
	return rd;
     }
    // Read n samples
    // Go to the beginning of current block
    in = myBuffer+_head;
    rdall=_tail-_head;
    startrd=rdall;
    while (rdall < onechunk)
      {
	  // don't ask too much front.
	  asked = (onechunk) - rdall;
	  if (asked > onechunk)
	      asked = onechunk;

	  rd = _instream->read(asked, myBuffer + _tail);
	  total+=rd;
	  _tail+=rd;
	  rdall += rd;
	  if (rd == 0)
	  {
	  	if(_tail&1) _tail--;
	      break;
	  }
      }
      ADM_assert(PROCESS_BUFFER_SIZE>_tail);
      
      // we did not get a single byte
      if(rdall==startrd)
      { // Nothing read
        if( (_tail-_head)<256) // Why 256, why not ?
        {       // And buffer empty
      	 return MINUS_ONE;
        }
        else // Padd if needed so that we can resample
        {
                onechunk=_tail-_head;
        }
      }
    

	while(1)
	{
                nb_in=(_tail-_head)/(2*_wavheader->channels);
                if(!nb_in) break;
                nbout=BLK_SIZE;
                uint32_t nbi=nb_in;
                for(int i=0;i<_wavheader->channels;i++)
                {
                    nbout=BLK_SIZE;;
                    if(!sox_run	(&(_resamples[i]), (int16_t *)(myBuffer+_head+2*i), 
                                    (int16_t *)(obuffer+2*i),
                                    &nbi, &nbout,_wavheader->channels-1))
                    {
                        printf("[Sox Resample] run error!!\n");
                        return 0;
                    }
                    
                }
                _head=_head+(nbi*2*_wavheader->channels);
                snbout+=nbout*2*_wavheader->channels;
                obuffer+=nbout*2*_wavheader->channels;
                if(!nbout) break;
	}
      
      	ADM_assert(_tail>=_head);
	ADM_assert(snbout<PROCESS_BUFFER_SIZE);
	if(PROCESS_BUFFER_SIZE-_tail < MINIMUM_BUFFER)
	{
		_tail=_tail-_head;
		memmove(myBuffer,myBuffer+_head,_tail);
		_head=0;
		total=0;		
	}
	ADM_assert( 0==(_head&1));
	ADM_assert( 0==(_tail&1));
	
    	return snbout;    	

}

