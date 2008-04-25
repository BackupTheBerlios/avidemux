/***************************************************************************
                             ADM_pluginLoad.cpp

    begin                : Mon Apr 14 2008
    copyright            : (C) 2008 by gruntster/mean
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
#include <vector>

#include "DIA_fileSel.h"
#include "ADM_pluginLoad.h"
#include "ADM_vidEncode.hxx"

struct COMPRES_PARAMS *AllVideoCodec = NULL;
int AllVideoCodecCount = 0;

extern COMPRES_PARAMS *internalVideoCodec[];
extern int getInternalVideoCodecCount();

#if 1
#define aprintf printf
#else
#define aprintf(...) {}
#endif

ADM_vidEnc_plugin::ADM_vidEnc_plugin(const char *file) : ADM_LibWrapper()
{
	initialised = (loadLibrary(file) && getSymbols(20,
		&getEncoders, "vidEncGetEncoders",
		&getEncoderName, "vidEncGetEncoderName",
		&getEncoderType, "vidEncGetEncoderType",
		&getEncoderDescription, "vidEncGetEncoderDescription",
		&getFourCC, "vidEncGetFourCC",
		&getEncoderApiVersion, "vidEncGetEncoderApiVersion",
		&getEncoderVersion, "vidEncGetEncoderVersion",
		&getEncoderGuid, "vidEncGetEncoderGuid",
		&isConfigurable, "vidEncIsConfigurable",
		&configure, "vidEncConfigure",
		&getOptions, "vidEncGetOptions",
		&setOptions, "vidEncSetOptions",
		&getPassCount, "vidEncGetPassCount",
		&getCurrentPass, "vidEncGetCurrentPass",
		&getEncoderGuid, "vidEncGetEncoderGuid",
		&open, "vidEncOpen",
		&beginPass, "vidEncBeginPass",
		&encodeFrame, "vidEncEncodeFrame",
		&finishPass, "vidEncFinishPass",
		&close, "vidEncClose"));
}

std::vector<ADM_vidEnc_plugin *> ADM_videoEncoderPlugins;

static int loadVideoPlugin(int uiType, const char *file)
{
	ADM_vidEnc_plugin *plugin = new ADM_vidEnc_plugin(file);
	int* encoderIds;
	int encoderCount;
	bool success = false;

	if (plugin->isAvailable())
	{
		// Retrieve video encoders
		encoderCount = plugin->getEncoders(uiType, &encoderIds);

		for (int encoderIndex = 0; encoderIndex < encoderCount; encoderIndex++)
		{
			int encoderId = encoderIds[encoderIndex];
			int apiVersion = plugin->getEncoderApiVersion(encoderId);

			if (apiVersion == ADM_VIDENC_API_VERSION)
			{
				plugin->encoderId = encoderId;
				plugin->fileName = ADM_GetFileName(file);

				int major, minor, patch;

				plugin->getEncoderVersion(encoderId, &major, &minor, &patch);

				printf("[ADM_vidEnc_plugin] Plugin loaded version %d.%d.%d, filename %s, desc: %s\n", major, minor, patch, plugin->fileName, plugin->getEncoderDescription(encoderId));

				ADM_videoEncoderPlugins.push_back(plugin);

				success = true;
			}
			else
				printf("[ADM_vidEnc_plugin] File %s has an outdated API version (%d vs %d)\n", ADM_GetFileName(file), apiVersion, ADM_VIDENC_API_VERSION);
		}		
	}
	else
		printf("[ADM_vidEnc_plugin] Unable to load %s\n", ADM_GetFileName(file));

	return success;
}
/**
 * 	\fn ADM_vidEnc_loadPlugins
 *  \brief load all audio plugins
 */
int ADM_vidEnc_loadPlugins(int uiType, const char *path)
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

	memset(files, 0, sizeof(char *)*MAX_EXTERNAL_FILTER);
	printf("[ADM_vidEnc_plugin] Scanning directory %s\n", path);
	fflush(stdout);

	if (!buildDirectoryContent(&nbFile, path, files, MAX_EXTERNAL_FILTER, SHARED_LIB_EXT))
		printf("[ADM_vidEnc_plugin] Cannot parse plugin\n");

	for (int i = 0; i < nbFile; i++)
		loadVideoPlugin(uiType, files[i]);

	printf("[ADM_vidEnc_plugin] Scanning done, found %d codec\n", ADM_videoEncoderPlugins.size());

	AllVideoCodecCount = ADM_videoEncoderPlugins.size() + getInternalVideoCodecCount();
	AllVideoCodec = new COMPRES_PARAMS[AllVideoCodecCount];

	// Copy over internal codecs
	int internalCodecCount = getInternalVideoCodecCount();

	for (int i = 0; i < internalCodecCount; i++)
		memcpy(&AllVideoCodec[i], internalVideoCodec[i], sizeof(COMPRES_PARAMS));

	// Add external codecs
	for (int i = 0; i < ADM_videoEncoderPlugins.size(); i++)
	{
		ADM_vidEnc_plugin *plugin = ADM_videoEncoderPlugins[i];
		ADM_assert(plugin);

		const char* codecName = plugin->getEncoderName(plugin->encoderId);
		const char* codecType = plugin->getEncoderType(plugin->encoderId);
		char* displayName = new char[strlen(codecName) + strlen(codecType) + 4];

		sprintf(displayName, "%s (%s)", codecType, codecName);

		COMPRES_PARAMS *params = &AllVideoCodec[internalCodecCount + i];

		params->codec = CodecExternal;
		params->menuName = displayName;
		params->tagName = displayName;
		params->extra_param = i;

		int length = plugin->getOptions(plugin->encoderId, NULL, 0);
		char *options = new char[length + 1];

		plugin->getOptions(plugin->encoderId, options, length);
		options[length] = 0;

		params->extraSettings = options;
		params->extraSettingsLen = length;
	}

	return 1;
}

ADM_vidEnc_plugin* ADM_vidEnc_getPlugin(int index)
{
	ADM_assert(index < ADM_videoEncoderPlugins.size());

	return ADM_videoEncoderPlugins[index];
}

//EOF
