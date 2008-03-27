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

#include "ADM_assert.h"

typedef enum 
{
  ELEM_INVALID=0,
  ELEM_TOGGLE,
  ELEM_INTEGER,
  ELEM_FLOAT,
  ELEM_MENU,
  ELEM_FILE_READ,
  ELEM_BITRATE,
  ELEM_BAR,
  ELEM_ROTEXT,
  ELEM_NOTCH,
  ELEM_DIR_SELECT,
  ELEM_TEXT,
  ELEM_FRAME,
  ELEM_HEXDUMP,
  ELEM_TOGGLE_UINT,
  ELEM_TOGGLE_INT,
  ELEM_BUTTON,
  ELEM_SLIDER,
  ELEM_THREAD_COUNT,
  ELEM_MATRIX,
  ELEM_COUNT,
  ELEM_MAX=ELEM_COUNT-1
}elemEnum;
typedef void ADM_FAC_CALLBACK(void *cookie);

#define ELEM_TYPE_FLOAT float

/*********************************************/
class diaElem
{
  protected:
    void    setSize(int z) {size=z;};
    int     readOnly;
    diaElem    *internalPointer;
public:
  void *param;
  void *myWidget;
  const char *paramTitle;
  const char *tip;
  elemEnum mySelf;
  int       size; // Size of the widget in line

  diaElem(elemEnum num) {paramTitle=NULL;param=NULL;mySelf=num;myWidget=NULL;size=1;readOnly=0;internalPointer=NULL;};
          int getSize(void) {return size;};
  virtual ~diaElem() {};
  virtual void setMe(void *dialog, void *opaque,uint32_t line)=0;
  virtual void getMe(void)=0;
  virtual void setRo(void) {readOnly=1;}
  virtual void setRw(void) {readOnly=0;}
          
  virtual void enable(uint32_t onoff) {}
  virtual void finalize(void) {} // in case some widget needs some stuff just before using them
};
/*********************************************/
#define MENU_MAX_lINK 10
typedef struct dialElemLink
{
  uint32_t  value;
  uint32_t  onoff;
  diaElem  *widget;
}dialElemLink;
/*********************************************/

typedef void      DELETE_DIA_ELEM_T(diaElem *widget);
typedef void      FINALIZE_DIA_ELEM_T(void);
/*********************************************/
typedef diaElem  *(CREATE_BUTTON_T)(const char *toggleTitle, ADM_FAC_CALLBACK *cb,void *cookie,const char *tip);


class diaElemButton : public diaElem
{
  protected:
  public:
    void            *_cookie;
    ADM_FAC_CALLBACK *_callBack;
            diaElemButton(const char *toggleTitle, ADM_FAC_CALLBACK *cb,void *cookie,const char *tip=NULL);
  virtual   ~diaElemButton() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
};

/*********************************************/

class diaElemMatrix : public diaElem
{
  protected:
  public:
    uint8_t *_matrix;
    uint32_t _matrixSize;
    		diaElemMatrix(uint8_t *trix,const char *toggleTitle, uint32_t trixSize,const char *tip=NULL);
  virtual   ~diaElemMatrix() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
};
/************************************/
template <typename T>
class diaElemGenericSlider : public diaElem
{
  protected:
    
    T min,max,incr;
    uint32_t digits;
public:
    diaElemGenericSlider(T *value,const char *toggleTitle, T min,T max,T incr = 1, const char *tip=NULL);
  virtual   ~diaElemGenericSlider() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
  void      setDigits(uint32_t digits) { this->digits = digits; }
};
typedef diaElemGenericSlider <int32_t> diaElemSlider;
/* Same but unsigned */
typedef diaElemGenericSlider <uint32_t> diaElemUSlider;
/* Same but float */
typedef diaElemGenericSlider <ELEM_TYPE_FLOAT> diaElemFSlider;

