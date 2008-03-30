/***************************************************************************
                          DIA_factory.cpp
  This file contains only redirect functions.
  Check the GTK/QT/.. functions to see the intersting parts
  (C) Mean 2008 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ADM_default.h"
#include "DIA_coreUI_internal.h"
#include "DIA_factoryStubs.h"

static FactoryDescriptor *Factory=NULL;
/**
 * 	\fn FactoryDescriptor
 */
uint8_t  DIA_factoryInit(FactoryDescriptor *d)
{
	Factory=d;
	return 1;
}
// ****************** All ************************
uint8_t diaFactoryRun(const char *title,uint32_t nb,diaElem **elems)
{
	ADM_assert(Factory); 
	return Factory->FactoryRun(title,nb,elems);
}
uint8_t diaFactoryRunTabs(const char *title,uint32_t nb,diaElemTabs **tabs)
{
	ADM_assert(Factory); 
	return Factory->FactoryRunTab(title,nb,tabs);
}

// ****************** Buttons ********************
diaElemButton ::diaElemButton(const char *toggleTitle, ADM_FAC_CALLBACK *cb,void *cookie,const char *tip) :diaElem(ELEM_BUTTON)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreareButton(toggleTitle, cb,cookie,tip);
}
diaElemButton ::~diaElemButton()
{
	ADM_assert(Factory); 
	Factory->DestroyButton(internalPointer);
	internalPointer=NULL;
}
void      diaElemButton::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemButton)
// ****************** Bar ********************
diaElemBar ::diaElemBar(uint32_t percent,const char *toggleTitle) :diaElem(ELEM_BAR)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateBar(percent,toggleTitle);
}
diaElemBar ::~diaElemBar()
{
	ADM_assert(Factory); 
	Factory->DestroyBar(internalPointer);
	internalPointer=NULL;
}
DIA_MKSTUBS(diaElemBar)

// ****************** Buttons ********************
diaElemFloat ::diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, ELEM_TYPE_FLOAT min,
        ELEM_TYPE_FLOAT max,const char *tip) :diaElem(ELEM_FLOAT)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateFloat(intValue,toggleTitle, min,max,tip);
}
diaElemFloat ::~diaElemFloat()
{
	ADM_assert(Factory); 
	Factory->DestroyFloat(internalPointer);
	internalPointer=NULL;
}
void      diaElemFloat::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemFloat)
// ****************** Integer ********************
diaElemInteger ::diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip) :
	diaElem(ELEM_INTEGER)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateInteger(intValue,toggleTitle, min,max,tip);
}
diaElemInteger ::~diaElemInteger()
{
	ADM_assert(Factory); 
	Factory->DestroyInteger(internalPointer);
	internalPointer=NULL;
}
void      diaElemInteger::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemInteger)
// ****************** UInteger ********************
diaElemUInteger ::diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip) :
	diaElem(ELEM_INTEGER)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateUInteger(intValue,toggleTitle, min,max,tip);
}
diaElemUInteger ::~diaElemUInteger()
{
	ADM_assert(Factory); 
	Factory->DestroyUInteger(internalPointer);
	internalPointer=NULL;
}
void      diaElemUInteger::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemUInteger)
// ****************** diaElemNotch ********************
diaElemNotch ::diaElemNotch(uint32_t yes,const char *toggleTitle, const char *tip):
	diaElem(ELEM_NOTCH)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateNotch(yes,toggleTitle,tip);
}
diaElemNotch ::~diaElemNotch()
{
	ADM_assert(Factory); 
	Factory->DestroyNotch(internalPointer);
	internalPointer=NULL;
}

DIA_MKSTUBS(diaElemNotch)
// ****************** diaReadonlyText ********************
diaElemReadOnlyText ::diaElemReadOnlyText(const char *readyOnly,const char *toggleTitle,const char *tip):
	diaElem(ELEM_ROTEXT)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateReadonlyText(readyOnly,toggleTitle,tip);
}
diaElemReadOnlyText ::~diaElemReadOnlyText()
{
	ADM_assert(Factory); 
	Factory->DestroyReadonlyText(internalPointer);
	internalPointer=NULL;
}

