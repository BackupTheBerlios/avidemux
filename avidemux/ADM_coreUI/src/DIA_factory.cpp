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
// ******************************************	
// EOF
