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
        uint32_t        maxQ;
        ADM_Qbitrate(QWidget *z,COMPRES_PARAMS *p,uint32_t mq,QGridLayout *layout,int line) : QWidget(z) 
        {
          
           compress=p;
           combo=new QComboBox(z);
           
           maxQ=mq;
           int index=0,set=-1;
#define add(x,z,y) if(compress->capabilities & ADM_ENC_CAP_##x) {combo->addItem(QT_TR_NOOP(#y));\
						if(p->mode==COMPRESS_##z) set=index;\
						index++;}
  
  add(CBR,CBR,Constant Bitrate);
  add(CQ,CQ,Constant Quality);
  add(SAME,SAME,Same Quantizer as input);
  add(AQ,AQ,Average Quantizer);
  add(2PASS,2PASS,Two pass-filesize);
  add(2PASS_BR,2PASS_BITRATE,Two pass-Avg bitrate);
  
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
       
       
          if(set!=-1) 
          {
        	  	combo->setCurrentIndex(set);
        	  	comboChanged(set);
          }
          QObject::connect(combo, SIGNAL(currentIndexChanged(int )), this, SLOT(comboChanged(int )));
          
          
        }
        virtual ~ADM_Qbitrate() ;
        void readBack(void);
};

/**
 * 	\fn 	readPullDown
 * \brief 	Convert the raw read of the combox into the actual compression mode
 */
static COMPRESSION_MODE readPulldown(COMPRES_PARAMS *copy,int rank)
{
	int index=0;
	COMPRESSION_MODE mode=COMPRESS_MAX;
	
#undef LOOKUP
#define LOOKUP(A,B) \
  if(copy->capabilities & ADM_ENC_CAP_##A) \
  { \
	  if(rank==index) mode=COMPRESS_##B; \
	  index++; \
  } 
  
  LOOKUP(CBR,CBR);
  LOOKUP(CQ,CQ);
  LOOKUP(SAME,SAME);
  LOOKUP(AQ,AQ);
  LOOKUP(2PASS,2PASS);
  LOOKUP(2PASS_BR,2PASS_BITRATE);
  
	ADM_assert(mode!=COMPRESS_MAX);
	return mode;
}

void ADM_Qbitrate::readBack(void)
{
#define Mx(x) compress->mode=x
#define Vx(x) compress->x=box->value();
	COMPRESSION_MODE mode=readPulldown(compress,combo->currentIndex());
  switch(mode)
  {
    case COMPRESS_CBR: Mx(COMPRESS_CBR);Vx(bitrate);break;
    case COMPRESS_CQ: Mx(COMPRESS_CQ);Vx(qz);break;
    case COMPRESS_2PASS: Mx(COMPRESS_2PASS);Vx(finalsize);break;
    case COMPRESS_2PASS_BITRATE: Mx(COMPRESS_2PASS_BITRATE);Vx(avg_bitrate);break;
    case COMPRESS_SAME: Mx(COMPRESS_SAME);break;
    case COMPRESS_AQ: Mx(COMPRESS_AQ);break;
    default :
          ADM_assert(0);
  }
}
void ADM_Qbitrate::comboChanged(int i)
{
  printf("Changed\n"); 
 #define P(x) text2->setText(QT_TR_NOOP(#x))
#define M(x,y) box->setMinimum  (x);box->setMaximum  (y);
#define S(x)   box->setValue(x);
  COMPRESSION_MODE mode=readPulldown(compress,i);
    switch(mode)
  {
    case COMPRESS_CBR: //CBR
          P(Bitrate (kb/s));
          M(0,20000);
          S(compress->bitrate);
          break; 
    case COMPRESS_CQ:// CQ
          P(Quantizer);
          M(2,maxQ);
          S(compress->qz);
          break;
    case COMPRESS_2PASS : // 2pass Filesize
          P(FileSize (MB));
          M(1,8000);
          S(compress->finalsize);
          break;
    case COMPRESS_2PASS_BITRATE : // 2pass Avg
          P(Average Br (kb/s));
          M(0,20000);
          S(compress->avg_bitrate);
          break;
    case COMPRESS_SAME : // Same Qz as input
          P(-);
          M(0,0);
          break;
    case COMPRESS_AQ : // AQ
          P(Quantizer);
          M(2,maxQ);
          S(compress->qz);
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
  maxQ=31;
}
void diaElemBitrate::setMaxQz(uint32_t qz)
{
  maxQ=qz; 
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
  
  ADM_Qbitrate *b=new ADM_Qbitrate( (QWidget *)dialog,(COMPRES_PARAMS *)&copy,maxQ,layout,line);
  myWidget=(void *)b;
  
}
void diaElemBitrate::getMe(void)
{
  ((ADM_Qbitrate *)myWidget)->readBack();
  memcpy(param,&copy,sizeof(copy));
}

//EOF

