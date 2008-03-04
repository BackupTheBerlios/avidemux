#include "config.h"

#ifdef HAVE_GETTEXT
#include <stdio.h>
#include <libintl.h>
#include <locale.h>

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

void initGetText(void)
{
	char *local = setlocale(LC_ALL, "");

#ifdef __WIN32
	bindtextdomain("avidemux", "./share/locale");
#elif defined(__APPLE__)
#define MAX_PATH_SIZE 1024
	char buffer[MAX_PATH_SIZE];

	CFURLRef url(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
	buffer[0] = '\0';

	if (url)
	{
		CFURLGetFileSystemRepresentation(url, true, (UInt8*)buffer, MAX_PATH_SIZE);
		CFRelease(url);

		char *slash = strrchr(buffer, '/');
		
		if (slash)
			slash[1] = '\0';
	}

	strcat(buffer, "../Resources/locale");
	bindtextdomain("avidemux", buffer);
#else
	bindtextdomain("avidemux", ADMLOCALE);
#endif

	bind_textdomain_codeset("avidemux", "UTF-8");
  
	if(local)
		printf("\n[Locale] setlocale %s\n", local);

	local = textdomain(NULL);
	textdomain("avidemux");

	if(local)
	    printf("[Locale] Textdomain was %s\n", local);

	local = textdomain(NULL);

	if(local)
		printf("[Locale] Textdomain is now %s\n", local);

#if !defined(__WIN32) && !defined(__APPLE__)
	printf("[Locale] Files for %s appear to be in %s\n","avidemux", ADMLOCALE);
#endif
	printf("[Locale] Test: %s\n\n", dgettext("avidemux", "_File"));
};
#endif
