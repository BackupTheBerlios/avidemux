#include "config.h"

#ifdef HAVE_GETTEXT
#include <stdio.h>
#include <libintl.h>
#include <locale.h>

void initGetText(void)
{
	char *local = setlocale(LC_ALL, "");

#ifdef ADM_WIN32
	bindtextdomain("avidemux", "./share/locale");
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

#ifndef ADM_WIN32
	printf("[Locale] Files for %s appear to be in %s\n","avidemux", ADMLOCALE);
#endif
	printf("[Locale] Test: %s\n\n", dgettext("avidemux", "_File"));
};
#endif