/*********************************************/
class diaElemToggle : public diaElem
{
  protected:
    dialElemLink        links[MENU_MAX_lINK];
    uint32_t            nbLink;
    
public:
            diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip=NULL);
  virtual   ~diaElemToggle() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
  void      finalize(void);
  void      updateMe();
  uint8_t   link(uint32_t onoff,diaElem *w);
};
/*********************************************/
class diaElemToggleUint : public diaElem
{
  protected:
        uint32_t *emb;
        const char *embName;
        void *widgetUint;
        uint32_t _min,_max;
public:
            diaElemToggleUint(uint32_t *toggleValue,const char *toggleTitle, uint32_t *uintval, const char *name,uint32_t min,uint32_t max,const char *tip=NULL);
  virtual   ~diaElemToggleUint() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
  void      finalize(void);
  void      updateMe();
};
class diaElemToggleInt : public diaElemToggleUint
{
  protected:
        int32_t *emb;
        int32_t _min,_max;
public:
            diaElemToggleInt(uint32_t *toggleValue,const char *toggleTitle, int32_t *uintval, const char *name,int32_t min,int32_t max,const char *tip=NULL);
  virtual   ~diaElemToggleInt() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
};
/*********************************************/
typedef diaElem  *(CREATE_INTEGER_T)(int32_t *intValue,const char *toggleTitle,int32_t min, int32_t max,const char *tip);
typedef diaElem  *(CREATE_UINTEGER_T)(uint32_t *intValue,const char *toggleTitle,uint32_t min, uint32_t max,const char *tip);
class diaElemInteger : public diaElem
{

public:
  int32_t min,max;
  diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip=NULL);
  virtual ~diaElemInteger() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  void      enable(uint32_t onoff) ;
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
  void      enable(uint32_t onoff) ;
  
};
/*************************************************/
typedef diaElem  *(CREATE_BAR_T)(uint32_t percent,const char *toggleTitle);

class diaElemBar : public diaElem
{
  protected :
        uint32_t per;
public:
  
  diaElemBar(uint32_t percent,const char *toggleTitle);
  virtual ~diaElemBar() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
};

/*********************************************/
typedef diaElem  *(CREATE_FLOAT_T)(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, ELEM_TYPE_FLOAT min,
        ELEM_TYPE_FLOAT max,const char *tip);
class diaElemFloat : public diaElem
{

public:
  ELEM_TYPE_FLOAT min,max;
  diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, ELEM_TYPE_FLOAT min, 
               ELEM_TYPE_FLOAT max,const char *tip=NULL);
  virtual ~diaElemFloat() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  void      enable(uint32_t onoff) ;
};
/*************************************************/
typedef struct diaMenuEntry
{
  public:
    uint32_t    val;
    const char *text;
    const char *desc;
  public:
}diaMenuEntry;

class diaMenuEntryDynamic : public diaMenuEntry
{
  public:
  public:
    diaMenuEntryDynamic(uint32_t v,const char *t,const char *d) 
      {
          val=v;
          text=ADM_strdup(t);
          desc=ADM_strdup(d);
      }
    ~diaMenuEntryDynamic() 
      { 
          ADM_dealloc(text);
          ADM_dealloc(desc);
      }
    
};
//*******************************
// static (i.e. hardcoded) menu
//*******************************



//*******************************
// Same but for dynamic menus
//*******************************
class diaElemMenuDynamic : public diaElem
{
diaMenuEntryDynamic **menu;
uint32_t            nbMenu;
dialElemLink        links[MENU_MAX_lINK];
uint32_t            nbLink;

public:
  diaElemMenuDynamic(uint32_t *intValue,const char *itle, uint32_t nb, 
               diaMenuEntryDynamic **menu,const char *tip=NULL);
  
  virtual   ~diaElemMenuDynamic() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  uint8_t   link(diaMenuEntryDynamic *entry,uint32_t onoff,diaElem *w);
  void      updateMe(void);
  void      enable(uint32_t onoff) ;
  void      finalize(void);
};

class diaElemMenu : public diaElem
{
const diaMenuEntry  *menu;
uint32_t            nbMenu;
dialElemLink        links[MENU_MAX_lINK];
uint32_t            nbLink;

diaElemMenuDynamic  *dyna;
diaMenuEntryDynamic  **menus;
public:
  diaElemMenu(uint32_t *intValue,const char *itle, uint32_t nb, 
               const diaMenuEntry *menu,const char *tip=NULL);
  
