/***************************************************************************
  FAC_toggle.cpp
  Handle dialog factory element : Toggle
  (C) 2006 Mean Fixounet@free.fr 
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);

class  ADM_Qbitrate : public QWidget
{
     Q_OBJECT
    
  signals:
        
        
   public slots:
        void comboChanged(int i);
  public:
        QSpinBox        *box;
        QComboBox       *combo;
        QLabel          *text1;
        QLabel          *text2;
        COMPRES_PARAMS  *compress;
        
        ADM_Qbitrate(QWidget *z,COMPRES_PARAMS *p,QGridLayout *layout,int line) : QWidget(z) 
        {
          
           compress=p;
           combo=new QComboBox(z);
           
#define add(x) combo->addItem(#x)
  
  add(Constant Bitrate);
  add(Constant Quality);
  add(Two pass-filesize);
  add(Two pass-Avg bitrate);
  
           combo->show();
           
           text1=new QLabel( "Encoding mode",z);
          text1->setBuddy(combo);
          text1->show();
          
           box=new QSpinBox(z);
           box->show();
           
           text2=new QLabel( "Bitrate",z);
           text2->setBuddy(combo);
          
          
          layout->addWidget(text1,line,0);
          layout->addWidget(combo,line,1);
          
          layout->addWidget(text2,line+1,0);
          layout->addWidget(box,line+1,1);
          int i;
          switch(compress-> mode)
          {
            case  COMPRESS_CQ: i=1;break;
            case  COMPRESS_CBR:i=0;break;
            case  COMPRESS_2PASS:i=2;break;
            case  COMPRESS_SAME:i=4;break;
            case  COMPRESS_2PASS_BITRATE:i=3;break;
            default: ADM_assert(0);
          }
          combo->setCurrentIndex(i);
          comboChanged(i);        
          QObject::connect(combo, SIGNAL(currentIndexChanged(int )), this, SLOT(comboChanged(int )));
          
          
        }
        virtual ~ADM_Qbitrate() ;
        void readBack(void);
};
void ADM_Qbitrate::readBack(void)
{
#define Mx(x) compress->mode=x
#define Vx(x) compress->x=box->value();
  switch(combo->currentIndex())
  {
    case 0: Mx(COMPRESS_CBR);Vx(bitrate);break;
    case 1: Mx(COMPRESS_CQ);Vx(qz);break;
    case 2: Mx(COMPRESS_2PASS);Vx(finalsize);break;
    case 3: Mx(COMPRESS_2PASS_BITRATE);Vx(bitrate);break;
    case 4: Mx(COMPRESS_SAME);break;
    default :
          ADM_assert(0);
  }
}
void ADM_Qbitrate::comboChanged(int i)
{
  printf("Changed\n"); 
  #define P(x) text2->setText(_(#x))
#define M(x,y) box->setMinimum  (x);box->setMaximum  (y);
#define S(x)   box->setValue(x);
  switch(i)
  {
    case 0: //CBR
          P(Bitrate (kb/s));
          M(0,20000);
          S(compress->bitrate);
          break; 
    case 1:// CQ
          P(Quantizer);
          M(2,31);
          S(compress->qz);
          break;
    case 2 : // 2pass Filesize
          P(FileSize (MB));
          M(1,8000);
          S(compress->finalsize);
          break;
    case 3 : // 2pass Avg
          P(Average Br (kb/s));
          M(0,20000);
          S(compress->avg_bitrate);
          break;
    case 4 : // Same Qz as input
          P(-);
          M(0,0);
          break;
    default:ADM_assert(0);
  }
}
ADM_Qbitrate::~ADM_Qbitrate()
{
#define DEL(x) if(x) {delete x;x=NULL;}
#if 0 // Automatically deleted
                 DEL(text1)
                 DEL(text2)
                 DEL(box) 
                 DEL(combo) 
#endif
};

//**********************************
diaElemBitrate::diaElemBitrate(COMPRES_PARAMS *p,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_BITRATE)
{
 
  param=(void *)p;
  memcpy(&copy,p,sizeof(copy));
  paramTitle=NULL;
  this->tip=tip;
  setSize(2);
}

diaElemBitrate::~diaElemBitrate()
{
  ADM_assert(myWidget);
#if 0 // Automatically deleted as it is a child of main dialog
  ADM_Qbitrate *z=(ADM_Qbitrate *)myWidget;
  
  if(z) delete z;

  myWidget=NULL;
#endif
}
void diaElemBitrate::setMe(void *dialog, void *opaque,uint32_t line)
{
  QGridLayout *layout=(QGridLayout*) opaque;
  
  ADM_Qbitrate *b=new ADM_Qbitrate( (QWidget *)dialog,(COMPRES_PARAMS *)&copy,layout,line);
  myWidget=(void *)b;
  
}
void diaElemBitrate::getMe(void)
{
  ((ADM_Qbitrate *)myWidget)->readBack();
  memcpy(param,&copy,sizeof(copy));
}

//EOF

