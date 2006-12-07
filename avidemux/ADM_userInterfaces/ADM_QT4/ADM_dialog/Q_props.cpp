/***************************************************************************
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "ui_props.h"
#include "default.h"
#include "avidemutils.h"
#include "avi_vars.h"
#include "ADM_osSupport/ADM_misc.h"
static char *yesno[2]={_(" No"),_(" Yes")};
extern const char *getStrFromAudioCodec( uint32_t codec);
 class propWindow : public QDialog
 {
     Q_OBJECT

 public:
     propWindow();
     Ui_Dialog ui;
 public slots:
  
 private slots:
   

 private:
     
 };


propWindow::propWindow()     : QDialog()
 {
     ui.setupUi(this);
     uint8_t gmc, qpel,vop;
 uint32_t info=0;
 uint32_t war,har;
 uint16_t hh, mm, ss, ms;
 char text[80];
 const char *s;
  
    text[0] = 0;
    if (!avifileinfo)
        return;
  
        // Fetch info
        info=video_body->getSpecificMpeg4Info();
        vop=!!(info & ADM_VOP_ON);
        qpel=!!(info & ADM_QPEL_ON);
        gmc=!!(info & ADM_GMC_ON);

#define FILLTEXT(a,b,c) {snprintf(text,79,b,c);ui.a->setText(text);}
#define FILLTEXT4(a,b,c,d) {snprintf(text,79,b,c,d);ui.a->setText(text);}
#define FILLTEXT5(a,b,c,d,e) {snprintf(text,79,b,c,d,e);ui.a->setText(text);}
        
        FILLTEXT4(labeImageSize,"%lu x %lu  ", avifileinfo->width,avifileinfo->height);
        FILLTEXT(labelFrameRate, "%2.3f fps", (float) avifileinfo->fps1000 / 1000.F);
        FILLTEXT(labelNbOfFrames,"%ld frames", avifileinfo->nb_frames);
        FILLTEXT(label4CC, "%s ",      fourCC::tostring(avifileinfo->fcc));
        if (avifileinfo->nb_frames)
          {
                frame2time(avifileinfo->nb_frames, avifileinfo->fps1000,
                          &hh, &mm, &ss, &ms);
                snprintf(text,79, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
                ui.labelVideoDuration->setText(text);
          }
        war=video_body->getPARWidth();
        har=video_body->getPARHeight();
        getAspectRatioFromAR(war,har, &s);
        FILLTEXT5(LabelAspectRatio," %s (%u:%u)", s,war,har);
#define SET_YES(a,b) ui.a->setText(yesno[b])
#define FILL_(q) ui.q->setText(text);
        SET_YES(LabelPackedBitstream,vop);
        SET_YES(LabelQuarterPixel,qpel);
        SET_YES(LabelGMC,gmc);
        
         WAVHeader *wavinfo=NULL;
        if (currentaudiostream) wavinfo=currentaudiostream->getInfo();
          if(wavinfo)
          {
              
              switch (wavinfo->channels)
                {
                case 1:
                    sprintf(text, "MONO");
                    break;
                case 2:
                    sprintf(text, "STEREO");
                    break;
                default:
                    sprintf(text, "%d",wavinfo->channels);
                    break;
                }

                FILL_(labelChannels);
                FILLTEXT(labelFrequency, "%lu Hz", wavinfo->frequency);
                FILLTEXT4(labelBitrate, "%lu Bps / %lu kbps", wavinfo->byterate,wavinfo->byterate * 8 / 1000);
                
                sprintf(text, "%s ", getStrFromAudioCodec(wavinfo->encoding));
                FILL_(labelACodec);
                strcpy(text,"??:??:??.??");
                // Duration in seconds too
                if(currentaudiostream && wavinfo->byterate>1)
                {
                        uint32_t l=currentaudiostream->getLength();
                        double du;
                        du=l;
                        du*=1000;
                        du/=wavinfo->byterate;
                        ms2time((uint32_t)floor(du),
                                  &hh, &mm, &ss, &ms);
                        sprintf(text, "%02d:%02d:%02d.%03d (%u MBytes)", hh, mm, ss, ms
                                ,l>>20);
                }
                FILL_(labelAudioDuration);
                SET_YES(labelVBR,currentaudiostream->isVBR());
        } else
          {
              sprintf(text, _("NONE"));

                FILL_(labelACodec);
                FILL_(labelChannels);
                FILL_(labelBitrate);
                FILL_(labelFrequency);
                FILL_(labelAudioDuration);
                FILL_(labelVBR);
          }
 }
/**
    \fn DIA_properties
    \brief Display dialog with file information (size, codec, duration etc....)
*/
void DIA_properties( void )
{

      if (playing)
        return;
      if (!avifileinfo)
        return;
     // Fetch info
     propWindow *propwindow=new propWindow ;
     propwindow->exec();
     delete propwindow;
}  
//********************************************
//EOF
