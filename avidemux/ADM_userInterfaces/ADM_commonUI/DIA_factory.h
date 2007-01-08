/***************************************************************************
                          DIA_factory.h
  Handles univeral dialog
  (C) Mean 2006 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DIA_FACTORY_H
#define DIA_FACTORY_H

typedef enum elemEnum
{
  ELEM_INVALID=0,
  ELEM_TOGGLE,
  ELEM_MAX=ELEM_TOGGLE
};

class diaElem
{

public:
  void *param;
  void *myWidget;
  const char *paramTitle;
  const char *tip;
  elemEnum mySelf;

  diaElem(elemEnum num) {param=NULL;mySelf=num;myWidget=NULL;};
  virtual ~diaElem() {};
  virtual void setMe(void *dialog, void *opaque)=0;
  virtual void getMe(void)=0;
};

class diaElemToggle : public diaElem
{

public:
  diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip=NULL);
  virtual ~diaElemToggle() ;
  void setMe(void *dialog, void *opaque);
  void getMe(void);
};

uint8_t diaFactoryRun(const char *title,uint32_t nb,diaElem **elems);

#endif
