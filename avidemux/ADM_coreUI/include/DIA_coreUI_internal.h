/***************************************************************************
                          DIA_coreUI_internal.h
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
#ifndef DIA_COREUI_INTERNAL_H
#define DIA_COREUI_INTERNAL_H
#include "DIA_enter.h"
#include "DIA_fileSel.h"
#include "DIA_factory.h"
 
// Dia enter
typedef struct
{
	DIA_FILE_INIT	  *fileInit;
	DIA_FILE_SEL_CB   *fileReadCb;
	DIA_FILE_SEL_CB   *fileWriteCb;
	DIA_FILE_SEL_NAME *fileReadName;
	DIA_FILE_SEL_NAME *fileWriteName;
	DIA_FILE_SELECT   *fileSelectRead;
	DIA_FILE_SELECT   *fileSelectWrite;
	DIA_FILE_SELECT   *fileSelectDirectory;
}DIA_FILESEL_DESC_T;

// Call this to hook your fileSelector functions
uint8_t DIA_fileSelInit(DIA_FILESEL_DESC_T *d);


//
typedef uint8_t DIA_FACTORY_RUN_TABS(const char *title,uint32_t nb,diaElemTabs **tabs);
typedef uint8_t DIA_FACTORY_RUN(const char *title,uint32_t nb,diaElem **elems);
typedef struct
{
	DIA_FACTORY_RUN      *FactoryRun;
	DIA_FACTORY_RUN_TABS *FactoryRunTab;
//	
	CREATE_BUTTON_T 	 *CreareButton;
	DELETE_DIA_ELEM_T    *DestroyButton;
//
	CREATE_BAR_T		 *CreateBar;
	DELETE_DIA_ELEM_T    *DestroyBar;
// Float
	CREATE_FLOAT_T       *CreateFloat;
	DELETE_DIA_ELEM_T    *DestroyFloat;
// Integer	
	CREATE_INTEGER_T     *CreateInteger;
	DELETE_DIA_ELEM_T    *DestroyInteger;
// UInteger	
	CREATE_UINTEGER_T    *CreateUInteger;
	DELETE_DIA_ELEM_T    *DestroyUInteger;
	
}FactoryDescriptor;
//
uint8_t DIA_factoryInit(FactoryDescriptor *d);
#endif
