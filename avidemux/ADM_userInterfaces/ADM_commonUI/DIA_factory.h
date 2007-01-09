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
  ELEM_INTEGER,
  ELEM_FLOAT,
  ELEM_MAX=ELEM_TOGGLE
};
/*********************************************/
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
  virtual void setMe(void *dialog, void *opaque,uint32_t line)=0;
  virtual void getMe(void)=0;
};
/*********************************************/
class diaElemToggle : public diaElem
{

public:
  diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip=NULL);
  virtual ~diaElemToggle() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
};
/*********************************************/
class diaElemInteger : public diaElem
{

public:
  int32_t min,max;
  diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip=NULL);
  virtual ~diaElemInteger() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
};
/* Same but unsigned */
class diaElemUInteger : public diaElem
{

public:
  uint32_t min,max;
  diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip=NULL);
  virtual ~diaElemUInteger() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
};

/*********************************************/
#define ELEM_TYPE_FLOAT float
class diaElemFloat : public diaElem
{

public:
  ELEM_TYPE_FLOAT min,max;
  diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, ELEM_TYPE_FLOAT min, 
               ELEM_TYPE_FLOAT max,const char *tip=NULL);
  virtual ~diaElemFloat() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
};
/*********************************************/
uint8_t diaFactoryRun(const char *title,uint32_t nb,diaElem **elems);

#endif
