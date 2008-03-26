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

#include "config.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QSpinBox>

#include "ADM_default.h"
#include "DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);
typedef enum
{
    TT_TOGGLE,TT_TOGGLE_UINT,TT_TOGGLE_INT
  
}TOG_TYPE;
class ADM_QCheckBox : public QCheckBox
{
      Q_OBJECT
    
  signals:
  protected:
        void     *_toggle;
        TOG_TYPE _type;
        
   public slots:
        void changed(int i)
        {
          switch(_type)
          {
            case TT_TOGGLE:
                              ((diaElemToggle *)_toggle)->updateMe();break;
            case TT_TOGGLE_UINT:
                              ((diaElemToggleUint *)_toggle)->updateMe();break;
            case TT_TOGGLE_INT:
                              ((diaElemToggleInt *)_toggle)->updateMe();break;
            default:
                  ADM_assert(0);
          }
        }
  public:
  ADM_QCheckBox(const QString & str,QWidget *root,void *toggle,TOG_TYPE type) : QCheckBox(str,root)
  {
    _toggle=toggle;
    _type=type;
  }
  void connectMe(void)
  {
    QObject::connect(this, SIGNAL(stateChanged(int)), this, SLOT(changed(int )));
  }
  
};

/**/



diaElemToggle::diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)toggleValue;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
  myWidget=NULL;
  nbLink=0;
}

diaElemToggle::~diaElemToggle()
{
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
 // if(box) delete box;
  myWidget=NULL;
  if(paramTitle)
    delete paramTitle;
}
void diaElemToggle::setMe(void *dialog, void *opaque,uint32_t l)
{
 ADM_QCheckBox *box=new ADM_QCheckBox(QString::fromUtf8(paramTitle),(QWidget *)dialog,this,TT_TOGGLE);
 QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
 if( *(uint32_t *)param)
 {
    box->setCheckState(Qt::Checked); 
 }
 box->show();
 layout->addWidget(box,l,0);
 box->connectMe();
}
void diaElemToggle::getMe(void)
{
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  uint32_t *val=(uint32_t *)param;
  if(Qt::Checked==box->checkState())
  {
    *val=1; 
  }else
    *val=0;
}
void diaElemToggle::enable(uint32_t onoff) 
{
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(TRUE);
  else
    box->setDisabled(TRUE);
}

void   diaElemToggle::finalize(void)
{
  updateMe(); 
}
void   diaElemToggle::updateMe(void)
{
 
  uint32_t val;
  uint32_t rank=0;
  if(!nbLink) return;
  ADM_assert(myWidget);
  
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  
  if(Qt::Checked==box->checkState())
  {
    rank=1;
  }
  /* Now search through the linked list to see if something happens ...*/
  
   /* 1 disable everything */
  for(int i=0;i<nbLink;i++)
  {
    dialElemLink *l=&(links[i]);
    l->widget->enable(0);
  }
  /* Then enable */
  for(int i=0;i<nbLink;i++)
  {
      dialElemLink *l=&(links[i]);
      if(l->onoff==rank)  l->widget->enable(1);
  }
}

uint8_t   diaElemToggle::link(uint32_t onoff,diaElem *w)
{
    ADM_assert(nbLink<MENU_MAX_lINK);
    links[nbLink].onoff=onoff;
    links[nbLink].widget=w;
    nbLink++;
    return 1;
}
//******************************************************
// An UInt and a toggle linked...
//******************************************************
diaElemToggleUint::diaElemToggleUint(uint32_t *toggleValue,const char *toggleTitle, uint32_t *uintval, const char *name,uint32_t min,uint32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE_UINT)
{
  param=(void *)toggleValue;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
  embName=name;
  emb=uintval;
  widgetUint=NULL;
  _min=min;
  _max=max;
}