DIA_MKSTUBS(diaElemReadOnlyText)
// ****************** diaText ********************
diaElemText ::diaElemText(char **readyOnly,const char *toggleTitle,const char *tip):
	diaElem(ELEM_TEXT)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateText(readyOnly,toggleTitle,tip);
}
diaElemText ::~diaElemText()
{
	ADM_assert(Factory); 
	Factory->DestroyText(internalPointer);
	internalPointer=NULL;
}
void      diaElemText::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemText)
// ******************************************	

diaElemHex ::diaElemHex(const char *toggleTitle, uint32_t dataSize,uint8_t *data):
	diaElem(ELEM_HEXDUMP)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateHex(toggleTitle,dataSize,data);
}
diaElemHex ::~diaElemHex()
{
	ADM_assert(Factory); 
	Factory->DestroyHex(internalPointer);
	internalPointer=NULL;
}
void      diaElemHex::finalize(void)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->finalize(); 
	}
DIA_MKSTUBS(diaElemHex)
// ****************** diaElemMatrix ********************
diaElemMatrix ::diaElemMatrix(uint8_t *trix,const char *toggleTitle, uint32_t trixSize,const char *tip):
	diaElem(ELEM_MATRIX)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateMatrix(trix,toggleTitle,trixSize,tip);
}
diaElemMatrix ::~diaElemMatrix()
{
	ADM_assert(Factory); 
	Factory->DestroyMatrix(internalPointer);
	internalPointer=NULL;
}
void      diaElemMatrix::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
DIA_MKSTUBS(diaElemMatrix)
// ****************** diaElemMenu ********************
diaElemMenu ::diaElemMenu(uint32_t *intValue,const char *itle, uint32_t nb, 
        const diaMenuEntry *menu,const char *tip):
	diaElem(ELEM_MENU)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateMenu(intValue, itle,  nb,  menu,tip);
}
diaElemMenu ::~diaElemMenu()
{
	ADM_assert(Factory); 
	Factory->DestroyMenu(internalPointer);
	internalPointer=NULL;
}
void      diaElemMenu::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
uint8_t   diaElemMenu::link(diaMenuEntry *entry,uint32_t onoff,diaElem *w)
{
	diaElemMenu *cast=(diaElemMenu *)internalPointer;
	cast->link(entry,onoff,w);
}
void      diaElemMenu::updateMe(void)
{
	

}
void   diaElemMenu::finalize(void)
{
	internalPointer->finalize();
}
DIA_MKSTUBS(diaElemMenu)
// ****************** diaElemMenuDynamic ********************
diaElemMenuDynamic ::diaElemMenuDynamic(uint32_t *intValue,const char *itle, uint32_t nb, 
         diaMenuEntryDynamic **menu,const char *tip):
	diaElem(ELEM_MENU)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateMenuDynamic(intValue, itle,  nb,  menu,tip);
}
diaElemMenuDynamic ::~diaElemMenuDynamic()
{
	ADM_assert(Factory); 
	Factory->DestroyMenuDynamic(internalPointer);
	internalPointer=NULL;
}
void      diaElemMenuDynamic::enable(uint32_t onoff)
	{ 
		ADM_assert(internalPointer); 
		internalPointer->enable(onoff); 
	}
uint8_t   diaElemMenuDynamic::link(diaMenuEntryDynamic *entry,uint32_t onoff,diaElem *w)
{
	diaElemMenuDynamic *cast=(diaElemMenuDynamic *)internalPointer;
	cast->link(entry,onoff,w);
}
void   diaElemMenuDynamic::finalize(void)
{
	internalPointer->finalize();
}
void      diaElemMenuDynamic::updateMe(void)
{
	

}

DIA_MKSTUBS(diaElemMenuDynamic)
// ****************** diaElemMatrix ********************
diaElemThreadCount ::diaElemThreadCount(uint32_t *value, const char *title, const char *tip):
	diaElem(ELEM_THREAD_COUNT)
{
	ADM_assert(Factory); 
	internalPointer=Factory->CreateThreadCount(value,title,tip);
}
diaElemThreadCount ::~diaElemThreadCount()
{
	ADM_assert(Factory); 
	Factory->DestroyThreadCount(internalPointer);
	internalPointer=NULL;
}
DIA_MKSTUBS(diaElemThreadCount)


// EOF
