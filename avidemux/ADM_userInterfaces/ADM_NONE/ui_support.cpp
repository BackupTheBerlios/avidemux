#include <stdio.h>

#include "config.h"
#include "ADM_inttype.h"
#include "ADM_files.h"
#include "ADM_encoder/ADM_pluginLoad.h"

#ifdef HAVE_GETTEXT
#include <libintl.h>

extern void initGetText(void);

void initTranslator(void)
{
	initGetText();
}

const char* translate(const char *__domainname, const char *__msgid)
{
	return (const char*)dgettext(PACKAGE, __msgid);
}
#else
void initTranslator(void) {}

const char* translate(const char *__domainname, const char *__msgid)
{
	return __msgid;
}
#endif

void getUIDescription(char* desc)
{
	sprintf(desc, "CLI");
}

void loadPlugins(void)
{
	char *pluginDir = ADM_getPluginPath();

	loadVideoEncoderPlugins(ADM_VIDENC_UI_CLI, pluginDir);
	delete [] pluginDir;
}
