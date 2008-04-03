/***************************************************************************
                          ADM_OpenDML.cpp  -  description
                             -------------------
	
		OpenDML redordering, convert PTS<->DTS order
		
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

#include "ADM_default.h"
#include "ADM_editor/ADM_Video.h"

#include "fourcc.h"
#include "ADM_openDML.h"


#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_ODML
#include "ADM_osSupport/ADM_debug.h"

#define OPENDML_VERBOSE
uint8_t			OpenDMLHeader::isReordered( void )
{ 
 	return _reordered;
}

uint8_t OpenDMLHeader::reorder( void )
{
  
#define INDEX_TMPL        odmlIndex
#define INDEX_ARRAY_TMPL  _idx
#define FRAMETYPE_TMPL    intra
  
#include "ADM_video/ADM_reorderTemplate.cpp"

#undef INDEX_TMPL       
#undef INDEX_ARRAY_TMPL 
#undef FRAMETYPE_TMPL   
             
}
