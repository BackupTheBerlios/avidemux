/**
        \file ADM_pluginLoad.cpp
        \brief Interface for dynamically loaded audio decoder
        
        There are 2 known problem here
        1: The destructor is called instead of calling destroy in the class factory
        2: Memory leak, ADM_audioPlugins is not destroyed as of today
        
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ADM_default.h"
#include "ADM_ad_plugin.h"
#include "DIA_fileSel.h"
#include "ADM_dynamicLoading.h"
#include <vector>
#if 1
#define aprintf printf
#else
#define aprintf(...) {}
#endif

/**
 * 
 */
class ADM_ad_plugin
{
public:
	ADM_ad_CreateFunction		*create;
	ADM_ad_DeleteFunction		*destroy;
	ADM_ad_SupportedFormat		*supportedFormat;
	ADM_ad_GetApiVersion		*getApiVersion;
	ADM_ad_GetDecoderVersion	*getDecoderVersion;
	ADM_ADM_ad_GetInfo			*getInfo;
	ADM_LibWrapper				*wrapper;
	const char 					*name;
};

std::vector<ADM_ad_plugin *> ADM_audioPlugins;
/**
 * 	\fn tryLoadingAudioPlugin
 *  \brief try to load the plugin given as argument..
 */
static uint8_t tryLoadingAudioPlugin(const char *file)
{
	ADM_ad_plugin blank;
	//printf("[ADM_ad_plugin] Scanning %s\n",ADM_GetFileName(file));
	ADM_LibWrapper *wrapper=new ADM_LibWrapper;
	 if(true!=wrapper->loadLibrary(file))
	 {
		 printf("[ADM_ad_plugin] LoadLibrary failed for %s\n",ADM_GetFileName(file));
		 goto Err_ad;
	 }
#define FUNCKY(a,b,c)	 blank.a=(b *)wrapper->getSymbol(#c); if(!blank.a) { printf("[ADM_ad_plugin]"#c" failed\n"); goto Err_ad;;}
	 FUNCKY(create,ADM_ad_CreateFunction,create);
	 FUNCKY(destroy,ADM_ad_DeleteFunction,destroy);
	 FUNCKY(supportedFormat,ADM_ad_SupportedFormat,supportedFormat);
	 FUNCKY(getApiVersion,ADM_ad_GetApiVersion,getApiVersion);
	 FUNCKY(getDecoderVersion,ADM_ad_GetDecoderVersion,getDecoderVersion);
	 FUNCKY(getInfo,ADM_ADM_ad_GetInfo,getInfo);
	 
	 // Check API version
	 if(blank.getApiVersion()!=AD_API_VERSION)
	 {
		 	printf("[ADM_ad_plugin] File %s has API version too old (%d vs %d)\n",
		 			ADM_GetFileName(file),ADM_GetFileName(file),AD_API_VERSION);
		 	 goto Err_ad;
	 }
	 // Get infos
	 uint32_t major,minor,patch;
	 const char *desc;
	 blank.getDecoderVersion(&major,&minor,&patch);
	 blank.wrapper=wrapper;
	 blank.name=ADM_strdup(ADM_GetFileName(file));
	 desc=blank.getInfo();
	 // Print out stuff
	 printf("[ADM_ad_plugin] Plugin loaded version %d.%d.%d, name %s,desc : %s ",
			 		major,minor,patch,blank.name,desc);
	 
	 //
	 {
		 ADM_ad_plugin *p=new ADM_ad_plugin;
		 *p=blank;
		 ADM_audioPlugins.push_back(p);
	 }
	 return 1;
Err_ad:
	delete wrapper;
	return 0;
	
}
/**
 * 	\fn ADM_ad_loadPlugins
 *  \brief load all audio plugins
 */
uint8_t ADM_ad_loadPlugins(const char *path)
{
#define MAX_EXTERNAL_FILTER 50

#ifdef __WIN32
#define SHARED_LIB_EXT "dll"
#elif defined(__APPLE__)
#define SHARED_LIB_EXT "dylib"
#else
#define SHARED_LIB_EXT "so"
#endif

	char *files[MAX_EXTERNAL_FILTER];
	uint32_t nbFile;

	memset(files,0,sizeof(char *)*MAX_EXTERNAL_FILTER);
	printf("[ADM_ad_plugin] Scanning directory %s\n",path);

	if(!buildDirectoryContent(&nbFile, path, files, MAX_EXTERNAL_FILTER, SHARED_LIB_EXT))
	{
		printf("[ADM_ad_plugin] Cannot parse plugin\n");
		return 0;
	}

	for(int i=0;i<nbFile;i++)
		tryLoadingAudioPlugin(files[i]);

	printf("[ADM_ad_plugin] Scanning done, found %d codec\n", ADM_audioPlugins.size());

	return 1;
}
/**
 * 	\fn ADM_ad_searchCodec
 *  \brief Try to instantiate a codec that can decode the given stuff
 */
ADM_Audiocodec *ADM_ad_searchCodec(uint32_t fourcc,	WAVHeader *info,uint32_t extraLength,uint8_t *extraData)
{
	for(int i=0;i<ADM_audioPlugins.size();i++)
	{
		ADM_ad_plugin *a=ADM_audioPlugins[i];
		ADM_assert(a);
		ADM_assert(a->supportedFormat);
		aprintf("Format 0x%x : probing %s\n",fourcc,a->name);
		if(a->supportedFormat(fourcc)==true)
		{
			ADM_assert(a->create);
			return a->create(fourcc, info,extraLength,extraData);
		}
	}
	return NULL;
}

//EOF
