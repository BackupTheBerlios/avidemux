/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

# include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ADM_default.h"

#include "prefs.h"



#include "ADM_assert.h" 

#include "DIA_encoding.h"
#include "ADM_video/ADM_vidMisc.h"
DIA_encoding::DIA_encoding( uint32_t fps1000 )
{
uint32_t useTray=0;
        if(!prefs->get(FEATURE_USE_SYSTRAY,&useTray)) useTray=0;



        _lastnb=0;
        _totalSize=0;
        _audioSize=0;
        _videoSize=0;
        _current=0;
        setFps(fps1000);
        _lastTime=0;
        _lastFrame=0;
        _fps_average=0;
        tray=NULL;
        _total=1000;

}
void DIA_encoding::setFps(uint32_t fps)
{
        _roundup=(uint32_t )floor( (fps+999)/1000);
        _fps1000=fps;
        ADM_assert(_roundup<MAX_BR_SLOT);
        memset(_bitrate,0,sizeof(_bitrate));
        _bitrate_sum=0;
        _average_bitrate=0;
        
}

void DIA_stop( void)
{
        printf("Stop request\n");

}
DIA_encoding::~DIA_encoding( )
{

}
void DIA_encoding::setPhasis(const char *n)
{
            fprintf(stderr,"Encoding Phase        : %s\n",n);

}
void DIA_encoding::setAudioCodec(const char *n)
{
            fprintf(stderr,"Encoding Audio codec  : %s\n",n);

}

void DIA_encoding::setCodec(const char *n)
{
            fprintf(stderr,"Encoding Video codec  : %s\n",n);

}
void DIA_encoding::setBitrate(uint32_t br,uint32_t globalbr)
{
         

}
void DIA_encoding::reset(void)
{
          
          _totalSize=0;
          _videoSize=0;
          _current=0;
}
void DIA_encoding::setContainer(const char *container)
{
        fprintf(stderr,"Encoding Container        : %s\n",container);
}
#define  ETA_SAMPLE_PERIOD 60000 //Use last n millis to calculate ETA
#define  GUI_UPDATE_RATE 500  

void DIA_encoding::setFrame(uint32_t nb,uint32_t size, uint32_t quant,uint32_t total)
{
          _total=total;
          _videoSize+=size;
          if(nb < _lastnb || _lastnb == 0) // restart ?
           {
                _lastnb = nb;
                clock.reset();
                _lastTime=clock.getElapsedMS();
                _lastFrame=0;
                _fps_average=0;
                _videoSize=size;
    
                _nextUpdate = _lastTime + GUI_UPDATE_RATE;
                _nextSampleStartTime=_lastTime + ETA_SAMPLE_PERIOD;
                _nextSampleStartFrame=0;
          } 
          _lastnb = nb;
          _current=nb%_roundup;
          _bitrate[_current].size=size;
          _bitrate[_current].quant=quant;
}
void DIA_encoding::updateUI(void)
{
uint32_t   tim;
uint32_t   hh,mm,ss;
uint32_t   cur,max;
uint32_t   percent;
          //
           //	nb/total=timestart/totaltime -> total time =timestart*total/nb
           //
           //
          if(!_lastnb) return;
          
          tim=clock.getElapsedMS();
          if(_lastTime > tim) return;
          if( tim < _nextUpdate) return ; 
          _nextUpdate = tim+GUI_UPDATE_RATE;
          // Average bitrate  on the last second
          uint32_t sum=0,aquant=0,gsum;
          for(int i=0;i<_roundup;i++)
          {
            sum+=_bitrate[i].size;
            aquant+=_bitrate[i].quant;
          }
          
          aquant/=_roundup;

          sum=(sum*8)/1000;

          // Now compute global average bitrate
          float whole=_videoSize,second;
            second=_lastnb;
            second/=_fps1000;
            second*=1000;
           
          whole/=second;
          whole/=1000;
          whole*=8;
      
          gsum=(uint32_t)whole;

          setBitrate(sum,gsum);
          setQuantIn(aquant);

          // compute fps
          uint32_t deltaFrame, deltaTime;
          deltaTime=tim-_lastTime;
          deltaFrame=_lastnb-_lastFrame;

          _fps_average    =(uint32_t)( deltaFrame*1000.0 / deltaTime ); 
  
  
            double framesLeft=(_total-_lastnb);
            ms2time((uint32_t)floor(0.5+deltaTime*framesLeft/deltaFrame),&hh,&mm,&ss);
  
           // Check if we should move on to the next sample period
          if (tim >= _nextSampleStartTime + ETA_SAMPLE_PERIOD ) {
            _lastTime=_nextSampleStartTime;
            _lastFrame=_nextSampleStartFrame;
            _nextSampleStartTime=tim;
            _nextSampleStartFrame=0;
          } else if (tim >= _nextSampleStartTime && _nextSampleStartFrame == 0 ) {
            // Store current point for use later as the next sample period.
            //
            _nextSampleStartTime=tim;
            _nextSampleStartFrame=_lastnb;
          }
          // update progress bar
            float f=_lastnb;
            f=f/_total;
            percent=(int)(100*f);
        
            _totalSize=_audioSize+_videoSize;
          setSize(_totalSize>>20);
          setAudioSizeIn((_audioSize>>20));
          setVideoSizeIn((_videoSize>>20));

          fprintf(stderr,"Done:%u%% Frames: %u/%u ETA: %02u:%02u:%02u\r",percent,_lastnb,_total,hh,mm,ss);

}
void DIA_encoding::setQuantIn(int size)
{
      
}

void DIA_encoding::setSize(int size)
{
      
}
void DIA_encoding::setAudioSizeIn(int size)
{
      
}
void DIA_encoding::setVideoSizeIn(int size)
{
      
}

void DIA_encoding::setAudioSize(uint32_t size)
{
      
}
uint8_t DIA_encoding::isAlive( void )
{
        updateUI();
        return 1;
}
