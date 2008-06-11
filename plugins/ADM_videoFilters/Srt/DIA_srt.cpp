//
// C++ Implementation: ADM_vidForcedPP
//
// Description:
//
//	Force postprocessing assuming constant quant & image type
//	Uselefull on some badly authored DVD for example
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ADM_default.h"
#include "ADM_videoFilterDynamic.h"
#include "DIA_coreToolkit.h"
#include "ADM_vidFont.h"
#include "ADM_vidSRT.h"
#define ADM_MINIMAL_UI_INTERFACE
#include "DIA_factory.h"
#include "ADM_colorspace.h"
/*****************************************************************/
extern int DIA_colorSel(uint8_t *r, uint8_t *g, uint8_t *b);
extern int DIA_srtPos(AVDMGenericVideoStream *source,uint32_t *size,uint32_t *position);
static void colorCallBack(void *cookie);
static void sizePositionCallback(void *cookie);
/*****************************************************************/
typedef struct unicd
{
	char *display,*name;
}unicd;

  static unicd  names[]=
{
	{(char *)"Ascii"	,(char *)"ISO-8859-1"},
	{(char *)"Cyrillic"	,(char *)"WINDOWS-1251"}, // ru
	{(char *)"Czech"	,(char *)"ISO-8859-2"},	// cz
	{(char *)"German"	,(char *)"ISO-8859-9"}		// german ?
	,{(char *)"Slovene"	,(char *)"CP1250"}		// UTF8

        ,{(char *)"UTF16"	,(char *)"UTF16"}		// UTF8
	,{(char *)"UTF8"	,(char *)"UTF8"}		// UTF8
	,{(char *)"Chinese Traditionnal(Big5)"	,(char *)"CP950"}		// UTF8
	,{(char *)"Chinese Simplified (GB2312)"	,(char *)"CP936"}		// UTF8
};
typedef struct
{
  AVDMGenericVideoStream *source;
  uint32_t               *size;
  uint32_t               *position;
}sizePosition;

/**
      \fn    DIA_srt
      \brief Dialog to handle srt, generic part
*/
uint8_t DIA_srt(AVDMGenericVideoStream *source, SUBCONF *param)
{
#define item(x) QT_TR_NOOP(names[x].display)
#define Mitem(x) {x,item(x)}
diaMenuEntry encoding[]={
  Mitem(0),
  Mitem(1),
  Mitem(2),
  Mitem(3),
  Mitem(4),
  Mitem(5),
  Mitem(6),
  Mitem(7),
};

#define PX(x) &(param->x)
  diaElemFile subtitle(0,(char **)PX(_subname),QT_TR_NOOP("_Subtitle file:"), NULL, QT_TR_NOOP("Select Subtitle file"));
  diaElemFile font(0,(char **)PX(_fontname),QT_TR_NOOP("_Font (TTF):"), NULL, QT_TR_NOOP("Select TTF file"));
  int colors[3]={param->_Y_percent,param->_U_percent,param->_V_percent};

  uint32_t fontSize=param->_fontsize;
  uint32_t baseLine=param->_baseLine;

  sizePosition sizePos={source,&fontSize,&baseLine};

  uint32_t myEncoding=0;
      // convert internal to display
      if(param->_charset)
      for(int i=0;i<sizeof(names)/sizeof(unicd);i++)
      {
          if(!strcmp(param->_charset,names[i].name)) myEncoding=i;
      }
      diaElemMenu      encodingM(&myEncoding,QT_TR_NOOP("_Encoding:"),8,encoding);
    //  diaElemUInteger  fontSize(PX(_fontsize),QT_TR_NOOP("Font Si_Ze:"),8,120);
      diaElemButton    color(QT_TR_NOOP("S_elect C_olor"), colorCallBack,&(colors[0]));
      diaElemButton    setBase(QT_TR_NOOP("Set Size and _Position"), sizePositionCallback,&sizePos);
      diaElemToggle    autoSplit(PX(_selfAdjustable),QT_TR_NOOP("_Auto split"));
      diaElemInteger   delay(PX(_delay),QT_TR_NOOP("_Delay (ms):"),-100000,100000);

      diaElem *tabs[]={&subtitle,&font,&encodingM,&color,&setBase,&autoSplit,&delay};
      if( diaFactoryRun(QT_TR_NOOP("Subtitler"),7,tabs))
	{
          if(param->_charset) ADM_dealloc(param->_charset);
          param->_charset=ADM_strdup(names[myEncoding].name);
            param->_Y_percent=colors[0];
            param->_U_percent=colors[1];
            param->_V_percent=colors[2];
            param->_fontsize=fontSize;
            param->_baseLine=baseLine;
          return 1;
        }
         return 0;
}
/**
      \fn colorCallBack
      \brief Callback used to select a color
*/
void colorCallBack(void *cookie)
{

      int32_t *colors=(int32_t *)cookie;

        uint8_t r,g,b;
        uint8_t y;
        int8_t u,v;

        y=colors[0];
        u=colors[1];
        v=colors[2];

        COL_YuvToRgb(   y,  u,  v, &b,&g,&r);
        if(DIA_colorSel(&r,&g,&b))
        {
                COL_RgbToYuv(b,  g,  r, &y, &u,&v);
                colors[0]=y;
                if(abs(u)<2) u=0;
                if(abs(v)<2) v=0;
                colors[1]=u;
                colors[2]=v;
        }
}
/**
      \fn sizePositionCallback
      \brief Callback used to set size and position
*/
void sizePositionCallback(void *cookie)
{
    sizePosition *sz=(sizePosition *)cookie;
    printf("Size and position invoked\n");
    DIA_srtPos(sz->source,sz->size,sz->position);
}
// EOF