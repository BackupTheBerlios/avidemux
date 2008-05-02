/** *************************************************************************
                          \fn ADM_videoFilterDynamic
                          \brief Include for external dynamically loaded video filters
	(C) 2008 Mean fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef VIDEO_FILTERS_DYNAMIC_H
 #define VIDEO_FILTERS_DYNAMIC_H

#include "ADM_videoFilter.h"
#include "ADM_videoFilter_internal.h"

typedef FilterDescriptor *(VF_getDescriptor)(void);

#define VF_DEFINE_FILTER(myClass,myParam,name,displayname,version,category,description) \
    SCRIPT_CREATE(name##_script,myClass,myParam); \
    BUILD_CREATE(name##_create,myClass); \
	static FilterDescriptor descriptor_vf_id_##myClass (\
								0, \
								#name, \
								displayname, \
								description, \
								category, \
								name##_create, \
								name##_script, \
								ADM_FILTER_API_VERSION); \
    extern "C" { 	FilterDescriptor *ADM_VF_getDescriptor(void) {return &descriptor_vf_id_##myClass ;}};							

/* Hook, filters cannot include config.h as they are framework independant */
#ifdef QT_TR_NOOP
#undef QT_TR_NOOP
#endif
// FIXME TODO extern const char* translate(const char *__domainname, const char *__msgid);
#define QT_TR_NOOP(String) String //translate ("avidemux", String)
#define ADM_MINIMAL_UI_INTERFACE

#endif