  virtual ~diaElemMenu() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  uint8_t   link(diaMenuEntry *entry,uint32_t onoff,diaElem *w);
  void      updateMe(void);
  void      enable(uint32_t onoff) ;
  void      finalize(void);;
};

/*************************************************/
#include "ADM_encoder/ADM_vidEncode.hxx"

class diaElemBitrate : public diaElem
{
  protected:
    COMPRES_PARAMS    copy;
    uint32_t maxQ;
public:
  
  diaElemBitrate(COMPRES_PARAMS *p,const char *toggleTitle,const char *tip=NULL);
  virtual ~diaElemBitrate() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  void setMaxQz(uint32_t qz);
  
  void updateMe(void);
};

/*************************************************/
class diaElemFile : public diaElem
{

protected:
    const char * defaultSuffix;
public:
  
  diaElemFile(uint32_t writeMode,char **filename,const char *toggleTitle,
              const char *defaultSuffix = 0,const char *tip=NULL);
  virtual ~diaElemFile() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  uint32_t _write;
  void   changeFile(void);
  void   enable(uint32_t onoff);
};
/*************************************************/
class diaElemDirSelect : public diaElem
{

public:
  
  diaElemDirSelect(char **filename,const char *toggleTitle,const char *tip=NULL);
  virtual ~diaElemDirSelect() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  
  void changeFile(void);
  void   enable(uint32_t onoff);
};
/*************************************************/
/* The text MUST be copied internally ! */
class diaElemReadOnlyText : public diaElem
{

public:
  
  diaElemReadOnlyText(char *readyOnly,const char *toggleTitle,const char *tip=NULL);
  virtual ~diaElemReadOnlyText() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  
};
/*************************************************/
/* The text MUST be copied internally ! */
class diaElemText : public diaElem
{

public:
  
  diaElemText(char **text,const char *toggleTitle,const char *tip=NULL);
  virtual ~diaElemText() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void);
  void enable(uint32_t onoff);
};

/*********************************************/
class diaElemNotch : public diaElem
{
  uint32_t yesno;
public:
  
  diaElemNotch(uint32_t yes,const char *toggleTitle, const char *tip=NULL);
  virtual ~diaElemNotch() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void) {};
};
/*********************************************/
class diaElemTabs 
{
  public:
    
  uint32_t nbElems;
  diaElem  **dias;
  const char *title;
  
  diaElemTabs(const char *toggleTitle, uint32_t nb, diaElem **content)
  {
      nbElems=nb;
      dias=content; 
      title=toggleTitle;
  }
  virtual ~diaElemTabs() 
  {
  }
};
/**********************************************/
#define DIA_MAX_FRAME 10
class diaElemFrame : public diaElem
{
  uint32_t frameSize;
  uint32_t nbElems;
  diaElem  *elems[DIA_MAX_FRAME];
public:
  
  diaElemFrame(const char *toggleTitle, const char *tip=NULL);
  virtual ~diaElemFrame() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void) ;
  void swallow(diaElem *widget);
  void enable(uint32_t onoff);
  void finalize(void);
};
/**********************************************/
class diaElemHex : public diaElem
{
  uint32_t dataSize;
  uint8_t  *data;
  
public:
  
  diaElemHex(const char *toggleTitle, uint32_t dataSize,uint8_t *data);
  virtual ~diaElemHex() ;
  void setMe(void *dialog, void *opaque,uint32_t line);
  void getMe(void) ;
  void finalize(void);
};
/**********************************************/
class diaElemThreadCount : public diaElem
{

public:
  
  diaElemThreadCount(uint32_t *value, const char *title, const char *tip = NULL);
  virtual ~diaElemThreadCount() ;
  void setMe(void *dialog, void *opaque, uint32_t line);
  void getMe(void);
};
/*********************************************/
uint8_t diaFactoryRun(const char *title,uint32_t nb,diaElem **elems);
uint8_t diaFactoryRunTabs(const char *title,uint32_t nb,diaElemTabs **tabs);
/*********************************************/
#endif
