/***************************************************************************
                          ADM_videoInternal.h  -  description
    begin                : Thu Apr 18 2008
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef  ADM_videoInternal_H
#define  ADM_videoInternal_H

#define ADM_DEMUXER_API_VERSION 2
#include "ADM_dynamicLoading.h"
#include "ADM_Video.h"
class ADM_demuxer :public ADM_LibWrapper
{
public:
        int         initialised;
        vidHeader   *(*createdemuxer)();
        void         (*deletedemuxer)(vidHeader *demuxer);
        uint8_t      (*getVersion)(uint32_t *major,uint32_t *minor,uint32_t *patch);
        /// Return true if that demuxer can handle that file, lower value means
        /// less likely to be a good demuxer for that
        uint32_t         (*probe)(uint32_t magic, const char *fileName);
        // Only initialized once
        const char    *name;
        const char    *descriptor;
        uint32_t      apiVersion;

        ADM_demuxer(const char *file) : ADM_LibWrapper()
        {
        const char   *(*getDescriptor)();
        uint32_t     (*getApiVersion)();
        const char  *(*getDemuxerName)();

			initialised = (loadLibrary(file) && getSymbols(7,
				&createdemuxer, "create",
				&deletedemuxer, "destroy",
				&probe,         "probe",

				&getDemuxerName, "getName",
				&getApiVersion,  "getApiVersion",
				&getVersion,     "getVersion",
				&getDescriptor,  "getDescriptor"));
                if(initialised)
                {
                    name=getDemuxerName();
                    apiVersion=getApiVersion();
                    descriptor=getDescriptor();
                    printf("[Demuxer]Name :%s ApiVersion :%d Description :%s\n",name,apiVersion,descriptor);
                }else
                {
                    printf("[Demuxer]Symbol loading failed for %s\n",file);
                }
        }
};

#define ADM_DEMUXER_BEGIN( Class,maj,mn,pat,name,desc) \
extern "C" {\
vidHeader   *create(void){ return new Class; } \
void         destroy(vidHeader *h){ Class *z=(Class *)h;delete z;} \
uint8_t      getVersion(uint32_t *major,uint32_t *minor,uint32_t *patch) {*major=maj;*minor=mn;*patch=pat;} \
uint32_t     getApiVersion(void) {return ADM_DEMUXER_API_VERSION;} \
const char  *getName(void) {return name;} \
const char  *getDescriptor(void) {return desc;} \
}

#endif
//EOF