diaElemToggleUint::~diaElemToggleUint()
{
   if(paramTitle)
    delete paramTitle;
}
void diaElemToggleUint::setMe(void *dialog, void *opaque,uint32_t line)
{
 ADM_QCheckBox *box=new ADM_QCheckBox(QString::fromUtf8(paramTitle),(QWidget *)dialog,this,TT_TOGGLE_UINT);
 QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
 if( *(uint32_t *)param)
 {
    box->setCheckState(Qt::Checked); 
 }
 box->show();
 layout->addWidget(box,line,0);
 // Now add spin
 QSpinBox *spin=new QSpinBox((QWidget *)dialog);
 widgetUint=(void *)spin; 
   
 spin->setMinimum(_min);
 spin->setMaximum(_max);
 spin->setValue(*(uint32_t *)emb);
 spin->show();
 layout->addWidget(spin,line,1);
 box->connectMe();
}

void diaElemToggleUint::getMe(void)
{
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  uint32_t *val=(uint32_t *)param;
  if(Qt::Checked==box->checkState())
  {
    *val=1; 
  }else
    *val=0;
  //
    uint32_t u;
  QSpinBox *spin=(QSpinBox *)widgetUint;
  u=spin->value();
 if(u<_min) u=_min;
 if(u>_max) u=_max;
 *emb=u;
  
}
void   diaElemToggleUint::finalize(void)
{
  updateMe();
}
void   diaElemToggleUint::updateMe(void)
{
  uint32_t val;
  uint32_t rank=FALSE;
  ADM_assert(myWidget);
  
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  QSpinBox *spin=(QSpinBox *)widgetUint;
  
  if(Qt::Checked==box->checkState())
  {
    rank=TRUE;
  }
  spin->setEnabled(rank);
}
void   diaElemToggleUint::enable(uint32_t onoff)
{
    ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
      QSpinBox *spin=(QSpinBox *)widgetUint;
  ADM_assert(box);
  if(onoff)
  {
    box->setEnabled(TRUE);
    spin->setEnabled(TRUE);
  }
  else
  {
    box->setEnabled(FALSE);
    spin->setEnabled(FALSE);
  }
}

//******************************************************
// An Int and a toggle linked...
//******************************************************

diaElemToggleInt::diaElemToggleInt(uint32_t *toggleValue,const char *toggleTitle, int32_t *uintval, const char *name,int32_t min,int32_t max,const char *tip)
  : diaElemToggleUint(toggleValue,toggleTitle, NULL, name,0,0,tip)
{
   param=(void *)toggleValue;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
  embName=name;
  emb=uintval;
  widgetUint=NULL;
  _min=min;
  _max=max;
}

diaElemToggleInt::~diaElemToggleInt()
{
  
}
void diaElemToggleInt::setMe(void *dialog, void *opaque,uint32_t line)
{
 ADM_QCheckBox *box=new ADM_QCheckBox(QString::fromUtf8(paramTitle),(QWidget *)dialog,this,TT_TOGGLE_INT);
 QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
 if( *(uint32_t *)param)
 {
    box->setCheckState(Qt::Checked); 
 }
 box->show();
 layout->addWidget(box,line,0);
 // Now add spin
 QSpinBox *spin=new QSpinBox((QWidget *)dialog);
 widgetUint=(void *)spin; 
   
 spin->setMinimum(_min);
 spin->setMaximum(_max);
 spin->setValue(*emb);
 spin->show();
 layout->addWidget(spin,line,1);
 box->connectMe();
}

void diaElemToggleInt::getMe(void)
{
  ADM_QCheckBox *box=(ADM_QCheckBox *)myWidget;
  uint32_t *val=(uint32_t *)param;
  if(Qt::Checked==box->checkState())
  {
    *val=1; 
  }else
    *val=0;
  //
    int32_t u;
  QSpinBox *spin=(QSpinBox *)widgetUint;
  u=spin->value();
 if(u<_min) u=_min;
 if(u>_max) u=_max;
 *emb=u;
}
//******************************************************




//EOF
